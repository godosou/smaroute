/*
 * funcs.h
 *
 *  Created on: Jul 10, 2012
 *      Author: jchen
 */

#ifndef FUNCS_H_
#define FUNCS_H_

#include "patException.h"
#include "patError.h"
#include "patNBParameters.h"
#include "patEnumeratePaths.h"
#include "patKMLPathWriter.h"
#include "patDisplay.h"
#include "patNetworkCar.h"
#include "patLinkAndPathCost.h"
#include "MHWeightFunction.h"
#include "MHPathGenerator.h"
#include "RWPathGenerator.h"
#include "patSimulation.h"
#include "patSampleChoiceSetWithObservations.h"
#include "patSampleChoiceSetWithRandomWalk.h"
#include "patReadObservationFromKML.h"
#include "MHWeightFunction.h"
#include "patReadPathsFromKML.h"
#include "patReadChoiceSetFromKML.h"
#include "patComputePathSize.h"
#include "patChoiceSet.h"
#include <boost/lexical_cast.hpp>
#include <dirent.h>
void initParameters(string param_file) {

	patError* err(NULL);

	//Read parameters

	patNBParameters::the()->readFile(param_file, err);
	patNBParameters::the()->init(err);
	if (err != NULL) {
		exit(-1);
	}
}
patNetworkEnvironment initiateNetworks(double b1 = 0.0, double b2 = 0.0,
		double b3 = 0.0, double b4 = 0.0) {

	patError* err(NULL);

	string network_file = patNBParameters::the()->OsmNetworkFileName;
	string network_folder = network_file.substr(0, network_file.rfind("/"));
	string network_rule = network_folder + "/network_rules.csv";
	if (ifstream(network_rule.c_str())) {
		DEBUG_MESSAGE("network rules file: "<<network_rule);
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

vector<string> getObservationFiles(string folder) {

	DIR * dip;
	struct dirent *dit;
	const char* dir_name_char = folder.c_str();

	DEBUG_MESSAGE("Try to open directory " << dir_name_char);
	if ((dip = opendir(dir_name_char)) == NULL) {
		stringstream str;
		str << "Directory " << dir_name_char
				<< " doesn't exist or no permission to read.";
		WARNING(str.str());
		exit(-1);
	}
//	DEBUG_MESSAGE("Direcotry " << dir_name_char << " is now open");
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
//				DEBUG_MESSAGE("Found file:" << fileName);
				observation_files.push_back(fileName);
			}

		}
	}

	closedir(dip);

	if (observation_files.empty()) {
		exit(0);
	}

	DEBUG_MESSAGE(observation_files.size()<<" kml files are read.");
	return observation_files;
}

vector<patObservation> readSyntheticObservations(string& folder,
		vector<string>& observation_files,
		const patNetworkElements& network_elements) {

	vector<patObservation> observations;
	for (unsigned i = 0; i < observation_files.size(); ++i) {
		string file_name = folder + observation_files[i];
		patObservation new_observation;
		new_observation.setId(boost::lexical_cast<string>(i));
		if (!ifstream(file_name.c_str())) {
			WARNING(file_name);
			throw RuntimeException("no valid observation file");
		}
		patReadPathsFromKML rp;

		vector<patMultiModalPath> obs_paths = rp.read(&network_elements,
				file_name);
		if (obs_paths.size() != 1) {
			WARNING("WRONG PATH NUMBER"<<obs_paths.size());
		}
		new_observation.addPath(obs_paths.front(), 1.0);
		observations.push_back(new_observation);

	}
	DEBUG_MESSAGE(observations.size()<<" observations are read.");
	return observations;
}
vector<patObservation> readRealObservations(string& folder,
		vector<string>& observation_files,
		const patNetworkElements& network_elements) {

	vector<patObservation> observations;
	for (unsigned i = 0; i < observation_files.size(); ++i) {
		string file_name = folder + observation_files[i];
		patObservation new_observation;
		patReadObservationFromKML ro(&network_elements);
		ro.parseFile(file_name, &new_observation);
		observations.push_back(new_observation);
	}
	DEBUG_MESSAGE(observations.size()<<" observations are read.");
	return observations;
}
void sampleChoiceSet(string& folder,
		map<ARC_ATTRIBUTES_TYPES, double>& link_coef,
		const patNetworkEnvironment& network_environment,
		vector<patObservation> &observations) {

	if (patNBParameters::the()->pathSampleAlgorithm == "MH") {
		patLinkAndPathCost router_link_cost(
				patNBParameters::the()->mh_link_scale,
				-patNBParameters::the()->mh_length_coef, 0.0,
				patNBParameters::the()->router_cost_sb_coef);

		map<const patMultiModalPath, double> ps;
		MHWeightFunction mh_weight(link_coef,
				patNBParameters::the()->mh_link_scale,
				patNBParameters::the()->mh_ps_coef,
				patNBParameters::the()->mh_obs_scale);
		if (patNBParameters::the()->mh_ps_coef > 0.0) {
			DEBUG_MESSAGE("with path size specification");
			string choiceset_file = folder + "universal_choice_set.kml";
			if (!ifstream(choiceset_file.c_str())) {
				exit(-1);
			}
			patReadChoiceSetFromKML rc(
					&network_environment.getNetworkElements());

			patRandomNumber rnd(patNBParameters::the()->randomSeed);
			map<patOd, patChoiceSet> od_choice_set = rc.read(choiceset_file,
					rnd);
			//	DEBUG_MESSAGE(od_choice_set.size());
			patOd od = od_choice_set.begin()->first;
			patChoiceSet choice_set = od_choice_set.begin()->second;
			patComputePathSize ps_computer;
			ps = ps_computer.computePS(choice_set);
			mh_weight.setPathSize(&ps);
		}
		MHPathGenerator path_generator(patNBParameters::the()->randomSeed);
		path_generator.setRouterLinkCost(&router_link_cost);
		path_generator.setNetwork(network_environment.getNetwork(CAR));

		DEBUG_MESSAGE("START TO DEAL WITH OBSERVATIONS"<<observations.size());
		patSampleChoiceSetWithObservations sco;
		for (unsigned i = 0; i < observations.size(); ++i) {
			sco.addObservation(observations[i]);
		}
		DEBUG_MESSAGE("START TO SAMPLE CHOICE SET");
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
		for (unsigned i = 0; i < observations.size(); ++i) {
			scr.addObservation(observations[i]);
		}
		DEBUG_MESSAGE("START TO SAMPLE CHOICE SET");
		scr.sampleChoiceSet(&path_generator, folder);

	} else {
		throw RuntimeException("Invalid sampling algorithm");
	}
}
#endif /* FUNCS_H_ */

