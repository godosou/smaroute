/*
 * testMHPath.h
 *
 *  Created on: Apr 26, 2012
 *      Author: jchen
 */

#ifndef TESTMHPATH_H_
#define TESTMHPATH_H_

#include <cxxtest/TestSuite.h>
#include <vector>;
#include "patNBParameters.h"
#include "patError.h"
#include "patDisplay.h"
#include <stdio.h>
#include "patGeoBoundingBox.h"
#include "MHLinkAndPathCost.h"
#include "patRouter.h"
#include "patRandomNumber.h"
#include <boost/random.hpp>
#include "patNetworkEnvironment.h"
#include "patNetworkBase.h"
#include "patMultiModalPath.h"
#include "MHPathProposal.h"
#include "patKMLPathWriter.h"
#include "patNetworkReducer.h"
#include "patTransportMode.h"
#include "patNetworkCar.h"
class MyTestMHPath: public CxxTest::TestSuite {
public:
	void testMHPath(void) {

		patError* err(NULL);
		patNBParameters::the()->readFile(
				"/Users/jchen/Documents/Project/newbioroute/src/config.xml",
				err);
		patNBParameters::the()->init(err);
		TS_ASSERT_EQUALS(err, (void*)0);
		MHLinkAndPathCost::configure();
		patGeoBoundingBox bb = patGeoBoundingBox(6.49409428385,
				46.544856996900002, 6.5770872475999997, 46.510016372300001);
		patNetworkEnvironment network_environment(bb, err);
		DEBUG_MESSAGE("network loaded");
		patNetworkBase *network = new patNetworkCar();
		DEBUG_MESSAGE("new network initiated");
		network_environment.getNetwork(CAR);
		*network = *network_environment.getNetwork(CAR);
		DEBUG_MESSAGE("network copied");
		TS_ASSERT_EQUALS(err, (void*)0);

		MHLinkAndPathCost linkAndPathCost;
		//linkAndPathCost.configure();
		DEBUG_MESSAGE("cost setting configured");
		linkAndPathCost.setNodeLoopScale(0.0);
		patRouter router(network, &linkAndPathCost);

		const patNode* origin_node =
				network_environment.getNetworkElements().getNode(263297446);
		const patNode* destination_node =
				network_environment.getNetworkElements().getNode(263291704);
		TS_ASSERT_DIFFERS(origin_node, (void*)0);
		TS_ASSERT_DIFFERS(destination_node, (void*)0);
		DEBUG_MESSAGE("node read");
		patKMLPathWriter path_writer("paths.kml");

		double linkCostSP =
				router.fwdCost(origin_node, destination_node).getLabel(
						destination_node);
		double linkCostSP_b =
				router.bwdCost(origin_node, destination_node).getLabel(
						origin_node);
		TS_ASSERT_DELTA(linkCostSP, linkCostSP_b,1.0e-4);

		if (patNBParameters::the()->CUTOFFPROBABILITY_ELEMENT) {

			double expansion = 1.0
					- log(patNBParameters::the()->CUTOFFPROBABILITY_ELEMENT)
							/ linkAndPathCost.getLinkCostScale() / linkCostSP;
			DEBUG_MESSAGE(linkCostSP<<"*"<<expansion);
			DEBUG_MESSAGE(
					"expansion"<<expansion<<"log cutoff:"<<log(patNBParameters::the()->CUTOFFPROBABILITY_ELEMENT)<<
					", cost scale"<<linkAndPathCost.getLinkCostScale()<<"sp"<<linkCostSP);
			patNetworkReducer nr(origin_node, destination_node,
					&linkAndPathCost, expansion);
			nr.reduce(network);
			network->exportKML("reduced.kml");
			DEBUG_MESSAGE("network reduced");
		}

		DEBUG_MESSAGE("start propose");
		patRandomNumber rng(patNBParameters::the()->randomSeed);
		MHPathProposal proposal(origin_node, destination_node, &router,
				linkAndPathCost.getLinkCostScale(), &rng);
		MHPath path = proposal.newInitialState();
		DEBUG_MESSAGE("shortest path generated");
		map<string, string> attr;
		path_writer.writePath(path, attr);
		DEBUG_MESSAGE("shortest path written");
		path.setPoints(MHPoints(1, 3, path.size() - 1));
		DEBUG_MESSAGE("points set");
		path.insertDetour(
				network_environment.getNetworkElements().getNode(428511453));
		path_writer.writePath(path, attr);
		path_writer.close();
	}
};
#endif /* TESTMHPATH_H_ */
