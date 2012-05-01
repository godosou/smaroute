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
#include <map>
#include <set>
#include <list>
enum Direction{FWD,BWD};
class patShortestPathTreeGeneral {
public:
	patShortestPathTreeGeneral();
	patShortestPathTreeGeneral(Direction direction);

	list<const patRoadBase*> getShortestPathTo(const patNode* end_node) const ;

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
	map<const patNode*, double> getLabels() const;

	bool empty() const;
	void clear();
	void setPredecessor(const patNode* node, const patRoadBase* road);
	void insertSuccessor(const patNode* node, const patRoadBase* road);
	virtual ~patShortestPathTreeGeneral();
protected:
	map<const patNode*, double> m_labels;
	set<const patNode* > m_root;
	map<const patNode*,  const patRoadBase*> m_predecessors;
	map<const patNode*, map<const patNode*, const patRoadBase*> > m_successors;
	Direction m_direction;
};

#endif /* PATSHORTESTPATHTREEGENERAL_H_ */
