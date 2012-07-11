/*
 * randomWalk.cc
 *
 *  Created on: Jul 11, 2012
 *      Author: jchen
 */

#include "patNBParameters.h"
#include "patDisplay.h"
#include "initParams.h"
#include "initNetworks.h"
#include "getObservations.h"
#include "sampleChoiceSet.h"
#include "patNetworkEnvironment.h"
#include "patObservation.h"
#include <vector>
#include <map>
using namespace std;
int main(int argc, char *argv[]) {

	DEBUG_MESSAGE("STARTED..")

	initParameters(argv[1]);

	patNetworkEnvironment network_environment = initiateNetworks(-100, 100, 100,
			-100);

	string folder = patNBParameters::the()->observationDirectory;

	map<ARC_ATTRIBUTES_TYPES, double> link_coef;
	link_coef[ENUM_LENGTH] = patNBParameters::the()->mh_length_coef;
	link_coef[ENUM_SPEED_BUMP] = patNBParameters::the()->mh_sb_coef;

	const patNode* origin = network_environment.getNetworkElements().getNode(
			patNBParameters::the()->OriginId);
	const patNode* destination =
			network_environment.getNetworkElements().getNode(
					patNBParameters::the()->DestinationId);
	if (origin == NULL || destination == NULL) {
		WARNING("od not specified");
		exit(-1);
	}

	string choice_set_folder = folder + patNBParameters::the()->choiceSetFolder+"/";
	if (patNBParameters::the()->pathSampleAlgorithm == "RW") {
		sampleRWChoiceSetWithOd(choice_set_folder, 1000,network_environment, origin,
				destination);
	} else if (patNBParameters::the()->pathSampleAlgorithm == "MH") {

		sampleMHChoiceSetWithOd(choice_set_folder, 1000,link_coef, network_environment, origin,
				destination);
	}
}

