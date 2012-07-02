/*
 * pathGeneration.cc
 *
 *  Created on: May 3, 2012
 *      Author: jchen
 */

#include "patException.h"
#include "patError.h"
#include "patNBParameters.h"
#include "MHPathGenerator.h"
#include "RWPathGenerator.h"
#include "patKMLPathWriter.h"
#include "patDisplay.h"
#include "patNetworkCar.h"
#include "patReadObservationFromKML.h"
#include "patObservation.h"
#include "patSampleChoiceSetWithObservations.h"
#include "patSampleChoiceSetWithRandomWalk.h"
#include <dirent.h>
#include <boost/date_time.hpp>
#include "patWay.h"
#include <unistd.h>
void workerFunc(string file_name, const patPathGenerator* path_generator,
		const patNetworkElements* network_elements) {
}
void testThread() {

}
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
		string folder = patNBParameters::the()->observationDirectory;
		string dir_name = patNBParameters::the()->observationDirectory
				+ "observations/";

		DIR * dip;
		struct dirent *dit;
		const char* dir_name_char = dir_name.c_str();

		DEBUG_MESSAGE("Try to open directory " << dir_name_char);
		if ((dip = opendir(dir_name_char)) == NULL) {
			stringstream str;
			str << "Directory " << dir_name_char
					<< " doesn't exist or no permission to read.";
			WARNING(str.str());
			exit(-1);
		}DEBUG_MESSAGE("Direcotry " << dir_name_char << " is now open");
		vector<string> observation_files;
		unsigned char isFile = 0x8;
		string esp("~");
		string kml("kml");

		while ((dit = readdir(dip)) != NULL) {
			if (dit->d_type == isFile) {
				//DEBUG_MESSAGE(dit->d_name);
				string fileName(dit->d_name);
				if (fileName.find(esp) == string::npos
						&& fileName.find(kml) != string::npos) {
					DEBUG_MESSAGE("Found file:" << fileName);
					observation_files.push_back(fileName);
				}

			}
		}

		closedir(dip);

		DEBUG_MESSAGE("nbr of observations"<<observation_files.size());
		if (observation_files.empty()) {
			exit(0);
		}

		string network_file = patNBParameters::the()->OsmNetworkFileName;
		string network_folder = network_file.substr(0,network_file.rfind("/"));
		string network_rule = network_folder+"/network_rules.csv";
		DEBUG_MESSAGE("network rules file: "<<network_rule);
		patWay::initiateNetworkTypeRules(network_rule);

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

		vector<patObservation> observations;
		for (int i = 0; i < observation_files.size(); ++i) {
			string file_name = dir_name + observation_files[i];
			DEBUG_MESSAGE("===BEGIN " << file_name<<" ===");
			patObservation new_observation;
			patReadObservationFromKML ro(
					&network_environment.getNetworkElements());
			ro.parseFile(file_name, &new_observation);
			observations.push_back(new_observation);
			DEBUG_MESSAGE("===End " << file_name<<" ===");
		}

		if (patNBParameters::the()->pathSampleAlgorithm == "MH") {

			patLinkAndPathCost router_link_cost(
					patNBParameters::the()->mh_link_scale,
					-patNBParameters::the()->mh_length_coef, 0.0,
					patNBParameters::the()->router_cost_sb_coef);

			map<ARC_ATTRIBUTES_TYPES, double> link_coef;
			link_coef[ENUM_LENGTH] = patNBParameters::the()->mh_length_coef;
//			link_coef[ENUM_TRAFFIC_SIGNAL] = 0.0;

			map<const patMultiModalPath, double> ps;
			MHWeightFunction mh_weight(link_coef,
					patNBParameters::the()->mh_link_scale,
					patNBParameters::the()->mh_ps_coef,
					patNBParameters::the()->mh_obs_scale);
			MHPathGenerator path_generator(patNBParameters::the()->randomSeed);
			path_generator.setRouterLinkCost(&router_link_cost);
			path_generator.setNetwork(network_environment.getNetwork(CAR));

			DEBUG_MESSAGE("START TO DEAL WITH OBSERVATIONS"<<observations.size());
			patSampleChoiceSetWithObservations sco;
			for (int i = 0; i < observations.size(); ++i) {
				sco.addObservation(observations[i]);
	//		break;
			}DEBUG_MESSAGE("START TO SAMPLE CHOICE SET");
			sco.averagePathProbas();

			const int sample_with_obs = patNBParameters::the()->samplingWithObs;
			if (sample_with_obs == 1 || sample_with_obs == 2) {
				mh_weight.setPathProbas(&(sco.getPathProbas()));
			}

			else if (sample_with_obs == 0 || sample_with_obs == 2) {
				mh_weight.setPathProbas(NULL);

			} else {
				throw RuntimeException("sample with obs? not known parameter!");
			}

			path_generator.setMHWeight(&mh_weight);
			sco.sampleChoiceSet(&path_generator, folder);
		} else if (patNBParameters::the()->pathSampleAlgorithm == "RW") {

			patLinkAndPathCost router_link_cost(1.0, 1.0, 0.0, 0.0);
			RWPathGenerator path_generator(patNBParameters::the()->randomSeed,
					patNBParameters::the()->kumaA, patNBParameters::the()->kumaB,
					&router_link_cost);
			path_generator.setNetwork(network_environment.getNetwork(CAR));
			DEBUG_MESSAGE("START TO DEAL WITH OBSERVATIONS"<<observations.size());
			patSampleChoiceSetWithRandomWalk scr;
			for (int i = 0; i < observations.size(); ++i) {
				scr.addObservation(observations[i]);
	//		break;
			}DEBUG_MESSAGE("START TO SAMPLE CHOICE SET");
			scr.sampleChoiceSet(&path_generator, folder);

		}
	} catch (exception& e) {
		DEBUG_MESSAGE(e.what());
	}

}
