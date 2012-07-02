/*
 * synPathGeneration.cc
 *
 *  Created on: May 26, 2012
 *      Author: jchen
 */

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
#include "MHWeightFunction.h"
#include "patReadPathsFromKML.h"
#include "patReadChoiceSetFromKML.h"
#include "patComputePathSize.h"
#include "patChoiceSet.h"
#include <boost/lexical_cast.hpp>
int main(int argc, char *argv[]) {

	DEBUG_MESSAGE("STARTED..")

	/*
	 * (1) Extract single command line parameter.
	 */
	patError* err(NULL);

	//Read parameters
	if (argc == 1) {
		patNBParameters::the()->readFile(
				"../../playground/Synthetic/params/config.xml", err);

	} else {
		patNBParameters::the()->readFile(argv[1], err);

	}
	patNBParameters::the()->init(err);
	if (err != NULL) {
		exit(-1);
	}
	patGeoBoundingBox bb = patGeoBoundingBox(-100, 100, 100, -100);

	patNetworkEnvironment network_environment(bb, err);
	//Load networks
	if (err != NULL) {
		exit(-1);
	}

	string folder = patNBParameters::the()->observationDirectory;

	string obsrvation_file = folder + "observations.kml";
	if (!ifstream(obsrvation_file.c_str())) {
		throw RuntimeException("no observation file found");
	}
	vector<patObservation> observations;
	for (unsigned i = 0; i < 1000; ++i) {
		string obs_file_name = patNBParameters::the()->observationDirectory
				+ "observations/" + boost::lexical_cast < string > (i) + ".kml";

		patObservation new_observation;
		new_observation.setId(boost::lexical_cast < string > (i));
		if (!ifstream(obs_file_name.c_str())) {
			WARNING(obs_file_name);
			throw RuntimeException("no valid observation file");
		}
		patReadPathsFromKML rp;

		vector<patMultiModalPath> obs_paths = rp.read(
				&network_environment.getNetworkElements(), obs_file_name);
		if (obs_paths.size() != 1) {
			WARNING("WRONG PATH NUMBER"<<obs_paths.size());
		}
		new_observation.addPath(obs_paths.front(), 1.0);
		observations.push_back(new_observation);

	}
	if (patNBParameters::the()->pathSampleAlgorithm == "MH") {
		patLinkAndPathCost router_link_cost(
				patNBParameters::the()->mh_link_scale,
				-patNBParameters::the()->mh_length_coef, 0.0,
				patNBParameters::the()->router_cost_sb_coef);

		map<ARC_ATTRIBUTES_TYPES, double> link_coef;
		link_coef[ENUM_LENGTH] = patNBParameters::the()->mh_length_coef;
		link_coef[ENUM_SPEED_BUMP] = patNBParameters::the()->mh_sb_coef;

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
					1000000, rnd);
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

	} else {
		throw RuntimeException("Invalid sampling algorithm");
	}
}
