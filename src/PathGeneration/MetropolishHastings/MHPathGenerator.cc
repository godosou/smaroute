/*
 * MHPathGenerator.cc
 *
 *  Created on: Apr 26, 2012
 *      Author: jchen
 */
#include <iostream>

#include "MHPathGenerator.h"
#include "patNBParameters.h"
#include "MHPathWriterWrapper.h"
#include "patNetworkReducer.h"
#include "patNetworkCar.h"
#include "patKMLPathWriter.h"
#include "patNetworkCompressor.h"
MHPathGenerator::MHPathGenerator(const patRandomNumber& rnd) :
		patPathGenerator::patPathGenerator(), m_rnd(rnd), m_writter_wrapper(
				NULL), m_path_writer(NULL) {
	m_msgInterval = patNBParameters::the()->MSGINTERVAL_ELEMENT;
	m_randomSeed = patNBParameters::the()->RANDOMSEED_ELEMENT;

	m_total_samples = patNBParameters::the()->SAMPLE_COUNT;
	m_totalIterations = patNBParameters::the()->TOTALITERATIONS_ELEMENT;

	m_sampleInterval = patNBParameters::the()->SAMPLEINTERVAL_ELEMENT;

	m_cutOffProbability = patNBParameters::the()->CUTOFFPROBABILITY_ELEMENT;

	m_relativeCostScale = patNBParameters::the()->RELATIVECOSTSCALE_ELEMENT;
//	DEBUG_MESSAGE(m_rnd.nextDouble());
	//m_router_cost.configure();

}

MHPathGenerator::MHPathGenerator(MHPathGenerator const& other) :
		m_rnd(other.m_rnd), m_router_cost(other.m_router_cost), m_writter_wrapper(
				other.m_writter_wrapper), m_path_writer(other.m_path_writer) {
	if (other.m_MHWeight != NULL) {
		m_MHWeight = other.m_MHWeight->clone();
	} else {
		m_MHWeight = NULL;
	}
	setNetwork(other.m_network);
	int DEFAULT_MSGINTERVAL = INT_MAX;

	int DEFAULT_BURNINITERATIONS = 0;

	int DEFAULT_SAMPLEINTERVAL = 1;

	m_msgInterval = other.m_msgInterval;
	m_randomSeed = other.m_randomSeed;

	m_total_samples = other.m_total_samples;
	m_totalIterations = other.m_totalIterations;

	m_sampleInterval = other.m_sampleInterval;

	m_cutOffProbability = other.m_cutOffProbability;

	m_relativeCostScale = other.m_relativeCostScale;
//	DEBUG_MESSAGE(m_rnd.nextDouble());

}
MHPathGenerator::~MHPathGenerator() {
	if (m_MHWeight != NULL) {
		delete m_MHWeight;
		m_MHWeight = NULL;
	}
}

void MHPathGenerator::setPathWriter(patPathWriter* path_writer) {
	m_path_writer = path_writer;
}

void MHPathGenerator::setRouterLinkCost(const patLinkAndPathCost* linkCost) {
	m_router_cost = linkCost;
}

void MHPathGenerator::setMHWeight(const MHWeightFunction* MHWeight) {
	m_MHWeight = MHWeight->clone();
}
double MHPathGenerator::calculatePathLogWeight(
		const patMultiModalPath& path) const {
	return m_MHWeight->logWeigthOriginal(path);
}

