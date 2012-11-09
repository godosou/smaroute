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
				NULL), m_path_writer(NULL), m_utility_function(NULL) {
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
		m_nip_calculator(other.m_nip_calculator), m_rnd(other.m_rnd), m_writter_wrapper(
				other.m_writter_wrapper), m_path_writer(other.m_path_writer), m_utility_function(
				other.m_utility_function) {
	if (other.m_MHWeight != NULL) {
		m_MHWeight = other.m_MHWeight->clone();
	} else {
		m_MHWeight = NULL;
	}
	if (other.m_router_cost != NULL) {
		m_router_cost = other.m_router_cost->clone();
	} else {
		m_router_cost = NULL;
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
	if (m_router_cost != NULL) {
		delete m_router_cost;
		m_router_cost = NULL;
	}
}

void MHPathGenerator::setPathWriter(patPathWriter* path_writer) {
	m_path_writer = path_writer;
}
void MHPathGenerator::setUtilityFunction(
		const patLinkAndPathCost* utility_function) {
	m_utility_function = utility_function;
}
void MHPathGenerator::setRouterLinkCost(const patLinkAndPathCost* linkCost) {
	m_router_cost = linkCost->clone();
}

void MHPathGenerator::setMHWeight(const MHWeightFunction* MHWeight) {
	m_MHWeight = MHWeight->clone();
}
double MHPathGenerator::calculatePathLogWeight(
		const patMultiModalPath& path) const {
	return m_MHWeight->logWeigthOriginal(path);
}

void calibrateMHWeight(const patMultiModalPath& sp_path);

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
			fwdTree.getShortestPathTo(sp_roads, node_iter->first);
			bwdTree.getShortestPathTo(sp_roads, node_iter->first);
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


void MHPathGenerator::calibrate(const patNode* origin, const patNode* destination){
	patRouter start_router(m_network, m_router_cost);

	patShortestPathTreeGeneral sp_tree(FWD);
	start_router.fwdCost(sp_tree, origin, destination);

	list<const patRoadBase*> list_of_roads;
	sp_tree.getShortestPathTo(list_of_roads, destination);

	patMultiModalPath sp_path(list_of_roads);


	m_MHWeight->calibrate(sp_path);
	m_router_cost->calibrate(sp_path);

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

	stringstream od_stream;
	od_stream << origin->getUserId() << "_" << destination->getUserId()
			<< ".kml";
	cout << "MHPathGenerator sp cost:" << linkCostSP << endl;
	patKMLPathWriter sp_writer(od_stream.str());
	map < string, string > attr;
	sp_writer.writePath(sp_path, attr);
	sp_writer.close();
	calibrate(origin,destination);

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
	patNetworkCompressor* nc = NULL;
	if (m_cutOffProbability > 0.0) {

//		double expansion_scale = log(2.0) / linkCostSP
//				/ (m_relativeCostScale - 1.0);
//		double expansion = 1.0
//				- log(m_cutOffProbability) / expansion_scale / linkCostSP;
		double expansion = 1.0 - log(m_cutOffProbability) / (linkCostSP);
		if (patNBParameters::the()->mh_link_scale_relative == 1) {
			expansion = 1.0
					- log(m_cutOffProbability)
							* (patNBParameters::the()->mh_link_scale - 1.0)
							/ (log(2));
		}
		patNetworkReducer nr(origin, destination, m_router_cost, expansion);
		nr.reduce(m_network);

		cout << "network reduced NODE SIZE:" << (m_network->getNodeSize());
		cout << " ARC SIZE:" << (m_network->getAllArcs().size()) << endl;
		if (patNBParameters::the()->exportReducedNetwork == 1) {
			if (m_path_writer != NULL) {
				m_network->exportShpFiles(
						m_path_writer->getFileName() + "reduced.shp");
				m_network->exportKML(
						m_path_writer->getFileName() + "reduced.kml");
			} else {
				m_network->exportKML("reduced.kml");
			}
		}
	}

	if (patNBParameters::the()->compressNetwork == 1) {
		unordered_set<const patNode*> uncompressed_nodes;
		uncompressed_nodes.insert(origin);
		uncompressed_nodes.insert(destination);
		nc = new patNetworkCompressor(m_network, uncompressed_nodes);
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

	}
	patRouter router(m_network, m_router_cost);

	list<const patRoadBase*> update_list_of_roads;
	patShortestPathTreeGeneral updatesp_tree(FWD);
	router.fwdCost(updatesp_tree, origin, destination);
	updatesp_tree.getShortestPathTo(update_list_of_roads, destination);
	patMultiModalPath updatesp_path(update_list_of_roads);

	cout << "original sp:" << sp_path.getArcString() << endl;

	cout << "compressed sp:" << updatesp_path.getArcString() << endl;
	if (updatesp_path != sp_path) {
		throw RuntimeException("wrong: two sp paths no equal");
	}
	/*
	 * (6) run the algorithm
	 */
	if (m_network->getOutgoingIncidents()->find(origin)
			== m_network->getOutgoingIncidents()->end()) {
		cout << "origin" << origin->getUserId() << " not in network" << endl;
	}
	if (m_network->getIncomingIncidents()->find(destination)
			== m_network->getIncomingIncidents()->end()) {
		cout << "destination" << destination->getUserId() << " not in network"
				<< endl;
	}

	cout << "---DERIVE NODE INSERTTION PROBABILITY---" << endl;
	m_nip_calculator->setParams(&router, m_network, m_MHWeight);
	unordered_map<const patNode*, double> proposal_probabilities =
			m_nip_calculator->calculate(origin, destination);

	cout << "---DONE NODE INSERTTION PROBABILITY---" << endl;

	patMultiModalPath init_path;
	if (patNBParameters::the()->mh_trigger_path == "most likely") {
		cout << "trigger with most likely observation" << endl;
		init_path = m_MHWeight->getMostLikelyPath(updatesp_path);
	} else {
		cout << "trigger with shortest path" << endl;
		init_path = updatesp_path;
	}
	cout << "init path:" << init_path.getArcString() << endl;

	patMultiModalPath new_init_path = m_network->recoverPath(init_path);
	cout << "recover init" << new_init_path.getArcString() << endl;

	if (new_init_path.getArcString()!=init_path.getArcString()){
		throw RuntimeException(m_path_writer->getFileName().c_str());
	}
	MHPathProposal proposal(origin, destination, &router,
			proposal_probabilities, 1.0, &m_rnd, new_init_path);

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
					m_utility_function);
			algo.addStateProcessor(&pww);

			algo.run(
					patNBParameters::the()->WARMUP_ITERATIONS
							+ (m_total_samples) * m_sampleInterval - 1);

		} else {
			throw RuntimeException("invalid path writter.");
		}
	}
	if (nc != NULL) {
		delete nc;
		nc = NULL;
	}
}

void MHPathGenerator::setWritterWrapper(
		MHStateProcessor<MHPath>* writter_wrapper) {
	m_writter_wrapper = writter_wrapper;
}

void MHPathGenerator::setSampleCount(const unsigned long& sample_count) {
	m_total_samples = sample_count;
}
;
void MHPathGenerator::setNIPCalculator(
		MHNodeInsertionProbability* nip_calculator) {
	m_nip_calculator = nip_calculator;
}
