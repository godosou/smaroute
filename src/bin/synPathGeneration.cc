/*
 * synPathGeneration.cc
 *
 *  Created on: May 26, 2012
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

	string obs_folder = folder + "observations/";
	vector<string> obs_files = getObservationFiles(obs_folder);
	vector<patObservation> observations = readSyntheticObservations(obs_folder,
			obs_files, network_environment.getNetworkElements());
	map<ARC_ATTRIBUTES_TYPES, double> link_coef;
	link_coef[ENUM_LENGTH] = patNBParameters::the()->mh_length_coef;
	link_coef[ENUM_SPEED_BUMP] = patNBParameters::the()->mh_sb_coef;

	sampleChoiceSet(folder, link_coef, network_environment, observations);
}
