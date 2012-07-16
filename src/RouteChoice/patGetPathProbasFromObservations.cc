/*
 * patGetPathProbasFromObservations.cc
 *
 *  Created on: Jul 11, 2012
 *      Author: jchen
 */

#include "patGetPathProbasFromObservations.h"
#include <set>
#include "patDisplay.h"
using namespace std;
patGetPathProbasFromObservations::patGetPathProbasFromObservations() {
	// TODO Auto-generated constructor stub

}

patGetPathProbasFromObservations::~patGetPathProbasFromObservations() {
	// TODO Auto-generated destructor stub
}

const map<const patMultiModalPath, double> patGetPathProbasFromObservations::getPathProbas(
		const vector<patObservation>& observations) const {

	map<const patMultiModalPath, double> path_obs_proba;
	map<const patOd, map<const patMultiModalPath, double> > od_path_probas;
	map<const patOd, int> od_count;

	const map<const patMultiModalPath, double> path_probas;
	for (vector<patObservation>::const_iterator obs_iter = observations.begin();
			obs_iter != observations.end(); ++obs_iter) {

		map<const patMultiModalPath, double> path_probas =
				(*obs_iter).getNormalizedPathProbas();
		set<patOd> od_set;
		for (map<const patMultiModalPath, double>::const_iterator path_iter =
				path_probas.begin(); path_iter != path_probas.end();
				++path_iter) {
//			DEBUG_MESSAGE(path_iter->second);
			patOd the_od(path_iter->first.getUpNode(),
					path_iter->first.getDownNode());
			od_set.insert(the_od);
			map<const patOd, map<const patMultiModalPath, double> >::iterator find_od =
					od_path_probas.find(the_od);
			if (find_od == od_path_probas.end()) {
				od_path_probas[the_od];
				od_path_probas[the_od][path_iter->first] = path_iter->second;
			} else {
				map<const patMultiModalPath, double>::iterator find_path =
						find_od->second.find(path_iter->first);

				if (find_path == find_od->second.end()) {
					od_path_probas[the_od][path_iter->first] = path_iter->second;
				} else {

					find_path->second += path_iter->second;
				}
			}
		}
		for (set<patOd>::const_iterator od_iter = od_set.begin();
				od_iter != od_set.end(); ++od_iter) {
			if (od_count.find(*od_iter) == od_count.end()) {
				od_count[*od_iter] = 0;
			}
			++od_count[*od_iter];
		}
	}

	DEBUG_MESSAGE( "od counts"<<od_count.size()<<":"<<od_count.begin()->second);
	for (map<const patOd, map<const patMultiModalPath, double> >::const_iterator od_iter =
			od_path_probas.begin(); od_iter != od_path_probas.end();
			++od_iter) {
		for (map<const patMultiModalPath, double>::const_iterator path_iter =
				od_iter->second.begin(); path_iter != od_iter->second.end();
				++path_iter) {
			map<const patOd, int>::const_iterator find_od_count = od_count.find(
					od_iter->first);
			if (find_od_count == od_count.end()) {
				throw RuntimeException("not od count found");
			} else {

				path_obs_proba[path_iter->first] = path_iter->second
						/ (double) find_od_count->second;
			}
//			DEBUG_MESSAGE( path_obs_proba[path_iter->first]);
		}
	}
	DEBUG_MESSAGE("Path proba size"<<path_obs_proba.size());
	return path_obs_proba;
}
