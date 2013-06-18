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
	char *file_name=NULL;
	int nbr_observation = INT_MAX;
	bool synthetic_network = false;
	bool real_observation = true;
	int c;
	while ((c = getopt(argc, argv, "c:f:n:o:srh")) != -1) {
		switch (c) {
		case 'c':
			command = optarg;
			break;
		case 'o':
			file_name=optarg;
			break;
		case 's':
			synthetic_network = true;
			break;
		case 'f':
			param_file = optarg;
			break;
		case 'r':
			real_observation = false;
			break;
		case 'h':
			cout
					<< "bioroute -c COMMAND -f PARAM_FILE -n NUMBER_OF_OBSERVATIONS"
					<< endl;
			cout << "COMMAND: " << endl;
			cout
					<< "\t SampleWithOd: \t\t\t Sample choice set with the od given in the config file."
					<< endl;
			cout
					<< "\t Sample: \t\t\t Sample choice set with observations in observations folder."
					<< endl;
//			cout
//					<< "\t SampleEqualProbability: \t\t\t Sample choice set with observations in observations folder with equal probability."
//					<< endl;
			cout
					<< "\t MHEnumeratePaths: \t\t Enumerate paths using MH algorithm."
					<< endl;
			cout
					<< "\t WriteBiogeme: \t\t\t Write biogeme file from observations and choice sets."
					<< endl;
			cout << "\t SimulateObservations: \t\t Simulate observations."
					<< endl;
			cout
					<< "\t SimulateObservationsError: \t Simulate errors in observations."
					<< endl;
			cout << "\t Verify: \t\t\t Verify sampling results." << endl;
			cout
					<< "\t ExportNetwork: \t\t Export networks in shp and kml formats."
					<< endl;
			cout << "\t TestNetwork: \t\t\t Test network." << endl;
			cout << endl;

			cout << "\t WriteNetworkToDB: Write network to edge table in the db"
					<< endl;
			cout << "Additional arguments:" << endl;
			cout << "\t -s\t The network is synthetic." << endl;
			cout << "\t -r\t The observations are real." << endl;
			cout << endl;
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
	cout << command_str << endl;

	if (synthetic_network==true){
		cout << "The network is synthetic."<<endl;
	}
	if (real_observation==false){
		cout << "The observations are synthetic."<<endl;
	}


	initParameters(param_file);

	try {
		if (command_str == "SampleWithOd") {
			patExperimentBed run(!synthetic_network, real_observation, CAR,
					nbr_observation);
			run.sampleChoiceSetWithOd(nbr_observation);
		} else if (command_str == "Sample") {
			patExperimentBed run(!synthetic_network, real_observation, CAR,
					nbr_observation);
			run.sampleChoiceSet();
			run.writeBiogeme();
			run.analyzeChoiceSet();
		} else if (command_str == "MHEnumeratePaths") {
			patExperimentBed run(!synthetic_network, real_observation, CAR,
					nbr_observation);
			run.enumerateMHPaths();
		} else if (command_str == "WriteBiogeme") {
			patExperimentBed run(!synthetic_network, real_observation, CAR,
					nbr_observation);
			run.writeBiogeme();
		} else if (command_str == "SimulateObservations") {
			patExperimentBed run(!synthetic_network, real_observation, CAR,
					nbr_observation);
			run.simulateObservations();
		} else if (command_str == "ExportNetwork") {
			patExperimentBed run(!synthetic_network, real_observation, CAR,
					nbr_observation);
			run.exportNetwork();
		} else if (command_str == "TestNetwork") {
			patExperimentBed run(!synthetic_network, real_observation, CAR,
					nbr_observation);
			run.testNetwork();
		} else if (command_str == "Verify") {
			patExperimentBed run(!synthetic_network, real_observation, CAR,
					nbr_observation);
			run.verifySamplingResult();
		} else if (command_str == "SimulateObservationsError") {
			patExperimentBed run(!synthetic_network, real_observation, CAR,
					nbr_observation);
			run.simulateObservationError();
		} else if (command_str == "WriteNetworkToDB") {
			patExperimentBed run(!synthetic_network, real_observation, CAR,
					nbr_observation);
			run.writeNetworkToDB();
		} else if (command_str == "AnalyzeChoiceSet") {
			patExperimentBed run(!synthetic_network, real_observation, CAR,
					nbr_observation);
			run.analyzeChoiceSet();
		}  else if (command_str == "WriteChoiceSetSHP") {
			patExperimentBed run(!synthetic_network, real_observation, CAR,
					nbr_observation);
			run.writeChoiceSetSHP();
		} else if (command_str == "KML2SHP") {
			patExperimentBed run(!synthetic_network, real_observation, CAR,
					nbr_observation);
			run.kml2SHP(string(file_name));
		}else if (command_str == "OBS2SHP") {
			patExperimentBed run(!synthetic_network, real_observation, CAR,
					nbr_observation);
			run.obs2SHP();
		}else if (command_str == "AnalyzeOBS") {
			patExperimentBed run(!synthetic_network, real_observation, CAR,
					nbr_observation);
			run.analyzeOBS();
		}

		else {
			cout << "Wrong command: " << command_str << endl;
			cout << "Options are:" << endl;
			cout << "\t" << "SampleWithOd" << endl;
			cout << "\t" << "MHEnumeratePaths" << endl;
			cout << "\t" << "WriteBiogeme" << endl;
			cout << "\t" << "SimulateObservations" << endl;

		}
	} catch (const exception& e) {
		cout << e.what() << endl;
	}
}

