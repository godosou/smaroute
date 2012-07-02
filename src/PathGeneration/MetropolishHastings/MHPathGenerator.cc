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
MHPathGenerator::MHPathGenerator(unsigned long rng) :
		m_rnd(rng), m_network(NULL) {
	m_msgInterval = patNBParameters::the()->MSGINTERVAL_ELEMENT;
	m_randomSeed = patNBParameters::the()->RANDOMSEED_ELEMENT;

	m_total_samples = patNBParameters::the()->SAMPLE_COUNT;
	m_totalIterations = patNBParameters::the()->TOTALITERATIONS_ELEMENT;

	m_sampleInterval = patNBParameters::the()->SAMPLEINTERVAL_ELEMENT;

	m_cutOffProbability = patNBParameters::the()->CUTOFFPROBABILITY_ELEMENT;

	m_relativeCostScale = patNBParameters::the()->RELATIVECOSTSCALE_ELEMENT;
//	DEBUG_MESSAGE(m_rnd.nextDouble());
	//m_linkAndPathCost.configure();

}

MHPathGenerator::MHPathGenerator(MHPathGenerator const& other) :
		m_rnd(other.m_rnd), m_linkAndPathCost(other.m_linkAndPathCost), m_MHWeight(
				other.m_MHWeight) {
	m_network = other.m_network->clone();
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
	// TODO Auto-generated destructor stub
}

void MHPathGenerator::setNetwork(const patNetworkBase* network_environment) {
	m_network = network_environment->clone();

}

void MHPathGenerator::setPathWriter(patPathWriter* path_writer) {
	m_path_writer = path_writer;
}

void MHPathGenerator::setRouterLinkCost(const patLinkAndPathCost* linkCost) {
	m_linkAndPathCost = linkCost;
}

void MHPathGenerator::setMHWeight(const MHWeightFunction* MHWeight) {
	m_MHWeight = MHWeight;
}
double MHPathGenerator::calculatePathLogWeight(
		const patMultiModalPath& path) const {
	return m_MHWeight->logWeigthOriginal(path);
}
void MHPathGenerator::run(const patNode* origin, const patNode* destination) {

	//MHLinkAndPathCost::configure();
	/*
	 * (3) compute shortest path link cost
	 */
	double linkCostSP;
	/*
	 patNetworkBase *network;
	 *network = *m_network;
	 *network */
//	DEBUG_MESSAGE("try to copy network");
//	DEBUG_MESSAGE("network copied");
//	DEBUG_MESSAGE("network copied"<<network->getNodeSize());
	patRouter start_router(m_network, m_linkAndPathCost);

//	const double nodeLoopScale = m_linkAndPathCost->getNodeLoopScale();
//	m_linkAndPathCost->setNodeLoopScale(0.0);
	patShortestPathTreeGeneral sp_tree(FWD);
	start_router.fwdCost(sp_tree, origin, destination);
	linkCostSP = sp_tree.getLabel(destination);
//	m_linkAndPathCost->setNodeLoopScale(nodeLoopScale);

	/*
	 * (4) compute new scale if it is defined in relative terms
	 */
	/*
	 if (m_relativeCostScale != 0.0) {
	 m_linkAndPathCost.setLinkCostScale(
	 log(2.0) / linkCostSP / (m_relativeCostScale - 1.0));
	 }
	 */

	/*
	 * (5) reduce the network if a positive cutoff probability is specified
	 */
	if (m_cutOffProbability > 0.0) {

//		double expansion_scale = log(2.0) / linkCostSP
//				/ (m_relativeCostScale - 1.0);
//		double expansion = 1.0
//				- log(m_cutOffProbability) / expansion_scale / linkCostSP;
		double expansion = 1.0 - log(m_cutOffProbability) / (linkCostSP);
		cout << m_linkAndPathCost->getLinkCostScale() << "," << linkCostSP
				<< endl;
		patNetworkReducer nr(origin, destination, m_linkAndPathCost, expansion);
		nr.reduce(m_network);
		m_network->exportKML(m_path_writer->getFileName()+".reduced.kml");
		cout << "network reduced NODE SIZE:" << (m_network->getNodeSize());
		cout << " ARC SIZE:" << (m_network->getAllArcs().size()) << endl;
	}

	patRouter router(m_network, m_linkAndPathCost);
	/*
	 * (6) run the algorithm
	 */

	cout << "---DERIVE NODE INSERTTION PROBABILITY---" << endl;
	unordered_map<const patNode*, double> proposal_probabilities;

//	cout << "Compute forward true";
	patShortestPathTreeGeneral fwdTree(FWD);
	router.fwdCost(fwdTree, origin);
//	cout << "Compute backward true";
	patShortestPathTreeGeneral bwdTree(BWD);
	router.bwdCost(bwdTree, destination);

//	cout << "Compute forward/backward costs";
	const unordered_map<const patNode*, double>& fwdCost = fwdTree.getLabels();
	const unordered_map<const patNode*, double>& bwdCost = bwdTree.getLabels();
	double minCost = DBL_MAX;

	set<const patNode*> all_nodes = m_network->getNodes();
	cout << "all nodes: " << all_nodes.size() << " forward: " << fwdCost.size()
			<< "backward" << bwdCost.size() << endl;

	for (unordered_map<const patNode*, double>::const_iterator node_iter =
			fwdCost.begin(); node_iter != fwdCost.end(); ++node_iter) {
		unordered_map<const patNode*, double>::const_iterator bc_iter =
				bwdCost.find(node_iter->first);
		if (bc_iter != bwdCost.end() && bc_iter->second != DBL_MAX
				&& node_iter->second != DBL_MAX) {
			double cost = node_iter->second + bc_iter->second;
			minCost = cost < minCost ? cost : minCost;
			proposal_probabilities[node_iter->first] = cost;

		}
	}
	double weightSum = 0.0;

	for (unordered_map<const patNode*, double>::iterator node_iter =
			proposal_probabilities.begin();
			node_iter != proposal_probabilities.end(); ++node_iter) {
		double weight = exp(-node_iter->second + minCost); //TODO more factors
		weightSum += weight;
		node_iter->second = weight;
	}
	for (unordered_map<const patNode*, double>::iterator node_iter =
			proposal_probabilities.begin();
			node_iter != proposal_probabilities.end(); ++node_iter) {
		node_iter->second /= weightSum;
	}
	cout<<"---NODE INTERSTION DONE---"<<endl;
	MHPathProposal proposal(origin, destination, &router,
			proposal_probabilities, 1.0, &m_rnd);

	MHAlgorithm<MHPath> algo(&proposal, m_MHWeight, &m_rnd);
	algo.setMsgInterval(m_msgInterval);
	MHPathWriterWrapper pww(m_path_writer, m_sampleInterval);
	algo.addStateProcessor(&pww);
//	cout<<
//			"sample cout"<<m_total_samples<<", interval"<<m_sampleInterval<<endl;
	algo.run((m_total_samples) * m_sampleInterval);
	delete m_network;
	m_network = NULL;
}
