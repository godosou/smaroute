/*
 * patSampleChoiceSetForObservations.cpp
 *
 *  Created on: Jul 11, 2012
 *      Author: jchen
 */

#include "patSampleChoiceSetForObservations.h"

#include "patNBParameters.h"
#include "patDisplay.h"
#include "patOd.h"
#include <boost/lexical_cast.hpp>
#include <set>
#include <unistd.h>
patSampleChoiceSetForObservations::patSampleChoiceSetForObservations() {
	// TODO Auto-generated constructor stub

}

patSampleChoiceSetForObservations::~patSampleChoiceSetForObservations() {
	// TODO Auto-generated destructor stub
}

void patSampleChoiceSetForObservations::sampleChoiceSet(
		const vector<patObservation>& observations,
		patPathGenerator* path_generator, string folder) {

	for (vector<patObservation>::const_iterator obs_iter = observations.begin();
			obs_iter != observations.end(); ++obs_iter) {
		addObservation(*obs_iter);
	}
	const string choice_set_folder = folder;

	int num_threads = patNBParameters::the()->nbrOfThreads;
	if (m_observations.size() <= num_threads) {
		num_threads = m_observations.size();
	}
	const int num_blocks = ceil((double) m_observations.size() / num_threads);
	DEBUG_MESSAGE(
			"threads: " << num_threads << ", " << "blocks: " << num_blocks << ", obs:" << m_observations.size());

	DEBUG_MESSAGE("path generators and networks cloned");
#pragma omp parallel num_threads( num_threads)

	{
#pragma omp for

		for (int j = 0; j < m_observations.size(); ++j) {
			patNetworkBase* cloned_network =
					path_generator->getNetwork()->clone();

			patPathGenerator* generator_clone = path_generator->clone();
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
void patSampleChoiceSetForObservations::addObservation(
		const patObservation& obs) {
	/**
	 * Get relevant OD first.
	 */

	string orig_obs_id;
	if (obs.getId() == string("")) {
		orig_obs_id = boost::lexical_cast<string>(m_observations.size() + 1);
	} else {
		orig_obs_id = obs.getId();
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
			patObservation new_obs;
			new_obs.setId(
					orig_obs_id + string("_")
							+ boost::lexical_cast<string>(obs_id));
			new_obs.addPath(path_iter->first, path_iter->second);
			m_observations.push_back(new_obs);
			++obs_id;
		}
	}
}
