/*
 * patLinkCostExcludingNodes.h
 *
 *  Created on: Apr 25, 2012
 *      Author: jchen
 */

#ifndef PATLINKCOSTEXCLUDINGNODES_H_
#define PATLINKCOSTEXCLUDINGNODES_H_
#include "patLinkAndPathCost.h"
#include "patNode.h"
#include "patArc.h"
#include "patTransportMode.h"
#include <set>
using namespace std;
class patLinkCostExcludingNodes: public patLinkAndPathCost {
public:
	patLinkCostExcludingNodes(const patLinkAndPathCost* link_cost, const set<const patNode*>& excluded_nodes);
	double getCost(const patRoadBase* arc) const;
	double getCost(const patRoadBase* arc, const TransportMode& mode) const;
	virtual ~patLinkCostExcludingNodes();
protected:
//	const patLinkAndPathCost* m_link_cost;
	const set<const patNode*>& m_excluded_nodes;
};

#endif /* PATLINKCOSTEXCLUDINGNODES_H_ */
