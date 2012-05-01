/*
 * patLinkCostExcludingNodes.h
 *
 *  Created on: Apr 25, 2012
 *      Author: jchen
 */

#ifndef PATLINKCOSTEXCLUDINGNODES_H_
#define PATLINKCOSTEXCLUDINGNODES_H_
#include "patLinkCost.h"
#include "patNode.h"
#include "patArc.h"
#include <set>
using namespace std;
class patLinkCostExcludingNodes: public patLinkCost {
public:
	patLinkCostExcludingNodes(const patLinkCost* link_cost, set<const patNode*> excluded_nodes);
	double getCost(const patArc* arc) const;
	double getCost(const patArc* arc, const TransportMode& mode) const;
	virtual ~patLinkCostExcludingNodes();
protected:
	const patLinkCost* m_link_cost;
	set<const patNode*> m_excluded_nodes;
};

#endif /* PATLINKCOSTEXCLUDINGNODES_H_ */
