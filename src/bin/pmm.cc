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
	char *command = NULL;
	char *gps_file = NULL;
	int c;
	while ((c = getopt(argc, argv, "c:f:g:h")) != -1) {
		switch (c) {
		case 'c':
			command = optarg;
			break;
		case 'f':
			param_file = optarg;
			break;
		case 'g':
			gps_file = optarg;
			break;

		case 'h':
			cout
					<< "bioroute -c COMMAND -f PARAM_FILE -n NUMBER_OF_OBSERVATIONS"
					<< endl;
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

	string command_str(command);
	string gps_file_string(gps_file);
	cout << command_str << endl;

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
	if (ifstream(network_rule.c_str())) {
		patWay::initiateNetworkTypeRules(network_rule);
	}

	patGpsSequence gps_sequence(gps_file_string);
	patGeoBoundingBox bb = gps_sequence.computeBoundingBox(0.01);
	// patGeoBoundingBox(
	// 		patNBParameters::the()->boundingBoxLeftUpLongitude,
	// 		patNBParameters::the()->boundingBoxLeftUpLatitude,
	// 		patNBParameters::the()->boundingBoxRightBottumLongitude,
	// 		patNBParameters::the()->boundingBoxRightBottumLatitude);
	patNetworkEnvironment network_environment(bb);
	//Load networks
	network_environment.getNetwork(CAR)->exportShpFiles(
			gps_file_string + "_network.shp");
//	network_environment.getNetwork(CAR)->exportKML(gps_file_string+"_network.kml");
	cout << gps_file_string << endl;
	patProbabilisticMapMatching pmm(&network_environment,
			gps_file_string.substr(0, gps_file_string.size() - 4), err);

	if (err != NULL) {
		abort();
	}
	pmm.run(err);

//#pragma omp parallel num_threads( num_threads)
	// {
//#pragma omp for

	// for (list<string>::iterator gps_file_iter = gps_files.begin();
	// 		gps_file_iter != gps_files.end();
	// 		++gps_file_iter
	// 	) {
	// 		try {
	// 			DEBUG_MESSAGE("OK");
	// 			DEBUG_MESSAGE("===FINISH "<< *gps_file_iter);

	// 			 if (err != NULL) {
	// 			 continue;
	// 			 } else {
	// 			 DEBUG_MESSAGE("===FINISH "<< *gps_file_iter);
	// 			 //	pmm.exportToKML();
	// 			 }

	// 		}
	// 		catch(...) {
	// 			DEBUG_MESSAGE("===Fail to map matching "<<*gps_file_iter);
	// 		}
	// 	}
	// }
	// return 0;
}
