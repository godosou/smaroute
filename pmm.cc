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
	patNBParameters::the()->readFile(argv[1], err);
	patNBParameters::the()->init(err);
	if (err != NULL) {
		exit(-1);
	}
	patGeoBoundingBox bb = patGeoBoundingBox(6.56, 46.53, 6.58, 46.51);

	//Load networks
	patNetworkEnvironment network_environment(bb,err);
	if (err != NULL) {
		exit(-1);
	}

	return 0;
}
