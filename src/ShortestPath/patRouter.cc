/*
 * patRouter.cc
 *
 *  Created on: Apr 20, 2012
 *      Author: jchen
 */

#include "patRouter.h"
#include "patException.h"
#include "patLinkCostExcludingNodes.h"
patRouter::patRouter(const patNetworkBase* network, const patLinkCost* link_cost) :
		m_network(network), m_link_cost(link_cost) {

}

patRouter::~patRouter() {
}

// -------------------- INTERNALS --------------------

//TODO
//FIXE treCost algorithm
//CHECK shortestpathtree for backward

patShortestPathTreeGeneral patRouter::treeCost(const patNode* root,
		set<const patNode*> targets, const Direction direction) const{
	/*
	 * (1) define set of feasible targets
	 */
	patShortestPathTreeGeneral shortest_path_tree(direction);
	set<const patNode*> targets_left;
	if (targets.empty()) {
		targets_left.insert(NULL);
	} else {
		targets_left.insert(targets.begin(), targets.end());
	}
	/*
	 * (2) initialize data structures and search loop
	 */
	patNode* current_node;
	double minimum_label = m_network->getMinimumLabel();
	shortest_path_tree.setLabel(root, 0.0);
	shortest_path_tree.insertRoot(root);

	deque<const patNode*> list_of_nodes;
	list_of_nodes.push_back(root);

	const map<const patNode*, set<const patRoadBase*> >* incidents;

	if (direction == FWD) {
//		DEBUG_MESSAGE("FORWARD");
		incidents = m_network->getOutgoingIncidents();
	} else if (direction == BWD) {
		//DEBUG_MESSAGE("BACKWARD");
		incidents = m_network->getIncomingIncidents();
	}

	else{
		throw RuntimeException("wrong directin parameter");
	}
	/*
	 * (3) search until all reachable targets are found
	 */
	while (!targets_left.empty() && !list_of_nodes.empty()) {
		const patNode* node_to_process = *list_of_nodes.begin();
		list_of_nodes.pop_front();
		targets_left.erase(node_to_process);

		/*
		 * (3-A) expand forwards
		 */

		map<const patNode*, set<const patRoadBase*> >::const_iterator find_out_going =
				incidents->find(node_to_process);
		if (find_out_going == incidents->end()) {

			continue;
		}
		for (set<const patRoadBase*>::const_iterator outgoing_road_iter =
				find_out_going->second.begin();
				outgoing_road_iter != find_out_going->second.end();
				++outgoing_road_iter) {
			const patNode* down_node;

			if (direction == FWD) {
				down_node=(*outgoing_road_iter)->getDownNode();
			} else if (direction == BWD) {
				down_node=(*outgoing_road_iter)->getUpNode();
			}
			const double road_cost = m_link_cost->getCost(*outgoing_road_iter);

			double down_node_label = shortest_path_tree.getLabel(down_node);
			if (down_node_label
					> shortest_path_tree.getLabel(node_to_process) + road_cost) {
				shortest_path_tree.setLabel(
						down_node,
						shortest_path_tree.getLabel(node_to_process)
								+ road_cost);

				if (shortest_path_tree.getLabel(down_node) < 0.0000001) {
					WARNING("NEGATIVE CYCLE DETECTED");
					throw RuntimeException("NEGATIVE CYCLE DETECTED");
				}

				shortest_path_tree.setPredecessor(down_node,
						*outgoing_road_iter);

				shortest_path_tree.insertSuccessor(node_to_process,
						*outgoing_road_iter);

				// Add the node following Bertsekas (1993)
				if (list_of_nodes.empty()) {
					list_of_nodes.push_back(down_node);
				} else {
					double top_label = shortest_path_tree.getLabel(
							list_of_nodes.front());
					if (down_node_label <= top_label) {
						list_of_nodes.push_front(down_node);
					} else {
						list_of_nodes.push_back(down_node);
					}
				}
			}
		}
	}
	return shortest_path_tree;
}

// -------------------- ROUTING IMPLEMENTATIONS --------------------
patShortestPathTreeGeneral patRouter::fwdCost(const patNode* origin,
		set<const patNode*> destinations) const{
	return treeCost(origin, destinations, FWD);
}
patShortestPathTreeGeneral patRouter::bwdCost(set<const patNode*> origins,
		const patNode* destination) const{
//	DEBUG_MESSAGE(*destination);
	return treeCost(destination, origins, BWD);
}

patShortestPathTreeGeneral patRouter::fwdCost(const patNode* origin,
		const patNode* destination) const{
	set<const patNode*> destinations;
	//destinations.insert(destination);
	return fwdCost(origin, destinations);
}

patShortestPathTreeGeneral patRouter::fwdCost(const patNode* origin) const{
	return fwdCost(origin, m_network->getNodes());
}
patShortestPathTreeGeneral patRouter::bwdCost(const patNode* destination) const{
	return bwdCost(m_network->getNodes(), destination);
}

patShortestPathTreeGeneral patRouter::bwdCost(const patNode* origin,
		const patNode* destination) const{
	set<const patNode*> origins;
	origins.insert(origin);
	//DEBUG_MESSAGE(*destination);
	return bwdCost(origins, destination);
}

