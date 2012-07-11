/*
 * sampleChoiceSet.h
 *
 *  Created on: Jul 11, 2012
 *      Author: jchen
 */

#ifndef SAMPLECHOICESET_H_
#define SAMPLECHOICESET_H_

#include "patException.h"
#include "patNBParameters.h"
#include "patDisplay.h"
#include "patLinkAndPathCost.h"
#include "MHWeightFunction.h"
#include "MHPathGenerator.h"
#include "RWPathGenerator.h"
#include "patSampleChoiceSetWithObservations.h"
#include "patSampleChoiceSetWithRandomWalk.h"
#include "MHWeightFunction.h"
#include "patComputePathSize.h"
#include "patChoiceSet.h"
#include <boost/lexical_cast.hpp>
#include "patReadChoiceSetFromKML.h"
#include "patKMLPathWriter.h"
void sampleRWChoiceSetWithOd(string& folder,
		const unsigned count,
		const patNetworkEnvironment& network_environment, const patNode* origin,
		const patNode* destination) {

	patLinkAndPathCost router_link_cost(1.0, 1.0, 0.0, 0.0);
	RWPathGenerator path_generator(patNBParameters::the()->randomSeed,
			patNBParameters::the()->kumaA, patNBParameters::the()->kumaB,
			&router_link_cost);
	path_generator.setNetwork(network_environment.getNetwork(CAR));
#pragma omp parallel num_threads(patNBParameters::the()->nbrOfThreads)
	{
#pragma omp for
		for (unsigned i = 0; i < count; ++i) {

			patNetworkBase* cloned_network =
					path_generator.getNetwork()->clone();

			RWPathGenerator* generator_clone = path_generator.clone();
			generator_clone->setNetwork(cloned_network);
			cout << "Start an od" << endl;
			string file_name = folder + boost::lexical_cast<string>(i + 1)+"_sample.kml";
			patKMLPathWriter path_writer(file_name);
			generator_clone->setPathWriter(&path_writer);
			generator_clone->run(origin, destination);
			cout << "An od is dealt with" << endl;
			path_writer.close();
			delete generator_clone;
			generator_clone = NULL;
			delete cloned_network;
			cloned_network = NULL;
		}
	}
}

void sampleMHChoiceSetWithOd(string& folder,
		const unsigned count,
		map<ARC_ATTRIBUTES_TYPES, double>& link_coef,
		const patNetworkEnvironment& network_environment, const patNode* origin,
		const patNode* destination) {

	patLinkAndPathCost router_link_cost(patNBParameters::the()->mh_link_scale,
			-patNBParameters::the()->mh_length_coef, 0.0,
			patNBParameters::the()->router_cost_sb_coef);

	map<const patMultiModalPath, double> ps;
	MHWeightFunction mh_weight(link_coef, patNBParameters::the()->mh_link_scale,
			patNBParameters::the()->mh_ps_coef,
			patNBParameters::the()->mh_obs_scale);
	if (patNBParameters::the()->mh_ps_coef > 0.0) {
		DEBUG_MESSAGE("with path size specification");
		string choiceset_file = folder + "universal_choice_set.kml";
		if (!ifstream(choiceset_file.c_str())) {
			exit(-1);
		}
		patReadChoiceSetFromKML rc(&network_environment.getNetworkElements());

		patRandomNumber rnd(patNBParameters::the()->randomSeed);
		map<patOd, patChoiceSet> od_choice_set = rc.read(choiceset_file, rnd);
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

	path_generator.setMHWeight(&mh_weight);

#pragma omp parallel num_threads(patNBParameters::the()->nbrOfThreads)
	{
#pragma omp for
		for (unsigned i = 0; i < count; ++i) {

			cout << "Start an od" << endl;
			patNetworkBase* cloned_network =
					path_generator.getNetwork()->clone();

			MHPathGenerator* generator_clone = path_generator.clone();

			string file_name = folder + boost::lexical_cast<string>(i + 1)+"_sample.kml";
			patKMLPathWriter path_writer(file_name);
			generator_clone->setPathWriter(&path_writer);
			generator_clone->run(origin, destination);
			cout << "An od is dealt with" << endl;
			path_writer.close();

			delete generator_clone;
			generator_clone = NULL;
			delete cloned_network;
			cloned_network = NULL;
		}
	}
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

#endif /* SAMPLECHOICESET_H_ */
