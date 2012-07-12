/*
 * writeBiogeme.cc
 *
 *  Created on: Jul 12, 2012
 *      Author: jchen
 */

#include "patDisplay.h"
#include "initParams.h"
#include "patExperimentBed.h"

int main(int argc, char *argv[]) {

	initParameters(argv[1]);
	int nbr_observations = INT_MAX;
	if (argc == 3) {
		nbr_observations = atoi(argv[2]);
	}
	patExperimentBed run(true, false, CAR, nbr_observations);
	run.writeBiogeme();
}