map<const patNode*, patMultiModalPath> patRouter::bestRoutes(
		set<const patNode*> origins, const patNode* destination,
		const patShortestPathTreeGeneral* bwdCostTree) const{
	map<const patNode*, patMultiModalPath> result;
	for (set<const patNode*>::const_iterator origin_iter = origins.begin();
			origin_iter != origins.end(); ++origin_iter) {
		bool create_path_success = true;
		patMultiModalPath new_path(bwdCostTree->getShortestPathTo(*origin_iter),
				create_path_success);
		if (create_path_success == false) {
			DEBUG_MESSAGE("invalid path");
			//throw RuntimeException("invalid path");
			continue;
		}
		result[*origin_iter] = new_path;

	}
	return result;
}

map<const patNode*, patMultiModalPath> patRouter::bestRoutes(
		const patNode* origin, set<const patNode*> destinations,
		const patShortestPathTreeGeneral* fwdCostTree) const{
	map<const patNode*, patMultiModalPath> result;
	for (set<const patNode*>::const_iterator dest_iter = destinations.begin();
			dest_iter != destinations.end(); ++dest_iter) {
		bool create_path_success = true;
		patMultiModalPath new_path(fwdCostTree->getShortestPathTo(*dest_iter),
				create_path_success);
		if (create_path_success == false) {
			DEBUG_MESSAGE("invalid path");
			//throw RuntimeException("invalid path");
			continue;
		}
		result[*dest_iter] = new_path;

	}
	return result;
}

patMultiModalPath patRouter::bestRouteBwd(const patNode*origin,
		const patNode* destination, const patShortestPathTreeGeneral* bwdCost) const {
	set<const patNode*> origins;
	origins.insert(origin);
	return bestRoutes(origins, destination, bwdCost)[origin];
}

patMultiModalPath patRouter::bestRouteFwd(const patNode* origin,
		const patNode* destination, const patShortestPathTreeGeneral* fwdCost) const{
	set<const patNode*> destinations;
	destinations.insert(destination);
	return bestRoutes(origin, destinations, fwdCost)[destination];
}

map<const patNode*, patMultiModalPath> patRouter::bestRoutes(
		const patNode* origin, set<const patNode*> destinations) const{
	patShortestPathTreeGeneral fwd_cost = fwdCost(origin, destinations);
	return bestRoutes(origin, destinations, &fwd_cost);
}

map<const patNode*, patMultiModalPath> patRouter::bestRoutes(
		set<const patNode*> origins, const patNode* destination) const{
	patShortestPathTreeGeneral bwd_cost = bwdCost(origins, destination);
	return bestRoutes(origins, destination, &bwd_cost);
}

patMultiModalPath patRouter::bestRoute(const patNode* origin,
		const patNode* destination) const{
	set<const patNode*> destinations;
	destinations.insert(destination);
	return bestRoutes(origin, destinations)[destination];
}

// public double bestRouteCost(final Node origin, final Node destination) {
//
//
// }

// -------------------- SUPPLEMENTARY IMPLEMENTATIONS --------------------

// -------------------- TODO FROM UpdateRouter --------------------

patShortestPathTreeGeneral patRouter::costWithoutExcludedNodes(const patNode* root,
		set<const patNode*> allNodes, set<const patNode*> excludedNodes,
		const patShortestPathTreeGeneral* treeCost, Direction direction) const{
	return costWithoutExcludedNodes2(root, excludedNodes, treeCost,direction);
}

patShortestPathTreeGeneral patRouter::costWithoutExcludedNodes2(const patNode* root,
		set<const patNode*> excludedNodes, const  patShortestPathTreeGeneral* treeCost,
		Direction direction) const{
	/*
	 * (1) simple case: the root node is excluded
	 */

	if (excludedNodes.find(root) != excludedNodes.end()) {
		//DEBUG_MESSAGE("root is excluded");
		patShortestPathTreeGeneral empty_tree(direction);
		empty_tree.insertRoot(root);
		return empty_tree;
	}
	/*
	 * (2) compute completely new tree cost
	 */
	patLinkCostExcludingNodes myLinkCost(m_link_cost, excludedNodes);
	//DEBUG_MESSAGE(excludedNodes.size());
	patRouter myRouter(m_network, &myLinkCost);
	patShortestPathTreeGeneral myTreeCost = myRouter.treeCost(root,
			set<const patNode*>(), direction);
	//DEBUG_MESSAGE("OK");
	/*
	 * (3) simply return the result
	 */
	return myTreeCost;
}

patShortestPathTreeGeneral patRouter::fwdCostWithoutExcludedNodes(
		const patNode* origin, set<const patNode*> allNodes,
		set<const patNode*> excludedNodes,
		const patShortestPathTreeGeneral* fwdCost) const{
	return costWithoutExcludedNodes(origin, allNodes, excludedNodes, fwdCost,
			FWD);
}

patShortestPathTreeGeneral patRouter::bwdCostWithoutExcludedNodes(
		const patNode* destination, set<const patNode*> allNodes,
		set<const patNode*> excludedNodes,
		const patShortestPathTreeGeneral* bwdCost) const{
	return costWithoutExcludedNodes(destination, allNodes, excludedNodes,
			bwdCost, BWD);
}


const patNetworkBase* patRouter::getNetwork() const{
	return m_network;
}
