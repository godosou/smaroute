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
#include "patNetworkEnvironment.h"
#include "patNetworkBase.h"
#include "patMultiModalPath.h"
#include "patNetworkReducer.h"
#include "patTransportMode.h"
#include "patReadChoiceSetFromKML.h"
class MyTestReadChoiceSet: public CxxTest::TestSuite {
public:
	void testReadChoiceSet(void) {

		/*
		 * (1) Extract single command line parameter.
		 */
		patError* err(NULL);

		//Read parameters
		patNBParameters::the()->readFile(
				"/Users/jchen/Documents/Project/newbioroute/src/params/config.xml",
				err);
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

		patReadChoiceSetFromKML rc(&network_environment.getNetworkElements());
		patObservation new_observation;
		rc.read(string("../../src/TestData/3 -6648.kml_sample.kml"), new_observation);

	}
};
#endif /* TESTMHPATH_H_ */
