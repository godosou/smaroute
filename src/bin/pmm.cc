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
	patError* err(NULL);

	//Read parameters
	if (argc==1) {
		patNBParameters::the()->readFile("/Volumes/Files/newbioroute/Accel10/bus/bt1acc1mcp0.0.xml", err);
		
	}
	else {
		patNBParameters::the()->readFile(argv[1], err);

	}

	patNBParameters::the()->init(err);
	if (err != NULL) {
		exit(-1);
	}
	patSpeedDistributions::readParams(err);
	if (err != NULL) {
			exit(-1);
		}

	patAccelMeasurementModel::readParams(err);
	if (err != NULL) {
			exit(-1);
		}


	list<string> gps_files = patListDirectory::getListOfFiles(
			patNBParameters::the()->dataDirectory, err);
	if (err != NULL) {
		exit(-1);
	}


	patGeoBoundingBox bb = patGeoBoundingBox(
			patNBParameters::the()->boundingBoxLeftUpLongitude,
			patNBParameters::the()->boundingBoxLeftUpLatitude,
			patNBParameters::the()->boundingBoxRightBottumLongitude,
			patNBParameters::the()->boundingBoxRightBottumLatitude);
	patNetworkEnvironment network_environment(bb, err);
	//Load networks
	if (err != NULL) {
		exit(-1);
	}
	for (list<string>::iterator gps_file_iter = gps_files.begin();
			gps_file_iter != gps_files.end(); ++gps_file_iter) {
		try{
		DEBUG_MESSAGE("OK");
		DEBUG_MESSAGE("===BEGIN " << *gps_file_iter);
		patProbabilisticMapMatching pmm(&network_environment,
				(*gps_file_iter).substr(0, (*gps_file_iter).size() - 4), err);

		if (err != NULL) {
			continue;
		}
		pmm.run(err);
		DEBUG_MESSAGE("===FINISH "<< *gps_file_iter);
		/*
		if (err != NULL) {
			continue;
		} else {
			DEBUG_MESSAGE("===FINISH "<< *gps_file_iter);
			//	pmm.exportToKML();
		}
		*/
		}
		catch(...){
			DEBUG_MESSAGE("===Fail to map matching "<<*gps_file_iter);
		}
	}

	return 0;
}