void MHPathGenerator::getNodeProbability(patRouter& router,
		unordered_map<const patNode*, double>& proposal_probabilities,
		const patNode* origin, const patNode* destination) {

	//==============Start derive insertion probability for each node==============//
	patShortestPathTreeGeneral fwdTree(FWD);
	router.fwdCost(fwdTree, origin);
	patShortestPathTreeGeneral bwdTree(BWD);
	router.bwdCost(bwdTree, destination);

	const unordered_map<const patNode*, double>& fwdCost = fwdTree.getLabels();
	const unordered_map<const patNode*, double>& bwdCost = bwdTree.getLabels();
	double minCost = DBL_MAX;

	set<const patNode*> all_nodes = m_network->getNodes();
	for (unordered_map<const patNode*, double>::const_iterator node_iter =
			fwdCost.begin(); node_iter != fwdCost.end(); ++node_iter) {
		unordered_map<const patNode*, double>::const_iterator bc_iter =
				bwdCost.find(node_iter->first);
		if (bc_iter != bwdCost.end() && bc_iter->second != DBL_MAX
		&& node_iter->second != DBL_MAX) {
			list<const patRoadBase*> sp_roads;
			fwdTree.getShortestPathTo(sp_roads,node_iter->first);
			bwdTree.getShortestPathTo(sp_roads,node_iter->first);
			patMultiModalPath sp(sp_roads);
			double cost = m_MHWeight->logWeigthOriginal(sp);
//			double cost = node_iter->second + bc_iter->second;
			minCost = cost < minCost ? cost : minCost;
			proposal_probabilities[node_iter->first] = cost;

		}
		//TODO add obs weight
	}
	double weightSum = 0.0;

	for (unordered_map<const patNode*, double>::iterator node_iter =
			proposal_probabilities.begin();
			node_iter != proposal_probabilities.end(); ++node_iter) {
		double weight = exp(node_iter->second - minCost); //weight
		weightSum += weight;
		node_iter->second = weight;
	}
	for (unordered_map<const patNode*, double>::iterator node_iter =
			proposal_probabilities.begin();
			node_iter != proposal_probabilities.end(); ++node_iter) {
		node_iter->second /= weightSum;
//		cout<<node_iter->first->getUserId()<< ":"<<node_iter->second<<endl;
	}
	//=========END drive insertion probaiblity=======================//
}
void MHPathGenerator::run(const patNode* origin, const patNode* destination) {

	//MHLinkAndPathCost::configure();
	/*
	 * (3) compute shortest path link cost
	 */
	double linkCostSP;
	patRouter start_router(m_network, m_router_cost);

	patShortestPathTreeGeneral sp_tree(FWD);
	start_router.fwdCost(sp_tree, origin, destination);
	linkCostSP = sp_tree.getLabel(destination);

	list<const patRoadBase*> list_of_roads;
	sp_tree.getShortestPathTo(list_of_roads, destination);

	patMultiModalPath sp_path(list_of_roads);

	patKMLPathWriter sp_writer("sp.kml");
	map<string, string> attr;
	sp_writer.writePath(sp_path, attr);
	sp_writer.close();

	/*
	 * (4) compute new scale if it is defined in relative terms
	 */
	/*
	 if (m_relativeCostScale != 0.0) {
	 m_router_cost.setLinkCostScale(
	 log(2.0) / linkCostSP / (m_relativeCostScale - 1.0));
	 }
	 */

	/*
	 * (5) reduce the network if a positive cutoff probability is specified
	 */
	patNetworkCompressor* nc=NULL;
	if (m_cutOffProbability > 0.0) {

//		double expansion_scale = log(2.0) / linkCostSP
//				/ (m_relativeCostScale - 1.0);
//		double expansion = 1.0
//				- log(m_cutOffProbability) / expansion_scale / linkCostSP;
		double expansion = 1.0 - log(m_cutOffProbability) / (linkCostSP);
		patNetworkReducer nr(origin, destination, m_router_cost, expansion);
		nr.reduce(m_network);

		cout << "network reduced NODE SIZE:" << (m_network->getNodeSize());
		cout << " ARC SIZE:" << (m_network->getAllArcs().size()) << endl;
		if (patNBParameters::the()->exportReducedNetwork == 1) {
			if (m_path_writer != NULL) {
				m_network->exportKML(
						m_path_writer->getFileName() + "reduced.kml");
			} else {

				m_network->exportKML("reduced.kml");
			}
		}
	}

	unordered_set<const patNode*> uncompressed_nodes;
	uncompressed_nodes.insert(origin);
	uncompressed_nodes.insert(destination);
	nc = new patNetworkCompressor(m_network,uncompressed_nodes);
	nc->compress(m_router_cost->getLinkCoefficients());
	cout << "network compressed NODE SIZE:" << (m_network->getNodeSize());
	cout << " ARC SIZE:" << (m_network->getAllArcs().size()) << endl;
	if (patNBParameters::the()->exportReducedNetwork == 1) {
		if (m_path_writer != NULL) {
			m_network->exportKML(
					m_path_writer->getFileName() + "compressed.kml");
		} else {

			m_network->exportKML("compressed.kml");
		}
	}
//	nc->compress(m_router_cost->getLinkCoefficients());
//	cout << "network compressed NODE SIZE:" << (m_network->getNodeSize());
//	cout << " ARC SIZE:" << (m_network->getAllArcs().size()) << endl;
//	if (patNBParameters::the()->exportReducedNetwork == 1) {
//		if (m_path_writer != NULL) {
//			m_network->exportKML(
//					m_path_writer->getFileName() + "compressed_2.kml");
//		} else {
//
//			m_network->exportKML("compressed_2.kml");
//		}
//	}
	m_MHWeight->calculateObsScale(sp_path);
	patRouter router(m_network, m_router_cost);

	/*
	 * (6) run the algorithm
	 */

	cout << "---DERIVE NODE INSERTTION PROBABILITY---" << endl;
	unordered_map<const patNode*, double> proposal_probabilities;
	cout << "---DONE NODE INSERTTION PROBABILITY---" << endl;

	getNodeProbability(router,proposal_probabilities,origin, destination) ;

	MHPathProposal proposal(origin, destination, &router,
			proposal_probabilities, 1.0, &m_rnd);

	MHAlgorithm<MHPath> algo(&proposal, m_MHWeight, &m_rnd);
	algo.setMsgInterval(m_msgInterval);
	if (m_writter_wrapper != NULL) {
		algo.addStateProcessor(m_writter_wrapper);
		algo.run(
				patNBParameters::the()->WARMUP_ITERATIONS
						+ (m_total_samples) * m_sampleInterval);

	} else {
		if (m_path_writer != NULL) {
			MHPathWriterWrapper pww(m_path_writer, m_sampleInterval,
					m_MHWeight);
			algo.addStateProcessor(&pww);

			algo.run(
					patNBParameters::the()->WARMUP_ITERATIONS
							+ (m_total_samples) * m_sampleInterval - 1);

		} else {
			throw RuntimeException("invalid path writter.");
		}
	}
	if(nc!=NULL){
		delete nc;
		nc=NULL;
	}
}

void MHPathGenerator::setWritterWrapper(
		MHStateProcessor<MHPath>* writter_wrapper) {
	m_writter_wrapper = writter_wrapper;
}

void MHPathGenerator::setSampleCount(const unsigned long& sample_count){
	m_total_samples = sample_count;
};
