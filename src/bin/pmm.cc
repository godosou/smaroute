/*
 * pmm.cc
 *
 *  Created on: Nov 11, 2011
 *      Author: jchen
 */
#include "patError.h"
#include "patNBParameters.h"
#include "patNetworkEnvironment.h"
#include "patGeoBoundingBox.h"
#include "patProbabilisticMapMatching.h"
#include "patListDirectory.h"
#include "patTransportMode.h"
#include "patTrafficModelComplex.h"
#include "patAccelMeasurementModel.h"
#include "patDisplay.h"
#include "patSpeedDistributions.h"
#include "initParams.h"
#include "patGpsSequence.h"
int main(int argc, char *argv[]) {
	/**
	 * Run map matching for given files.
	 * Algorithm:
	 * -# Read parameters from configuration file.
	 * -# Load networks from the database.
	 * -# Loop over each GPS file:
	 * 		-# Read the file;
	 * 		-# Clean the GPS sequence;
	 * 		-# Do map matching;
	 * -# Do path sampling for each OD.
	 */

	char *param_file = NULL;
	char *gps_file = NULL;
	int c;
	while ((c = getopt(argc, argv, "f:g:h")) != -1) {
		switch (c) {
		case 'f':
			param_file = optarg;
			break;
		case 'g':
			gps_file = optarg;
			break;

		case 'h':
			cout << "pmm  -f PARAM_FILE  -g gps_file" << endl;
			break;

		default:
			abort();
		}
	}

	if (param_file == NULL) {
		cout << "No param file is specified." << endl;
	} else {
		cout << "use param file: " << param_file << endl;
	}

	initParameters(param_file);
	patError* err(NULL);

	patSpeedDistributions::readParams(err);
	if (err != NULL) {
		exit(-1);
	}

	patAccelMeasurementModel::readParams(err);
	if (err != NULL) {
		exit(-1);
	}

	string network_file = patNBParameters::the()->OsmNetworkFileName;
	string network_folder = network_file.substr(0, network_file.rfind("/"));
	string network_rule = network_folder + "/network_rules.csv";
	if (ifstream(network_rule.c_str())) {
		patWay::initiateNetworkTypeRules(network_rule);
	}


	string gps_file_string(gps_file);
	cout << "read gps"<<gps_file_string << endl;
	patGpsSequence gps_sequence(gps_file_string);
	patGeoBoundingBox bb = gps_sequence.computeBoundingBox(0.01);
	cout<<bb<<endl;
	patNetworkEnvironment network_environment(bb);
	//Load networks
//	network_environment.getNetwork(CAR)->exportShpFiles(
//			gps_file_string + "_network.shp");
//	network_environment.getNetwork(CAR)->exportKML(gps_file_string+"_network.kml");
	patProbabilisticMapMatching pmm(&network_environment,
			gps_file_string.substr(0, gps_file_string.size() - 4), err);

	if (err != NULL) {
		abort();
	}
	pmm.run(err);

}
