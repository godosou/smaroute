/*
 * patSimulateProbabilisticObs.cc
 *
 *  Created on: Jul 27, 2012
 *      Author: jchen
 */

#include "patSimulateProbabilisticObs.h"
#include <map>
using namespace std;
patSimulateProbabilisticObs::patSimulateProbabilisticObs(
		const patNetworkBase* network, const patRouter* router,
		const patRandomNumber* rnd) :
		m_network(network), m_router(router), m_rnd(rnd) {

}

void patSimulateProbabilisticObs::run(patObservation& observation) const {
	const map<const patMultiModalPath, double>& original_paths =
			observation.getPathProbas();
	patSimulateProbabilisticPaths simulate_paths(m_network, m_router, m_rnd);

	map<patMultiModalPath, double> all_paths;
	for (map<const patMultiModalPath, double>::const_iterator path_iter =
			original_paths.begin(); path_iter != original_paths.end();
			++path_iter) {

		map<patMultiModalPath, double> paths_with_errors = simulate_paths.run(
				path_iter->first,
				patNBParameters::the()->nbrOfSimulatedErrorPaths,
				patNBParameters::the()->errorInSimulatedObservations);

		map<patMultiModalPath, double>::iterator find_path = all_paths.find(
				path_iter->first);
		double proba_true_path = 1.0
				- patNBParameters::the()->errorInSimulatedObservations;
		if (find_path == all_paths.end()) {
			all_paths.insert(make_pair(path_iter->first, proba_true_path));
		} else {
			find_path->second += proba_true_path;
		}
		for (map<patMultiModalPath, double>::iterator iter_2 =
				paths_with_errors.begin(); iter_2 != paths_with_errors.end();
				++iter_2) {
			find_path = all_paths.find(iter_2->first);
			if (find_path == all_paths.end()) {
				all_paths.insert(make_pair(iter_2->first, iter_2->second));
			} else {
				find_path->second += iter_2->second;
			}
		}
	}
	for (map<const patMultiModalPath, double>::const_iterator path_iter =
			all_paths.begin(); path_iter != all_paths.end(); ++path_iter) {
		observation.addPath(path_iter->first, path_iter->second);
	}

}
patSimulateProbabilisticObs::~patSimulateProbabilisticObs() {
// TODO Auto-generated destructor stub
}

