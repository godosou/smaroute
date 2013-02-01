/*
 * patPathSetSimilarity.h
 *
 *  Created on: Sep 28, 2012
 *      Author: jchen
 */

#ifndef PATPATHSETSIMILARITY_H_
#define PATPATHSETSIMILARITY_H_

#include <set>
#include <map>
#include <tr1/unordered_map>
#include "patMultiModalPath.h"

class patPathSetSimilarity {
public:
	patPathSetSimilarity(const std::set<patMultiModalPath>& path_set);
	void buildOverlap();

	double computeMeanSimilarity() const;
	std::map<const patMultiModalPath, double> computeSimilarity(
			const std::set<patMultiModalPath>& choice_set) const;
	virtual ~patPathSetSimilarity();
protected:
	const std::set<patMultiModalPath>& m_path_set;
	std::tr1::unordered_map<const patArc*, int> m_arc_overlap;
};

#endif /* PATPATHSETSIMILARITY_H_ */
