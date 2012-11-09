/*
 * patNetworkReducer.cc
 *
 *  Created on: Apr 28, 2012
 *      Author: jchen
 */

#include "patNetworkReducer.h"
#include "patException.h"
#include "patRouter.h"

patNetworkReducer::patNetworkReducer(const patNode* origin,
		const patNode* destination, const patLinkAndPathCost* linkCost,
		double expansion) {
	if (origin == NULL) {
		throw IllegalArgumentException("origin is null");
	}
	if (destination == NULL) {
		throw IllegalArgumentException("destination is null");
	}
	if (linkCost == NULL) {
		throw IllegalArgumentException("link cost is null");
	}
	if (expansion < 1.0) {
		throw IllegalArgumentException("expansion < 1.0");
	}
	m_origin = origin;
	m_destination = destination;
	m_linkCost = linkCost;
	m_expansion = expansion;
}
patNetworkReducer::~patNetworkReducer() {
	// TODO Auto-generated destructor stub
}

patNetworkReducer::patNetworkReducer(const patNode* origin,
		const patNode* destination, const patLinkAndPathCost* linkCost) {
	if (origin == NULL) {
		throw IllegalArgumentException("origin is null");
	}
	if (destination == NULL) {
		throw IllegalArgumentException("destination is null");
	}
	if (linkCost == NULL) {
		throw IllegalArgumentException("link cost is null");
	}
	m_origin = origin;
	m_destination = destination;
	m_linkCost = linkCost;
	m_expansion = DBL_MAX;
}

void patNetworkReducer::reduce(patNetworkBase* network) {
	/*
	 * (0) preparatory computations
	 */
	patRouter router(network, m_linkCost);

	set<const patNode*> all_nodes = network->getNodes();
	patShortestPathTreeGeneral fwdCost(FWD);
	router.fwdCost(fwdCost, m_origin, all_nodes);
	patShortestPathTreeGeneral bwdCost(BWD);
	router.bwdCost(bwdCost, all_nodes, m_destination);
	//FIXME
//    m_expansion = 2;
	double minCost = fwdCost.getLabel(m_destination);
	double maxCost = m_expansion * minCost;
	cout << m_expansion << "," << minCost << "-" << maxCost<<endl;
	/*
	 * (1) identify nodes that are too far away
	 */
	set<const patNode*> remove_nodes;
	for (set<const patNode*>::const_iterator node_iter = all_nodes.begin();
			node_iter != all_nodes.end(); ++node_iter) {
		if((*node_iter)->getUserId()==1096678549){
			cout<<"node 1096678549"<<fwdCost.getLabel(*node_iter) <<","<< bwdCost.getLabel(*node_iter)<<":"<<maxCost<<endl;
		}
//		cout<<fwdCost.getLabel(*node_iter) + bwdCost.getLabel(*node_iter)<<"?"<<maxCost<<endl;
		if (fwdCost.getLabel(*node_iter) + bwdCost.getLabel(*node_iter)
				> maxCost) {

			remove_nodes.insert(*node_iter);
		}
	}
	for (set<const patNode*>::const_iterator node_iter = remove_nodes.begin();
			node_iter != remove_nodes.end(); ++node_iter) {
		network->removeNode(*node_iter);
	}
	network->finalizeNetwork();
	/*
	 * (3) remove unconnected nodes
	 */
}
