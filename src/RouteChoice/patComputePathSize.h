/*
 * deprecated!
 * patComputePathSize.h
 *
 *  Created on: May 3, 2012
 *      Author: jchen
 */

#ifndef PATCOMPUTEPATHSIZE_H_
#define PATCOMPUTEPATHSIZE_H_
#include "patMultiModalPath.h"
#include <tr1/unordered_map>
#include <set>
#include "patArc.h"
#include "patChoiceSet.h"
using namespace std;
using namespace std::tr1;
class patComputePathSize {
public:
	patComputePathSize();
	unordered_map<const patArc*, int> getOverlap(
			const set<patMultiModalPath>& choice_set) const;
	map<const patMultiModalPath, double> computePS(
			const set<patMultiModalPath>& choice_set,
			const unordered_map<const patArc*, int>& arc_overlap) const;
	map<const patMultiModalPath, double> computePS(
			const set<patMultiModalPath>& choice_set) const;
	map<const patMultiModalPath, double> computePS(
			const patChoiceSet& choice_set) const;
	map<const patMultiModalPath, double> computePS(
			const patMultiModalPath& chosen_alternative,
			const patChoiceSet& choice_set) const;


	/**
	 * Compute the mean similarity indicator of a set of paths;
	 */
	double computeMeanSimilarity(
			const set<patMultiModalPath>& choice_set) const;

	/**
	 * Compute the similarity indicator for each path in the set.
	 */
	map<const patMultiModalPath, double> computeSimilarity(
			const set<patMultiModalPath>& choice_set,
			const unordered_map<const patArc*, int>& arc_overlap) const;

	virtual ~patComputePathSize();
};

#endif /* PATCOMPUTEPATHSIZE_H_ */
