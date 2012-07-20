/*
 * bioroute.cc
 *
 *  Created on: Jul 12, 2012
 *      Author: jchen
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "initParams.h"
#include "patExperimentBed.h"

int main(int argc, char *argv[]) {

	char *param_file = NULL;
	char *command = NULL;
	int nbr_observation = INT_MAX;
	bool synthetic_network = false;
	int c;
	while ((c = getopt(argc, argv, "c:f:n:s")) != -1) {
		switch (c) {
		case 'c':
			command = optarg;
			break;
		case 's':
			synthetic_network=true;
			break;
		case 'f':
			param_file = optarg;
			break;
		case 'n':
			try {
				nbr_observation = atoi(optarg);
			} catch (...) {
				cout << "Invalid argument: " << optarg
						<< ". Number of observation should be an integer."
						<< endl;
				exit(-1);
			}
			break;
		case '?':
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
	cout <<command_str<<endl;
	initParameters(param_file);

	patExperimentBed run(!synthetic_network, false, CAR, nbr_observation);

	if (command_str == "SampleWithOd") {
		run.sampleChoiceSetWithOd(nbr_observation);
	} else if (command_str == "Sample") {
		run.sampleChoiceSet();
		run.writeBiogeme();
	} else if (command_str == "MHEnumeratePaths") {
		run.enumerateMHPaths();
	} else if (command_str == "WriteBiogeme") {
		run.writeBiogeme();
	} else if (command_str == "SimulateObservations") {
		run.simulateObservations();
	}
	else if (command_str == "ExportNetwork"){
		run.exportNetwork();
	}
	else if (command_str == "TestNetwork"){
		run.testNetwork();
	}
	else if (command_str == "Verify"){
		run.verifySamplingResult();
	}
	else {
		cout << "Wrong command: " << command_str << endl;
		cout <<"Options are:"<<endl;
		cout << "\t"<<"SampleWithOd"<<endl;
		cout << "\t"<<"MHEnumeratePaths"<<endl;
		cout << "\t"<<"WriteBiogeme"<<endl;
		cout << "\t"<<"SimulateObservations"<<endl;

	}

}

