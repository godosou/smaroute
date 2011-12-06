/*
 * patShortestPathGeneral.cc
 *
 *  Created on: Nov 1, 2011
 *      Author: jchen
 */

#include "patShortestPathGeneral.h"
#include "patRoadBase.h"
#include "patConst.h"
#include "patGpsDDR.h"
#include "patDisplay.h"
#include "patMeasurementDDR.h"
patShortestPathGeneral::patShortestPathGeneral(patNetworkBase* a_network ) :
		network(a_network), minimum_label(a_network->getMinimumLabel()) {
	//DEBUG_MESSAGE("minimum_label "<<minimum_label);
}

patShortestPathGeneral::~patShortestPathGeneral() {

}

bool patShortestPathGeneral::buildShortestPathTree(const patNode* root_node,
		double ceil) {
	set<const patNode*> root_nodes;
	root_nodes.insert(root_node);
	return buildShortestPathTree(root_nodes, NULL, NULL, ceil);
}

bool patShortestPathGeneral::buildShortestPathTree(const patNode* root_node, patMeasurementDDR* gps_ddr,
		set<pair<const patArc*, const patRoadBase*> >* ddr_arcs
		, double ceil ){
	set<const patNode*> root_nodes;
	root_nodes.insert(root_node);
	return buildShortestPathTree(root_nodes, gps_ddr, ddr_arcs, ceil);

}
bool patShortestPathGeneral::buildShortestPathTree(set<const patNode*> root_nodes,
		patMeasurementDDR* gps_ddr,
		set<pair<const patArc*, const patRoadBase*> >* ddr_arcs, double ceil) {

	/**
	 * Preprocessing, build the tree from the roots, set and roots' labels as 0.0.
	 */
	for (set<const patNode*>::iterator iter = root_nodes.begin();
			iter != root_nodes.end(); ++iter) {
		m_list_of_nodes.push_back(*iter);
		shortest_path_tree.setLabel(*iter, 0.0);
		shortest_path_tree.insertRoot(*iter);
	}

	/**
	 * Recursive process to deal with each node
	 */
	while (!m_list_of_nodes.empty()) {

		//Start from the first node in the list.
		const patNode* node_to_process = *m_list_of_nodes.begin();
		m_list_of_nodes.pop_front();

		/**
		 * If the label exceeds the distance ceiling, i.e., we have reached the distance limit that we can climb in the tree.
		 * Terminate the process.
		 */
		if (shortest_path_tree.getLabel(node_to_process) > ceil) {
			//DEBUG_MESSAGE("exceed length limit");
			break;
		}

		//Deal with each outgoing road of the node.
		const map<const patNode*, set<const patRoadBase*> >* outgoing_map =
				network->getOutgoingIncidents();

		map<const patNode*,set<const patRoadBase*> >::const_iterator find_out_going = outgoing_map->find(node_to_process);
		if(find_out_going==outgoing_map->end()){
			continue;
		}
		for (set<const patRoadBase*>::const_iterator outgoing_road_iter =
				find_out_going->second.begin();
				outgoing_road_iter
						!= find_out_going->second.end();
				++outgoing_road_iter) {

			//implement RoadType::getDownNode();
			const patNode* down_node = (*outgoing_road_iter)->getDownNode();
			const double road_cost = (*outgoing_road_iter)->getLength();
			double down_node_label = shortest_path_tree.getLabel(down_node);

			if (gps_ddr != NULL) { //If we deal with DDR
				list<const patArc*> temp_arcs =
						(*outgoing_road_iter)->getArcList(); //A road may contain several arcs
				for (list<const patArc*>::const_iterator arc_iter =
						temp_arcs.begin(); arc_iter != temp_arcs.end();
						++arc_iter) {
					if (!gps_ddr->isArcInDomain(*arc_iter,network->getTransportMode())) {
						if (gps_ddr->detArcDDR(*arc_iter,network->getTransportMode())) {
							ddr_arcs->insert(
									pair<const patArc*, const patRoadBase*>(
											*arc_iter, *outgoing_road_iter));
						}
					} else {
						ddr_arcs->insert(
								pair<const patArc*, const patRoadBase*>(
										*arc_iter, *outgoing_road_iter));
					}

				}
				if (down_node_label
						> shortest_path_tree.getLabel(node_to_process)
								+ road_cost) {

					shortest_path_tree.setLabel(
							down_node,
							shortest_path_tree.getLabel(node_to_process)
									+ road_cost);
					if (shortest_path_tree.getLabel(down_node)
							< minimum_label) {
						WARNING("NEGATIVE CYCLE DETECTED");
						return false;
					}
					shortest_path_tree.setPredecessor(down_node,
							*outgoing_road_iter);

					shortest_path_tree.insertSuccessor(node_to_process,
							*outgoing_road_iter);

					// Add the node following Bertsekas (1993)
					if (m_list_of_nodes.empty()) {
						//DEBUG_MESSAGE("add node to list"<<downNodeId);
						m_list_of_nodes.push_back(down_node);
					} else {
						double top_label = shortest_path_tree.getLabel(
								m_list_of_nodes.front());
						if (down_node_label <= top_label) {
							m_list_of_nodes.push_front(down_node);
						} else {
							m_list_of_nodes.push_back(down_node);
						}
					}
				}
			}
		}
		//DEBUG_MESSAGE(theTree.predecessor[3893]);
	}
	return true;
}
patShortestPathTreeGeneral* patShortestPathGeneral::getTree() {
	return &shortest_path_tree;
}

