/*
 * patSampleChoiceSetWithRandomWalk.cc
 *
 *  Created on: Jun 26, 2012
 *      Author: jchen
 */

#include "patSampleChoiceSetWithRandomWalk.h"
#include "patNBParameters.h"
#include "patDisplay.h"
#include "patOd.h"
#include <boost/lexical_cast.hpp>
#include <set>
#include <unistd.h>

patSampleChoiceSetWithRandomWalk::patSampleChoiceSetWithRandomWalk() {
	// TODO Auto-generated constructor stub

}

patSampleChoiceSetWithRandomWalk::~patSampleChoiceSetWithRandomWalk() {
	// TODO Auto-generated destructor stub
}

void patSampleChoiceSetWithRandomWalk::sampleChoiceSet(
		RWPathGenerator* path_generator, string folder) {

	const string choice_set_folder = folder + "/"
			+ patNBParameters::the()->choiceSetFolder + "/";

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
			RWPathGenerator* generator_clone = path_generator->clone();
			patNetworkBase* cloned_network =
					path_generator->getNetwork()->clone();
			generator_clone->setNetwork(cloned_network);

			m_observations.at(j).sampleChoiceSet(path_generator, choice_set_folder);
			delete generator_clone;
			generator_clone = NULL;
			delete cloned_network;
			cloned_network = NULL;
		}
	}
}
void patSampleChoiceSetWithRandomWalk::addObservation(patObservation& obs) {
/**
 * Get relevant OD first.
 */
if (obs.getId() == string("")) {
	obs.setId(boost::lexical_cast < string > (m_observations.size()));
}

map<const patMultiModalPath, double> path_probas =
		obs.getNormalizedPathProbas();
set < patOd > od_set;

unsigned long obs_id = 1;
for (map<const patMultiModalPath, double>::const_iterator path_iter =
		path_probas.begin(); path_iter != path_probas.end(); ++path_iter) {
	patOd the_od(path_iter->first.getUpNode(), path_iter->first.getDownNode());
	pair<set<patOd>::const_iterator, bool> insert_result = od_set.insert(
			the_od);
	if (insert_result.second == true) {
		patObservation new_obs;
		new_obs.setId(
				obs.getId() + string("_") + boost::lexical_cast < string
						> (obs_id));
		new_obs.addPath(path_iter->first, path_iter->second);
		m_observations.push_back(new_obs);
		++obs_id;
	}
}
}
