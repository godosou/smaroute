/*
 *  deprecated!
 * patComputePathSize.cc
 *
 *  Created on: May 3, 2012
 *      Author: jchen
 */

#include "patComputePathSize.h"
#include "patNBParameters.h"
#include "patArc.h"
#include "patDisplay.h"
#include "patException.h"
patComputePathSize::patComputePathSize() {
	// TODO Auto-generated constructor stub

}

patComputePathSize::~patComputePathSize() {
	// TODO Auto-generated destructor stub
}

unordered_map<const patArc*, int> patComputePathSize::getOverlap(
		const set<patMultiModalPath>& choice_set) const {

	unordered_map<const patArc*, int> arc_overlap;
	for (set<patMultiModalPath>::const_iterator path_iter = choice_set.begin();
			path_iter != choice_set.end(); ++path_iter) {
//		DEBUG_MESSAGE("OK");
		vector<const patArc*> arc_list = (*path_iter).getArcList();
		double pL = (*path_iter).getLength();
		if (pL <= 0.0) {
			WARNING("invalid path length");
			throw RuntimeException("invalid path length");
		}
		for (vector<const patArc*>::const_iterator arc_iter = arc_list.begin();
				arc_iter != arc_list.end(); ++arc_iter) {
			if (arc_overlap.find(*arc_iter) == arc_overlap.end()) {
				arc_overlap[*arc_iter] = 0;
			}
			arc_overlap[*arc_iter] += 1;
		}
	}
	return arc_overlap;
}

double patComputePathSize::computeMeanSimilarity(
		const set<patMultiModalPath>& choice_set) const {

	unordered_map<const patArc*, int> arc_overlap = getOverlap(choice_set);
	map<const patMultiModalPath, double> sim_set = computeSimilarity(choice_set,
			arc_overlap);

	double sim=0.0;
	for (map<const patMultiModalPath, double>::const_iterator path_iter =
			sim_set.begin(); path_iter != sim_set.end(); ++path_iter) {
		sim+=path_iter->second;
	}
	return sim/(double)sim_set.size();
}
map<const patMultiModalPath, double> computeSimilarity(
		const set<patMultiModalPath>& choice_set,
		const unordered_map<const patArc*, int>& arc_overlap) const {

	map<const patMultiModalPath, double> sim_set;

	for (set<patMultiModalPath>::const_iterator path_iter = choice_set.begin();
			path_iter != choice_set.end(); ++path_iter) {

		vector<const patArc*> arc_list = (*path_iter).getArcList();
		double ps = 0;
		double pL = (*path_iter).getLength();

		for (vector<const patArc*>::const_iterator arc_iter = arc_list.begin();
				arc_iter != arc_list.end(); ++arc_iter) {
			unordered_map<const patArc*, int>::const_iterator find_arc_overlap =
					arc_overlap.find(*arc_iter);
			if (find_arc_overlap == arc_overlap.end()) {
				throw RuntimeException("an arc not in path set");
			} else {
				ps += (*arc_iter)->getLength() * find_arc_overlap->second
						/ (pL);

			}
		}
		sim_set[(*path_iter)] = ps;
	}
	return sim_set;
}
map<const patMultiModalPath, double> patComputePathSize::computePS(
		const set<patMultiModalPath>& choice_set,
		const unordered_map<const patArc*, int>& arc_overlap) const {

	map<const patMultiModalPath, double> ps_set;

	for (set<patMultiModalPath>::const_iterator path_iter = choice_set.begin();
			path_iter != choice_set.end(); ++path_iter) {

		vector<const patArc*> arc_list = (*path_iter).getArcList();
		double ps = 0;
		double pL = (*path_iter).getLength();

		for (vector<const patArc*>::const_iterator arc_iter = arc_list.begin();
				arc_iter != arc_list.end(); ++arc_iter) {
			unordered_map<const patArc*, int>::const_iterator find_arc_overlap =
					arc_overlap.find(*arc_iter);
			if (find_arc_overlap == arc_overlap.end()) {
				throw RuntimeException("an arc not in path set");
			} else {
				ps += (*arc_iter)->getLength()
						/ (pL * find_arc_overlap->second);

			}
		}
		ps_set[(*path_iter)] = ps;
	}
	return ps_set;
}

map<const patMultiModalPath, double> patComputePathSize::computePS(
		const set<patMultiModalPath>& choice_set) const {

	map<const patMultiModalPath, double> ps_set;
	unordered_map<const patArc*, int> arc_overlap = getOverlap(choice_set);
//	DEBUG_MESSAGE("Path overlapping summarized");
	return computePS(choice_set, arc_overlap);

}

map<const patMultiModalPath, double> patComputePathSize::computePS(
		const patMultiModalPath& chosen_alternative,
		const patChoiceSet& choice_set) const {
	set<patMultiModalPath> paths = choice_set.getChoiceSet();
//	DEBUG_MESSAGE("compute path size with paths: "<<paths.size());
	paths.insert(chosen_alternative);
	return computePS(paths);
}
map<const patMultiModalPath, double> patComputePathSize::computePS(
		const patChoiceSet& choice_set) const {
//	DEBUG_MESSAGE("compute path size")
	set<patMultiModalPath> paths = choice_set.getChoiceSet();
//	DEBUG_MESSAGE("choice set got");
	return computePS(paths);
}
