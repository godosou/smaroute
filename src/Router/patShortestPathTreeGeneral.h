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
enum Direction{FWD,BWD};
using namespace std::tr1;
class patShortestPathTreeGeneral {
public:
	patShortestPathTreeGeneral();
	patShortestPathTreeGeneral(Direction direction);

    bool getShortestPathTo(list<const patRoadBase*>& list_of_roads, const patNode* end_node) const ;

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
	//void insertSuccessor(const patNode* node, const patRoadBase* road);
protected:
	unordered_map<const patNode*, double> m_labels;
	unordered_set<const patNode* > m_root;
	unordered_map<const patNode*,  const patRoadBase*> m_predecessors;
	Direction m_direction;
};

#endif /* PATSHORTESTPATHTREEGENERAL_H_ */
