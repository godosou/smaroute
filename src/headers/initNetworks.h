/*
 * initNetworks.h
 *
 *  Created on: Jul 11, 2012
 *      Author: jchen
 */

#ifndef INITNETWORKS_H_
#define INITNETWORKS_H_

#include "patError.h"
#include "patNBParameters.h"
#include "patDisplay.h"
#include "patNetworkEnvironment.h"
#include "patWay.h"
#include "patGeoBoundingBox.h"
patNetworkEnvironment initiateNetworks(double b1 = 0.0, double b2 = 0.0,
		double b3 = 0.0, double b4 = 0.0) {

	patError* err(NULL);

	string network_file = patNBParameters::the()->OsmNetworkFileName;
	string network_folder = network_file.substr(0, network_file.rfind("/"));
	string network_rule = network_folder + "/network_rules.csv";
	if (ifstream(network_rule.c_str())) {
//		DEBUG_MESSAGE("network rules file: "<<network_rule);
		patWay::initiateNetworkTypeRules(network_rule);
	}
	patGeoBoundingBox bb;
	if (b1 == b2) {
		bb = patGeoBoundingBox(
				patNBParameters::the()->boundingBoxLeftUpLongitude,
				patNBParameters::the()->boundingBoxLeftUpLatitude,
				patNBParameters::the()->boundingBoxRightBottumLongitude,
				patNBParameters::the()->boundingBoxRightBottumLatitude);
	} else {

		 bb = patGeoBoundingBox(b1, b2, b3, b4);
	}

	patNetworkEnvironment network_environment(bb, err);
	if (err != NULL) {
		exit(-1);
	}
	DEBUG_MESSAGE("network loaded");

	return network_environment;
}


#endif /* INITNETWORKS_H_ */
