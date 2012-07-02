/*
 * writeBiogeme.cc
 *
 *  Created on: May 4, 2012
 *      Author: jchen
 */

#include "patException.h"
#include "patError.h"
#include "patNBParameters.h"
#include "patDisplay.h"
#include "patReadObservationFromKML.h"
#include "patObservation.h"
#include <dirent.h>
#include "patWay.h"
#include "patObservation.h"
#include "patReadChoiceSetFromKML.h"
#include <fstream>
#include <iomanip>
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
					"/Users/jchen/Documents/Project/newbioroute/src/params/config.xml",
					err);

		} else {
			patNBParameters::the()->readFile(argv[1], err);

		}
		patNBParameters::the()->init(err);
		if (err != NULL) {
			exit(-1);
		}
		patWay::initiateNetworkTypeRules();

		patGeoBoundingBox bb = patGeoBoundingBox(
				patNBParameters::the()->boundingBoxLeftUpLongitude,
				patNBParameters::the()->boundingBoxLeftUpLatitude,
				patNBParameters::the()->boundingBoxRightBottumLongitude,
				patNBParameters::the()->boundingBoxRightBottumLatitude);

		//		patGeoBoundingBox bb = patGeoBoundingBox(6.49909428385,
		//				46.550856996900002, 6.5700872475999997, 46.505016372300001);

		patNetworkEnvironment network_environment(bb, err);
		//Load networks
		if (err != NULL) {
			exit(-1);
		}

		DEBUG_MESSAGE("network loaded");

		patObservation new_observation;
		patReadObservationFromKML ro(&network_environment.getNetworkElements());
		ro.parseFile(
				patNBParameters::the()->observationDirectory + "observations"
						+ argv[2], new_observation);
		patReadChoiceSetFromKML rc(&network_environment.getNetworkElements());
		rc.read(
				patNBParameters::the()->observationDirectory
						+ patNBParameters::the()->choiceSetFolder + "/"
						+ new_observation.getId() + "_sample.kml",
				new_observation);

	} catch (exception& e) {
		DEBUG_MESSAGE(e.what());
	}

}
