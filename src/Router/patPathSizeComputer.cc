/*
 * patPathSizeComputer.cc
 *
 *  Created on: Jul 12, 2012
 *      Author: jchen
 */

#include "patPathSizeComputer.h"
#include "patArc.h"
#include "patDisplay.h"
#include "patException.h"

using namespace std;
patPathSizeComputer::~patPathSizeComputer() {
	cout << " ps no found: " << m_ps_not_found << endl;
	// TODO Auto-generated destructor stub
}

patPathSizeComputer::patPathSizeComputer(
		const set<patMultiModalPath>& choice_set) {
	m_ps_not_found = 0.0;
	buildOverlap(choice_set);
	initiatePS(choice_set);
}

patPathSizeComputer::patPathSizeComputer(const patPathSizeComputer& another) {
	m_ps_not_found = 0.0;
	m_arc_overlap = another.m_arc_overlap;
	m_path_size = another.m_path_size;
}
patPathSizeComputer::patPathSizeComputer(
		const set<patMultiModalPath>& choice_set,
		const patMultiModalPath& chosen_alternative) {
	m_ps_not_found = 0.0;
	set<patMultiModalPath> choice_set_copy = choice_set;
	choice_set_copy.insert(chosen_alternative);

	buildOverlap(choice_set_copy);
	initiatePS(choice_set_copy);
}

patPathSizeComputer* patPathSizeComputer::clone() const {
	return new patPathSizeComputer(*this);
}

void patPathSizeComputer::buildOverlap(
		const set<patMultiModalPath>& choice_set) {

	for (set<patMultiModalPath>::const_iterator path_iter = choice_set.begin();
			path_iter != choice_set.end(); ++path_iter) {
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

void patPathSizeComputer::initiatePS(const set<patMultiModalPath>& choice_set) {

	for (set<patMultiModalPath>::const_iterator path_iter = choice_set.begin();
			path_iter != choice_set.end(); ++path_iter) {
		computePS(*path_iter);
	}
}

double patPathSizeComputer::computePS(const patMultiModalPath& path) {

	vector<const patArc*> arc_list = path.getArcList();
	double ps = 0;
	double pL = path.getLength();

	for (vector<const patArc*>::const_iterator arc_iter = arc_list.begin();
			arc_iter != arc_list.end(); ++arc_iter) {
		vector<const patArc*> original_arc_list =
				(*arc_iter)->getOriginalArcList();
		for (vector<const patArc*>::const_iterator o_arc_iter =
				original_arc_list.begin();
				o_arc_iter != original_arc_list.end(); ++o_arc_iter) {
			unordered_map<const patArc*, int>::const_iterator find_arc_overlap =
					m_arc_overlap.find(*o_arc_iter);
			if (find_arc_overlap != m_arc_overlap.end()) {

				ps += (*o_arc_iter)->getLength()
						/ (pL * (double) find_arc_overlap->second);
			} else {
				continue;
				//			throw RuntimeException("an arc not in path set");

			}

		}
	}
	if (ps == 0.0) {
		cout << "!!!!!!!!WRONG PS SPECIFICATION!!!!!!!!!";
	}
	m_path_size[path] = ps;
	return ps;

}
double patPathSizeComputer::getPS(const patMultiModalPath& path) {
	if (m_path_size.empty()) {
		throw RuntimeException("No path to calculate the ps");
	}
	map<const patMultiModalPath, double>::const_iterator find_path =
			m_path_size.find(path);

	if (find_path == m_path_size.end()) {
//		cout << "\t\t path not found in ps, compute it." << endl;
		m_ps_not_found++;
		return computePS(path);
	} else {
		return find_path->second;
	}
}
