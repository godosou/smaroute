/*
 * patPathSimilarity.h
 *
 *  Created on: Jul 29, 2012
 *      Author: jchen
 */

#ifndef PATPATHSIMILARITY_H_
#define PATPATHSIMILARITY_H_
#include <tr1/unordered_set>
#include "patArcSequence.h"
class patPathSimilarity {
public:
	patPathSimilarity(const patArcSequence& a, const patArcSequence& b);

	void getArcsSet(std::tr1::unordered_set<const patArc*>& a_arcs_set,
			std::tr1::unordered_set<const patArc*>& b_arcs_set) const;
	void getNodesSet(std::tr1::unordered_set<const patNode*>& a_nodes_set,
			std::tr1::unordered_set<const patNode*>& b_nodes_set) const;
	double computeArcSim() const;
	double computeNodeSim() const;
	double computeDistance() const;
	virtual ~patPathSimilarity();
protected:
	const patArcSequence& m_a;
	const patArcSequence& m_b;
};

#endif /* PATPATHSIMILARITY_H_ */
