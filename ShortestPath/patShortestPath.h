/*
 * patShortestPath.cc
 *
 *  Created on: Oct 11, 2011
 *      Author: jchen
 */

#include "patShortestPath.h"
#include "patShortestPathTree.h"

#include "patNode.h"

#include <deque>
class patShortestPath {
public:
	patShortestPath(patNetworkBase* a_network, double the_min_label) :
			network(a_network), minimum_label(the_min_label) {

	}

	bool computeShortestPathTree(patNode* root_node, double ceil) {
		set<patNode*> root_nodes;
		root_nodes.insert(root_node);
		computeShortestPathTree(root_nodes, ceil);

	}
	// Return patFALSE if a negative cycle has been detected. patTRUE otherwise.
	bool computeShortestPathTree(set<patNode*> root_nodes, patGpsDDR* gps_ddr, set<patArc*> ddr_arcs,
			double ceil) {

		for (set<patNode*>::iterator iter = root_nodes.begin();
				iter != root_nodes.end(); ++iter) {
			list_of_nodes.push_back(*iter);
			shortest_path_tree.labels[*iter] = 0.0;
			shortest_path_tree.root.insert(*iter);
		}

		while (!list_of_nodes.empty()) {

			patNode* node_to_process = *list_of_nodes.begin();

			list_of_nodes.pop_front();
			if (shortest_path_tree.labels[node_to_process] > ceil) {
				//DEBUG_MESSAGE("exceed length limit");
				break;
			}

			const map<patNode*, set<patRoadBase*> >* outgoing_map =
					network->getOutgoingIncidents();

			for (set<patRoadBase*>::const_iterator outgoing_road_iter =
					outgoing_map->find(node_to_process)->second.begin();
					outgoing_road_iter
							!= outgoing_map->find(node_to_process)->second.end();
					++outgoing_road_iter) {

				//implement RoadType::getDownNode();
				const patNode* down_node = (*outgoing_road_iter)->getDownNode();
				const double road_cost = (*outgoing_road_iter)->getLength();
				double down_node_label = shortest_path_tree.lables[down_node];

				if (gps_ddr != NULL) {
					//TODO implement patGpsDDR::isArcInDDRSet
					//TODO implement patArc::getAllArcs()
					//TODO implement patPublicTransportSegment::getAllArcs()

					set<patArc*> temp_arcs =
							(*outgoing_road_iter)->getAllArcs();
					for (set<patArc*>::iterator arc_iter = temp_arcs.begin();
							arc_iter != temp_arcs.end(); ++arc_iter) {
						if (!gps_ddr->isArcInDDRSet(incomingArc)) {
							if (gps_ddr->detArcDDR(*arc_iter)) {
								ddr_arcs->insert(*arc_iter);
							}
						} else {
							ddr_arcs->insert(*arc_iter)
						}

					}
					if (down_node_label
							> shortest_path_tree.labels[node_to_process]
									+ road_cost) {

						shortest_path_tree.labels[down_node] =
								shortest_path_tree.labels[node_to_process]
										+ road_cost;
						if (shortest_path_tree.labels[shortest_path_tree]
								< minimum_label) {
							WARNING("NEGATIVE CYCLE DETECTED");
							return patFALSE;
						}
						shortest_path_tree.predecessor[down_node] =
								*outgoing_road_iter;
						if (shortest_path_tree.successor.find(node_to_process)
								== shortest_path_tree.successor.end()) {

							shortest_path_tree.successor[node_to_process] = set<
									patRoadBase*>();
						}
						shortest_path_tree.successor[node_to_process].insert(
								*outgoing_road_iter);

						// Add the node following Bertsekas (1993)
						if (list_of_nodes.empty()) {
							//DEBUG_MESSAGE("add node to list"<<downNodeId);
							list_of_nodes.push_back(down_node);
						} else {
							double top_label =
									shortest_path_tree.labels[list_of_nodes.begin()];
							if (down_node_label <= top_label) {
								list_of_nodes.push_front(down_node);
							} else {
								list_of_nodes.push_back(down_node);
							}
						}
					}
				}
			}
			//DEBUG_MESSAGE(theTree.predecessor[3893]);
			return true;
		}

		patShortestPathTree getTree() {
			return shortest_path_tree;
		}
		virtual ~patShortestPath();
	protected:
		patNetworkBase* network;
		double minimum_label;
		patShortestPathTree shortest_path_tree;
		deque<patNode*> list_of_nodes;
	}
	;

