/*
 * patPathSetSimilarity.cc
 *
 *  Created on: Sep 28, 2012
 *      Author: jchen
 */

#include "patPathSetSimilarity.h"

patPathSetSimilarity::patPathSetSimilarity(
		const std::set<patMultiModalPath>& path_set) :
		m_path_set(path_set) {
	// TODO Auto-generated constructor stub
	buildOverlap();
}

patPathSetSimilarity::~patPathSetSimilarity() {
	// TODO Auto-generated destructor stub
}

double patPathSetSimilarity::computeMeanSimilarity() const {

	map<const patMultiModalPath, double> sim_set = computeSimilarity(
			m_path_set);

	double sim = 0.0;
	for (map<const patMultiModalPath, double>::const_iterator path_iter =
			sim_set.begin(); path_iter != sim_set.end(); ++path_iter) {
		sim += path_iter->second;
	}
	return sim / (double) sim_set.size();
}
void patPathSetSimilarity::buildOverlap() {

	for (set<patMultiModalPath>::const_iterator path_iter = m_path_set.begin();
			path_iter != m_path_set.end(); ++path_iter) {
		vector<const patArc*> arc_list = (*path_iter).getArcList();
		double pL = (*path_iter).getLength();
		if (pL <= 0.0) {
			WARNING("invalid path length");
			throw RuntimeException("invalid path length");
		}
		for (vector<const patArc*>::const_iterator arc_iter = arc_list.begin();
				arc_iter != arc_list.end(); ++arc_iter) {
			if (m_arc_overlap.find(*arc_iter) == m_arc_overlap.end()) {
				m_arc_overlap[*arc_iter] = 0;
			}
			m_arc_overlap[*arc_iter] += 1;
		}
	}
}
map<const patMultiModalPath, double> patPathSetSimilarity::computeSimilarity(
		const set<patMultiModalPath>& path_set) const {

	map<const patMultiModalPath, double> sim_set;

	for (set<patMultiModalPath>::const_iterator path_iter = path_set.begin();
			path_iter != path_set.end(); ++path_iter) {

		vector<const patArc*> arc_list = (*path_iter).getArcList();
		double ps = 0;
		double pL = (*path_iter).getLength();

		for (vector<const patArc*>::const_iterator arc_iter = arc_list.begin();
				arc_iter != arc_list.end(); ++arc_iter) {
			unordered_map<const patArc*, int>::const_iterator find_arc_overlap =
					m_arc_overlap.find(*arc_iter);
			if (find_arc_overlap == m_arc_overlap.end()) {
				throw RuntimeException("an arc not in path set");
			} else {
				ps += (*arc_iter)->getLength() * (double) find_arc_overlap->second
						/ (pL);

			}
		}
		sim_set[(*path_iter)] = ps/(double) path_set.size();
	}
	return sim_set;
}
