/*
 * pathGeneration.cc
 *
 *  Created on: Apr 27, 2012
 *      Author: jchen
 */

#include "patException.h"
#include "patError.h"
#include "patNBParameters.h"
#include "MHPathGenerator.h"
#include "patKMLPathWriter.h"
#include "patDisplay.h"
#include "patNetworkCar.h"
int main(int argc, char *argv[]) {

	try {
		DEBUG_MESSAGE("STARTED..")

		/*
		 * (1) Extract single command line parameter.
		 */
		patError* err(NULL);

		//Read parameters
		if (argc == 1) {
			patNBParameters::the()->readFile(
					"../params/config.xml",
					err);

		} else {
			patNBParameters::the()->readFile(argv[1], err);

		}
		patNBParameters::the()->init(err);
		if (err != NULL) {
			exit(-1);
		}
		/*
		patGeoBoundingBox bb = patGeoBoundingBox(
				patNBParameters::the()->boundingBoxLeftUpLongitude,
				patNBParameters::the()->boundingBoxLeftUpLatitude,
				patNBParameters::the()->boundingBoxRightBottumLongitude,
				patNBParameters::the()->boundingBoxRightBottumLatitude);
		*/

		patGeoBoundingBox bb = patGeoBoundingBox(
				6.49409428385,
				46.544856996900002,
				6.5770872475999997,
				46.510016372300001);

		patNetworkEnvironment network_environment(bb, err);
		//Load networks
		if (err != NULL) {
			exit(-1);
		}

		MHLinkAndPathCost::configure();
		DEBUG_MESSAGE("network loaded");
		patNetworkBase *network = new patNetworkCar();
		DEBUG_MESSAGE("new network initiated");
		network_environment.getNetwork(CAR);
		*network = *network_environment.getNetwork(CAR);
		DEBUG_MESSAGE("network copied");
		/*
		 * (4) Instantiate the path writer.
		 */
		patKMLPathWriter path_writer("path.kml");
		/*
		 * (5) Instantiate and initialize the path generator.
		 */
		patRandomNumber rng(patNBParameters::the()->randomSeed);
		MHPathGenerator path_generator(&rng);
		path_generator.setNetwork(network);
		path_generator.setPathWriter(&path_writer);

		const patNode* origin_node  = network_environment.getNetworkElements().getNode(252683850);
		const patNode* destination_node  = network_environment.getNetworkElements().getNode(312048942);
		path_generator.run(origin_node,destination_node);
		path_writer.close();
		DEBUG_MESSAGE("..DONE");

	} catch (exception& e) {
		DEBUG_MESSAGE(e.what());
	}

}
