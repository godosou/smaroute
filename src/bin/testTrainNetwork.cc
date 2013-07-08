/*
 * testTrainNetwork.cc
 *
 *  Created on: Jun 18, 2013
 *      Author: jchen
 */


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
	 */

	char *param_file = NULL;
	int c;
	while ((c = getopt(argc, argv, "f:g:h")) != -1) {
		switch (c) {
		case 'f':
			param_file = optarg;
			break;
		case 'h':
			cout << "testTrainNetwork  -f PARAM_FILE " << endl;
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


	string network_file = patNBParameters::the()->OsmNetworkFileName;
	string network_folder = network_file.substr(0, network_file.rfind("/"));
	string network_rule = network_folder + "/network_rules.csv";
	patWay::initiateNetworkTypeRules();
	patGeoBoundingBox bb(patNBParameters::the()->boundingBoxLeftUpLongitude,
			patNBParameters::the()->boundingBoxLeftUpLatitude,
			patNBParameters::the()->boundingBoxRightBottumLongitude,
			patNBParameters::the()->boundingBoxRightBottumLatitude);
	;

	patNetworkEnvironment network_environment(bb);
	network_environment.getNetwork(TRAIN)->exportShpFiles("train_network.shp");
	network_environment.getNetwork(TRAIN)->exportKML("train_network.kml");



}
