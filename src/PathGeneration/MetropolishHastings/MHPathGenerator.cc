/*
 * MHPathGenerator.cc
 *
 *  Created on: Apr 26, 2012
 *      Author: jchen
 */

#include "MHPathGenerator.h"
#include "patNBParameters.h"
#include "MHPathWriterWrapper.h"
#include "patNetworkReducer.h"
MHPathGenerator::MHPathGenerator(boost::mt19937& rng):m_rnd(rng) {
	int DEFAULT_MSGINTERVAL = INT_MAX;

	int DEFAULT_BURNINITERATIONS = 0;

	int DEFAULT_SAMPLEINTERVAL = 1;

	m_msgInterval = patNBParameters::the()->MSGINTERVAL_ELEMENT;
	m_randomSeed = patNBParameters::the()->RANDOMSEED_ELEMENT;


	m_totalIterations = patNBParameters::the()->TOTALITERATIONS_ELEMENT;

	m_sampleInterval = patNBParameters::the()->SAMPLEINTERVAL_ELEMENT;

	m_cutOffProbability = patNBParameters::the()->CUTOFFPROBABILITY_ELEMENT;

	m_relativeCostScale = patNBParameters::the()->RELATIVECOSTSCALE_ELEMENT;
	DEBUG_MESSAGE(m_rnd.nextDouble());
	//m_linkAndPathCost.configure();

}

MHPathGenerator::~MHPathGenerator() {
	// TODO Auto-generated destructor stub
}

void MHPathGenerator::setNetwork( patNetworkBase* network_environment) {
	m_network = network_environment;
}

void MHPathGenerator::setPathWriter(patPathWriter* path_writer) {
	m_path_writer = path_writer;
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
	patRouter router(m_network, &m_linkAndPathCost);

	const double nodeLoopScale = m_linkAndPathCost.getNodeLoopScale();
	m_linkAndPathCost.setNodeLoopScale(0.0);

	linkCostSP = router.fwdCost(origin, destination).getLabel(destination);
	m_linkAndPathCost.setNodeLoopScale(nodeLoopScale);

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
		double expansion = 1.0
				- log(m_cutOffProbability)
						/ m_linkAndPathCost.getLinkCostScale() / linkCostSP;
		patNetworkReducer nr(origin, destination, &m_linkAndPathCost,
				expansion);
		nr.reduce(m_network);
		m_network->exportKML("reduced.kml");
	}

	DEBUG_MESSAGE("network reduced");
	/*
	 * (6) run the algorithm
	 */

	MHPathProposal proposal(origin, destination, &router,
			m_linkAndPathCost.getLinkCostScale(), &m_rnd);


	DEBUG_MESSAGE("proposal initiated");

	MHAlgorithm<MHPath> algo(&proposal, &m_linkAndPathCost, &m_rnd);

	DEBUG_MESSAGE("MA initiated");
	algo.setMsgInterval(m_msgInterval);
	MHPathWriterWrapper pww(m_path_writer, m_sampleInterval,
			&m_linkAndPathCost);
	DEBUG_MESSAGE("path writter initiated");
	algo.addStateProcessor(&pww);

	DEBUG_MESSAGE("state added");
	algo.run(m_totalIterations);
	DEBUG_MESSAGE("finish");
}
