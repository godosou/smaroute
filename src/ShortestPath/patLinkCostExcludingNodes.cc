/*
 * patLinkCostExcludingNodes.cc
 *
 *  Created on: Apr 25, 2012
 *      Author: jchen
 */

#include "patLinkCostExcludingNodes.h"

patLinkCostExcludingNodes::patLinkCostExcludingNodes(
		const patLinkCost* link_cost, set<const patNode*> excluded_nodes) :
		m_link_cost(link_cost), m_excluded_nodes(excluded_nodes) {

}

double patLinkCostExcludingNodes::getCost(const patArc* arc) const{
	if (m_excluded_nodes.find(arc->getUpNode()) != m_excluded_nodes.end()
			|| m_excluded_nodes.find(arc->getDownNode())
					!= m_excluded_nodes.end()) {
		return DBL_MAX;
	} else {
		return m_link_cost->getCost(arc);
	}
}

double patLinkCostExcludingNodes::getCost(const patArc* arc,const TransportMode& mode) const{
	//TODO multimodal
	if (m_excluded_nodes.find(arc->getUpNode()) != m_excluded_nodes.end()
			|| m_excluded_nodes.find(arc->getDownNode())
					!= m_excluded_nodes.end()) {
		return DBL_MAX;
	} else {
		return m_link_cost->getCost(arc);
	}
}


patLinkCostExcludingNodes::~patLinkCostExcludingNodes() {
	// TODO Auto-generated destructor stub
}
