/*
 * patSimulation.cc
 *
 *  Created on: May 25, 2012
 *      Author: jchen
 */

#include "patSimulation.h"
#include "patKMLPathWriter.h"
#include "patObservation.h"
#include "patUtilityFunction.h"
#include <vector>
#include <map>
#include "patMultiModalPath.h"
#include "patOd.h"
#include <fstream>
#include "patObservation.h"
#include "patReadChoiceSetFromKML.h"
#include "patNetworkEnvironment.h"
#include "patNetworkElements.h"
#include "patComputePathSize.h"
#include <boost/lexical_cast.hpp>
#include "patSampleDiscreteDistribution.h"
#include "patSimulateProbabilisticPaths.h"
#include "patRouter.h"
#include <boost/lexical_cast.hpp>
patSimulation::patSimulation(const patNetworkEnvironment* network_environment,
		patUtilityFunction* utility_function, int rng) :
		m_network_environment(network_environment), m_utility_function(
				utility_function), m_rnd(rng) {
}
void patSimulation::run(string folder, const patRouter* router) {
	patObservation observation;

	patReadChoiceSetFromKML rc(&m_network_environment->getNetworkElements());

	string choiceset_file = folder + "universal_choice_set.kml";
	if (!ifstream(choiceset_file.c_str())) {
		return;
	}

	map<patOd, patChoiceSet> od_choice_set = rc.read(choiceset_file, m_rnd);
//	DEBUG_MESSAGE(od_choice_set.size());
	patOd od = od_choice_set.begin()->first;
	patChoiceSet choice_set = od_choice_set.begin()->second;
	set<patMultiModalPath> paths_set = choice_set.getChoiceSet();
//	DEBUG_MESSAGE(paths_set.size());
	vector<patMultiModalPath> paths_vector;
	vector<double> utility_vector;

	//Normalize utility vector

	vector<int> sampled_observations;
	vector<double> sampled_empirical_proba;
	vector<double> sampled_count;

	DEBUG_MESSAGE("universal path size"<<paths_set.size());
	patComputePathSize ps_computer;

	map<const patMultiModalPath, double> ps = ps_computer.computePS(choice_set);
	DEBUG_MESSAGE("ps calculated"<<ps.size());
	for (set<patMultiModalPath>::iterator path_iter = paths_set.begin();
			path_iter != paths_set.end(); ++path_iter) {
		paths_vector.push_back(*path_iter);

		map<const patMultiModalPath, double>::const_iterator find_ps = ps.find(
				*path_iter);
		if (find_ps == ps.end()) {
			throw RuntimeException("no ps found");
		}

		DEBUG_MESSAGE(
				m_utility_function->getCost( *path_iter)<<"+"<< m_utility_function->getLinkCostScale()<<"*"<<m_utility_function->getPathSizeCoefficient()<<"*"<<log(find_ps->second));

		double utility = m_utility_function->getCostWithPathSize(*path_iter,
				find_ps->second);
		utility_vector.push_back(exp(utility));
		DEBUG_MESSAGE(utility<<","<<utility_vector.back());
		sampled_empirical_proba.push_back(0.0);
		sampled_count.push_back(0);
	}
	double sum = 0.0;
	for (vector<double>::const_iterator u_iter = utility_vector.begin();
			u_iter != utility_vector.end(); ++u_iter) {
		sum += *u_iter;
	}
	for (vector<double>::size_type i = 0; i < utility_vector.size(); ++i) {
		utility_vector[i] /= sum;
	}
//	return;
	DEBUG_MESSAGE(paths_vector.size()<<" alternatives");

	unsigned int total_sampled_count = 1000;
	ofstream sampled_file((folder + "sampled.txt").c_str());
	for (unsigned int i = 0; i < total_sampled_count; ++i) {
//		if (i % 10 == 0) {
//			DEBUG_MESSAGE(i);
//		}
		string i_str = boost::lexical_cast<string>(i);
		patKMLPathWriter kml_writer(folder + "observations/" + i_str + ".kml");
		int sampled = patSampleDiscreteDistribution()(utility_vector, m_rnd);
		sampled_file << sampled << endl;
		sampled_observations.push_back(sampled);
		sampled_empirical_proba[sampled] += 1.0 / (double) total_sampled_count;
		sampled_count[sampled]++;

		map<string, string> attrs_true;
		attrs_true["true"] = boost::lexical_cast<string>(
				utility_vector[sampled]);
		attrs_true["id"] = boost::lexical_cast<string>(sampled);
		attrs_true["proba"] = boost::lexical_cast<string>(
				1.0 - patNBParameters::the()->errorInSimulatedObservations);
		kml_writer.writePath(paths_vector[sampled], attrs_true);
		if (patNBParameters::the()->errorInSimulatedObservations > 0.0
				&& patNBParameters::the()->nbrOfSimulatedErrorPaths > 0) {
			patSimulateProbabilisticPaths simulate_prob_paths(
					paths_vector[sampled], router);
			map<patMultiModalPath, double> simulated_paths =
					simulate_prob_paths.run(
							patNBParameters::the()->nbrOfSimulatedErrorPaths,
							patNBParameters::the()->errorInSimulatedObservations);
			for (map<patMultiModalPath, double>::iterator path_iter =
					simulated_paths.begin(); path_iter != simulated_paths.end();
					++path_iter) {
				map<string, string> attrs;
				attrs["true"] = boost::lexical_cast<string>(
						utility_vector[sampled]);
				attrs["id"] = boost::lexical_cast<string>(sampled);
				attrs["proba"] = boost::lexical_cast<string>(path_iter->second);
				kml_writer.writePath(path_iter->first, attrs);

			}
		}
		kml_writer.close();
	}
	sampled_file.close();
	double chi2 = 0.0;
	for (unsigned int i = 0; i < utility_vector.size(); ++i) {
		double empirical_proba = (double) sampled_count[i]
				/ total_sampled_count;
		double theo_proba = utility_vector[i];
		chi2 += total_sampled_count * (theo_proba - empirical_proba)
				* (theo_proba - empirical_proba) / theo_proba;
		DEBUG_MESSAGE(
				utility_vector[i]<<","<<sampled_count[i]/(double)total_sampled_count);
	}DEBUG_MESSAGE("chi2: "<<chi2);
}

patSimulation::~patSimulation() {
	// TODO Auto-generated destructor stub
}

