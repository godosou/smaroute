/*
 * patShortestpathTreeGeneral.h
 *
 *  Created on: Nov 1, 2011
 *      Author: jchen
 */

#ifndef PATSHORTESTPATHTREEGENERAL_H_
#define PATSHORTESTPATHTREEGENERAL_H_
#include "patRoadBase.h"
#include "patNode.h"
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <set>
#include <list>
enum Direction {
	FWD, BWD
};
using namespace std::tr1;
class patShortestPathTreeGeneral {
public:
	patShortestPathTreeGeneral();
	patShortestPathTreeGeneral(Direction direction);

	bool getShortestPathTo(list<const patRoadBase*>& list_of_roads,
			const patNode* end_node) const;

	/**
	 * Set the label for a node;
	 */

	void setLabel(const patNode* node, double value);

	/**
	 * insert a root node
	 */
	void insertRoot(const patNode* node);

	/**
	 * Get the label of a node, default is patMaxReal.
	 */
	double getLabel(const patNode* node) const;
	const unordered_map<const patNode*, double>& getLabels() const;

	bool empty() const;
	void clear();
	void setPredecessor(const patNode* node, const patRoadBase* road);

	bool compareLabel(const patNode* a, const patNode* b)  {
		unordered_map<const patNode*, double>::const_iterator find_a =
				m_labels.find(a);
		unordered_map<const patNode*, double>::const_iterator find_b =
				m_labels.find(b);
		if (find_a != m_labels.end() && find_b != m_labels.end()) {
			if (find_a->second < find_b->second) {
				return true;
			} else if (find_a->second > find_b->second) {
				return false;
			}
		} else if (find_a != m_labels.end() && find_b == m_labels.end()) {
			return true;
		} else if (find_a == m_labels.end() && find_b != m_labels.end()) {
			return false;
		}
		return a< b;
	}
	const patNode* getPredecessor(const patNode* node, Direction direction) const;
	//void insertSuccessor(const patNode* node, const patRoadBase* road);
protected:
	unordered_map<const patNode*, double> m_labels;
	unordered_set<const patNode*> m_root;
	unordered_map<const patNode*, const patRoadBase*> m_predecessors;
	Direction m_direction;
};

typedef bool (patShortestPathTreeGeneral::*SpMemFn)(const patNode* a,
		const patNode* b) ;

class CompareNodeFunctor {
protected:
	 patShortestPathTreeGeneral* m_sp_tree;
	 SpMemFn m_spm;
public:
	CompareNodeFunctor( patShortestPathTreeGeneral* sp_tree,
			 SpMemFn spm) :
			m_sp_tree(sp_tree), m_spm(spm) {

	}
	;
	bool operator()(const patNode* a, const patNode* b) {
		return (m_sp_tree->*m_spm)(a,b);
	}
};
#endif /* PATSHORTESTPATHTREEGENERAL_H_ */
