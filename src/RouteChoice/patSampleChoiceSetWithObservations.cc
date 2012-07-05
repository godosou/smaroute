/*
 * patSampleChoiceSetWithObservations.cc
 *
 *  Created on: May 26, 2012
 *      Author: jchen
 */

#include "patSampleChoiceSetWithObservations.h"
#include "MHWeightFunction.h"
#include "MHPathGenerator.h"
#include "patNBParameters.h"
#include "patDisplay.h"
#include "patOd.h"
#include <boost/lexical_cast.hpp>
#include <set>
#include <unistd.h>

using namespace std;
//#include <thread>
patSampleChoiceSetWithObservations::patSampleChoiceSetWithObservations() {
	// TODO Auto-generated constructor stub

}

patSampleChoiceSetWithObservations::~patSampleChoiceSetWithObservations() {
	// TODO Auto-generated destructor stub
}

void patSampleChoiceSetWithObservations::averagePathProbas() {

	DEBUG_MESSAGE(
			"od counts"<<m_od_count.size()<<":"<<m_od_count.begin()->second);
	for (map<const patOd, map<const patMultiModalPath, double> >::const_iterator od_iter =
			m_od_path_probas.begin(); od_iter != m_od_path_probas.end();
			++od_iter) {
		for (map<const patMultiModalPath, double>::const_iterator path_iter =
				od_iter->second.begin(); path_iter != od_iter->second.end();
				++path_iter) {
			map<const patOd, int>::const_iterator find_od_count =
					m_od_count.find(od_iter->first);
			if (find_od_count == m_od_count.end()) {
				throw RuntimeException("not od count found");
			} else {

				m_path_obs_proba[path_iter->first] = path_iter->second
						/ (double) find_od_count->second;
			}
		}
	}
}

void patSampleChoiceSetWithObservations::sampleChoiceSet(
		MHPathGenerator* path_generator, string folder) {

	const string choice_set_folder = folder
			+ patNBParameters::the()->choiceSetFolder + "/";

	int num_threads = patNBParameters::the()->nbrOfThreads;
	if (m_observations.size() <= num_threads) {
		num_threads = m_observations.size();
	}
	DEBUG_MESSAGE( "threads: "<<num_threads<<", obs:"<<m_observations.size());

#pragma omp parallel num_threads( num_threads)

	{
#pragma omp for

		for (int j = 0; j < m_observations.size(); ++j) {
			cout << "START THREAD " << j;
			patNetworkBase* cloned_network =
					path_generator->getNetwork()->clone();

			MHPathGenerator* generator_clone = path_generator->clone();
			generator_clone->setNetwork(cloned_network);
			m_observations.at(j).sampleChoiceSet(generator_clone,
					choice_set_folder);

			delete generator_clone;
			generator_clone = NULL;
			delete cloned_network;
			cloned_network = NULL;
		}
	}
}
void patSampleChoiceSetWithObservations::addObservation(patObservation& obs,
		const bool& sample) {
	/**
	 * Get relevant OD first.
	 */
	if (obs.getId() == string("")) {
		obs.setId(boost::lexical_cast<string>(m_observations.size()));
	}

	map<const patMultiModalPath, double> path_probas =
			obs.getNormalizedPathProbas();
	set<patOd> od_set;

	unsigned long obs_id = 1;
	for (map<const patMultiModalPath, double>::const_iterator path_iter =
			path_probas.begin(); path_iter != path_probas.end(); ++path_iter) {
		patOd the_od(path_iter->first.getUpNode(),
				path_iter->first.getDownNode());
		pair<set<patOd>::const_iterator, bool> insert_result = od_set.insert(
				the_od);
		if (insert_result.second == true) {
//            DEBUG_MESSAGE("NEW OBSERVATION"<<obs.getId()+string("_")+boost::lexical_cast<string>(obs_id));
			if (sample == true) {
				patObservation new_obs;
				new_obs.setId(
						obs.getId() + string("_")
								+ boost::lexical_cast<string>(obs_id));
				new_obs.addPath(path_iter->first, path_iter->second);
				m_observations.push_back(new_obs);
				++obs_id;
			}
		}
		if (m_od_path_probas.find(the_od) == m_od_path_probas.end()) {
			m_od_path_probas[the_od];
			m_od_path_probas[the_od][path_iter->first] = 0.0;
		}
		m_od_path_probas[the_od][path_iter->first] += path_iter->second;
	}
	if (sample == false) {
		m_observations.push_back(obs);
	}
	for (set<patOd>::const_iterator od_iter = od_set.begin();
			od_iter != od_set.end(); ++od_iter) {
		if (m_od_count.find(*od_iter) == m_od_count.end()) {
			m_od_count[*od_iter] = 0;
		}
		++m_od_count[*od_iter];
	}
	//	DEBUG_MESSAGE("add a path finished");
}
