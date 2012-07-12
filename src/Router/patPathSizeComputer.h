/*
 * patPathSizeComputer.h
 *
 *  Created on: Jul 12, 2012
 *      Author: jchen
 */

#ifndef PATPATHSIZECOMPUTER_H_
#define PATPATHSIZECOMPUTER_H_

#include <set>
#include <map>
#include <tr1/unordered_map>
#include "patMultiModalPath.h"

class patArc;
class patPathSizeComputer {
public:
	patPathSizeComputer(const std::set<patMultiModalPath>& choice_set);
	patPathSizeComputer(const std::set<patMultiModalPath>& choice_set,
			const patMultiModalPath& chosen_alternative);
	patPathSizeComputer(const patPathSizeComputer& another);

	patPathSizeComputer* clone() const;
	void buildOverlap(const std::set<patMultiModalPath>& choice_set);
	void initiatePS(const std::set<patMultiModalPath>& choice_set);
	double computePS(const patMultiModalPath& path);
	double getPS(const patMultiModalPath& path);

	virtual ~patPathSizeComputer();
protected:
	std::tr1::unordered_map<const patArc*, int> m_arc_overlap;

	std::map<const patMultiModalPath, double> m_path_size;
};

#endif /* PATPATHSIZECOMPUTER_H_ */
