/*
 * pathGeneration.cc
 *
 *  Created on: May 3, 2012
 *      Author: jchen
 */

#include "patException.h"
#include "patError.h"
#include "patNBParameters.h"
#include "patEnumeratePaths.h"
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
					"../../playground/Synthetic/params/config.xml", err);

		} else {
			patNBParameters::the()->readFile(argv[1], err);

		}
		patNBParameters::the()->init(err);
		if (err != NULL) {
			exit(-1);
		}
		patGeoBoundingBox bb = patGeoBoundingBox(-100, 100, 100, -100);

		patNetworkEnvironment network_environment(bb, err);
		//Load networks
		if (err != NULL) {
			exit(-1);
		}

		/*
		 * (4) Instantiate the path writer.
		 */
		patKMLPathWriter path_writer(
				"../../playground/Synthetic/universal_choice_set.kml");
		/*
		 * (5) Instantiate and initialize the path generator.
		 */
		//patRandomNumber rng(patNBParameters::the()->randomSeed);
		patEnumeratePaths path_generator;
		patNetworkBase* cloned_network =
				network_environment.getNetwork(CAR)->clone();
		path_generator.setNetwork(cloned_network);
		path_generator.setPathWriter(&path_writer);

		const patNode* origin_node =
				network_environment.getNetworkElements().getNode(1);
		const patNode* destination_node =
				network_environment.getNetworkElements().getNode(38);
		if (origin_node != NULL && destination_node != NULL) {
			DEBUG_MESSAGE(origin_node->getUserId()<<"-"<<destination_node->getUserId());
			path_generator.run(origin_node, destination_node);
		}
		else{
			WARNING("NULL node");
		}
		path_writer.close();
		delete cloned_network;
		cloned_network = NULL;
		DEBUG_MESSAGE("..DONE");

	} catch (exception& e) {
		DEBUG_MESSAGE(e.what());
	}

}
