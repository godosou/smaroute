/*
 * randomWalk.cc
 *
 *  Created on: Jul 11, 2012
 *      Author: jchen
 */

#include "patDisplay.h"
#include "initParams.h"
#include "patExperimentBed.h"
using namespace std;
int main(int argc, char *argv[]) {

	initParameters(argv[1]);
	patExperimentBed run(true, false, CAR);
	run.sampleChoiceSetWithOd(1000);
}

