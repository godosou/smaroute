/*
 * patLinkCostExcludingNodes.cc
 *
 *  Created on: Apr 25, 2012
 *      Author: jchen
 */

#include "patLinkCostExcludingNodes.h"
#include "patDisplay.h"
patLinkCostExcludingNodes::patLinkCostExcludingNodes(
		const patLinkAndPathCost* link_cost, const set<const patNode*>& excluded_nodes) :
		patLinkAndPathCost::patLinkAndPathCost(*link_cost), m_excluded_nodes(excluded_nodes) {

}

double patLinkCostExcludingNodes::getCost(const patRoadBase* arc) const{
	if (m_excluded_nodes.find(arc->getUpNode()) != m_excluded_nodes.end()
			|| m_excluded_nodes.find(arc->getDownNode())
					!= m_excluded_nodes.end()) {
//		DEBUG_MESSAGE(arc->getUpNode()->getUserId()<<","<<arc->getDownNode()->getUserId());
		return DBL_MAX;
	} else {
		return patLinkAndPathCost::getCost(arc);
	}
}

double patLinkCostExcludingNodes::getCost(const patRoadBase* arc,const TransportMode& mode) const{
	//TODO multimodal
	if (m_excluded_nodes.find(arc->getUpNode()) != m_excluded_nodes.end()
			|| m_excluded_nodes.find(arc->getDownNode())
					!= m_excluded_nodes.end()) {
//		DEBUG_MESSAGE(arc->getUpNode()->getUserId()<<","<<arc->getDownNode()->getUserId());
		return DBL_MAX;
	} else {
		return patLinkAndPathCost::getCost(arc);
	}
}


patLinkCostExcludingNodes::~patLinkCostExcludingNodes() {
	// TODO Auto-generated destructor stub
}
