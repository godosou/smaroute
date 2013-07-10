/*
 * patNetworkTrain.cc
 *
 *  Created on: Oct 11, 2011
 *      Author: jchen
 */

#include "patNetworkTrain.h"
#include "patWay.h"
#include "patType.h"
#include "patDisplay.h"
#include "patNBParameters.h"
#include <tr1/unordered_map>
#include <stack>
using namespace std::tr1;
#include <deque>

patNetworkTrain::patNetworkTrain() {

	m_transport_mode = TransportMode(TRAIN);

}

void patNetworkTrain::addWay(const patWay* the_way,
		unordered_map<const patNode*, set<const patArc*> >* arc_outgoing) const {
	const vector<const patArc*>* list_of_arcs = the_way->getArcListPointer(
			true);

	const vector<const patArc*>* list_of_arcs_reverse =
			the_way->getArcListPointer(false);
	for (vector<const patArc*>::const_iterator arc_iter = list_of_arcs->begin();
			arc_iter != list_of_arcs->end(); ++arc_iter) {
		const patArc* the_arc = (*arc_iter);
		const patNode* up_node = the_arc->getUpNode();
		unordered_map<const patNode*, set<const patArc*> >::iterator find_up_node =
				arc_outgoing->find(up_node);
		if (find_up_node == arc_outgoing->end()) {
			set<const patArc*> outgoing_arcs_set;
			outgoing_arcs_set.insert(the_arc);
			arc_outgoing->insert(
					pair<const patNode*, set<const patArc*> >(up_node,
							outgoing_arcs_set));

		} else {
			find_up_node->second.insert(the_arc);
		}
	}
	for (vector<const patArc*>::const_iterator arc_iter =
			list_of_arcs_reverse->begin();
			arc_iter != list_of_arcs_reverse->end(); ++arc_iter) {
		const patArc* the_arc = (*arc_iter);
		const patNode* up_node = the_arc->getUpNode();
		unordered_map<const patNode*, set<const patArc*> >::iterator find_up_node =
				arc_outgoing->find(up_node);
		if (find_up_node == arc_outgoing->end()) {
			set<const patArc*> outgoing_arcs_set;
			outgoing_arcs_set.insert(the_arc);
			arc_outgoing->insert(
					pair<const patNode*, set<const patArc*> >(up_node,
							outgoing_arcs_set));

		} else {
			find_up_node->second.insert(the_arc);
		}
	}

}

void patNetworkTrain::createAdjancencyList(const patNetworkElements* network,
		unordered_map<const patNode*, set<const patArc*> >* arc_outgoing) const {

//	DEBUG_MESSAGE("read train network");
	unsigned long train_way_count = 0;
	unsigned long not_train_way_count = 0;

	const map<unsigned long, patWay>* all_ways = network->getWays();

	for (map<unsigned long, patWay>::const_iterator way_iter =
			all_ways->begin(); way_iter != all_ways->end(); ++way_iter) {
		signed short int train_way_indicator = way_iter->second.isTrainOneWay();

		if (train_way_indicator == -2) {
			++not_train_way_count;
			//DEBUG_MESSAGE("Not a car way;");

		} else if (train_way_indicator == 0) {
			train_way_count += 2;
			addWay(&(way_iter->second), arc_outgoing);
		} else {
			WARNING("wrong indicator" << train_way_indicator);
		}

	}
	DEBUG_MESSAGE("train  way: " << train_way_count);
	DEBUG_MESSAGE("not train way: " << not_train_way_count);
}


/**
 * Put new connection into the network
 */
void patNetworkTrain::newConnection(patNetworkElements* network,
		patPublicTransportSegment* tmp_pt_seg) {
	patPublicTransportSegment* pt_added_pointer = network->addPTSegment(
			tmp_pt_seg);

	DEBUG_MESSAGE(
			"a new connection found: "<<tmp_pt_seg->getUpNode()->getName()<<"->"<<tmp_pt_seg->getDownNode()->getName());
	const patNode* up_station = tmp_pt_seg->getUpNode();
	m_pt_outgoing_incidents[up_station];
	m_pt_outgoing_incidents[up_station].insert(pt_added_pointer);

}

double patNetworkTrain::getLabel(
		const unordered_map<const patNode*, double>* label,
		const patNode* node) const {
	unordered_map<const patNode*, double>::const_iterator find_node_label =
			label->find(node);
	if (find_node_label == label->end()) {
		return DBL_MAX;
	} else {
		return find_node_label->second;
	}

}


/**
 * New connection between two connections.
 * Parsing the shortest path result.
 */
void patNetworkTrain::newConnectionByShortestPath(patNetworkElements* network,
		const patNode* from_station,
		const unordered_map<const patNode*, const patArc*>* predecessors,
		const patNode* to_station) {

	patPublicTransportSegment new_pt_seg;
	const patNode* current_node = to_station;
	unordered_map<const patNode*, const patArc*>::const_iterator find_pred =
			predecessors->find(current_node);

	while (true) {

		if (find_pred == predecessors->end()) {
			return;
		}
		new_pt_seg.addArcToFront(find_pred->second);
		current_node = find_pred->second->getUpNode();

		if (current_node == from_station) {
			newConnection(network, &new_pt_seg);
			break;
		}

		find_pred = predecessors->find(current_node);
	}

}

/**
 * Build connection for a station.
 * Algorithm: shortest path to reach to next station.
 */
void patNetworkTrain::buildConnectionByShortestPath(const patNode* from_station,
		const unordered_map<const patNode*, set<const patArc*> > * arc_outgoing,
		const set<const patNode*>* railway_stations,
		patNetworkElements* network) {
	unordered_map<const patNode*, double> labels;
	unordered_map<const patNode*, const patArc*> predecessors;
	deque<const patNode*> list_of_nodes;
	list_of_nodes.push_back(from_station);
	labels[from_station] = 0;
	while (!list_of_nodes.empty()) {

		//Start from the first node in the list.
		const patNode* node_to_process = list_of_nodes.front();
		list_of_nodes.pop_front();

		unordered_map<const patNode*, set<const patArc*> >::const_iterator find_out_going =
				arc_outgoing->find(node_to_process);
		if (find_out_going == arc_outgoing->end()) {
			continue;
		}
		const double label_node_to_process = getLabel(&labels, node_to_process);
		for (set<const patArc*>::const_iterator arc_iter =
				find_out_going->second.begin();
				arc_iter != find_out_going->second.end(); ++arc_iter) {

			const patNode* down_node = (*arc_iter)->getDownNode();
			const double road_cost = (*arc_iter)->getLength();
			double down_node_label = getLabel(&labels, down_node);

			if (down_node_label > label_node_to_process + road_cost) {
				labels[down_node] = label_node_to_process + road_cost;
				if (labels[down_node] < 0.00001) {
					WARNING(
							"NEGATIVE CYCLE DETECTED:"<<labels[down_node] <<","<<0.00001);
					return;
				}
				predecessors[down_node] = *arc_iter;
				if (railway_stations->find(down_node)
						!= railway_stations->end()) {

					//a new connection;
					newConnectionByShortestPath(network, from_station,
							&predecessors, down_node);

				} else {
					//extend
					if (list_of_nodes.empty()) {
						list_of_nodes.push_back(down_node);
					} else {
						double top_label = getLabel(&labels,
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
	}

}

/**
 * Create a new connection between two stations.
 * @param network: the network elements.
 * @param station: the outgoing station
 * @param tmp_pt_seg: new pt segment to the next station.
 */
void patNetworkTrain::connectStations(patNetworkElements* network,
		const patNode* station, patPublicTransportSegment* tmp_pt_seg,
		const patArc* incoming_arc,
		unordered_map<const patNode*, unsigned int>* node_count,
		const unordered_map<const patNode*, set<const patArc*> > * arc_outgoing,
		const set<const patNode*>* railway_stations) {
	const patNode* down_node = tmp_pt_seg->getDownNode();

	if (down_node != NULL
			&& railway_stations->find(down_node) != railway_stations->end()) {
		//A next station is found. Create a new connection.
		if (down_node->getName() != station->getName()) {
			newConnection(network, tmp_pt_seg);
		}
//		exit(-1);
		return;
	} else {
		//Current node is not a station, search for outgoing arcs.

		if (down_node == NULL) {

//			DEBUG_MESSAGE("station: "<<station->getName());
			down_node = station;
		}
		unordered_map<const patNode*, set<const patArc*> >::const_iterator node_outgoing_arcs =
				arc_outgoing->find(down_node);
		if (node_outgoing_arcs == arc_outgoing->end()) {
			//Invalid connection return
			DEBUG_MESSAGE(
					"node not in train network:"<< down_node->getUserId());
			return;
		} else {
//			DEBUG_MESSAGE(node_outgoing_arcs->second.size()<<" outgoing arcs from "<<down_node->getName());
			for (set<const patArc*>::const_iterator arc_iter =
					node_outgoing_arcs->second.begin();
					arc_iter != node_outgoing_arcs->second.end(); ++arc_iter) {
				//avoid u turn and loops

				const patArc* new_arc = *arc_iter;
				const patNode* new_node = new_arc->getDownNode();
				unordered_map<const patNode*, unsigned int>::iterator find_new_node =
						node_count->find(new_node);
				if (find_new_node != node_count->end()
						&& find_new_node->second > 0) {
//					DEBUG_MESSAGE("node already exists"<<find_new_node->second);
					//New node alredy exists;
					continue;
				} else {
					//valid downstream
					//insert new arc

//					DEBUG_MESSAGE("valid downstream");
					//If down arc can't be added to the segment, ignore.
					if (incoming_arc != NULL
							&& incoming_arc->getUpNode()
									== new_arc->getDownNode()
							&& new_arc->getUpNode()
									== incoming_arc->getDownNode()) {
						continue;
					}
					if (tmp_pt_seg->addArcToBack(new_arc, 3) == false) {
						continue;
					}
//					DEBUG_MESSAGE(tmp_pt_seg->size());
//					DEBUG_MESSAGE(*tmp_pt_seg);
					if (find_new_node != node_count->end()) {
						find_new_node->second += 1;
					} else {
						pair<
								unordered_map<const patNode*, unsigned int>::iterator,
								bool> insert_node = node_count->insert(
								pair<const patNode*, unsigned int>(new_node,
										1));
						find_new_node = insert_node.first;
					}
//					DEBUG_MESSAGE("pt seg has arcs:"<<tmp_pt_seg->size());
					connectStations(network, station, tmp_pt_seg, new_arc,
							node_count, arc_outgoing, railway_stations);
					//pop out the arc
					tmp_pt_seg->deleteArcFromBack();
					//pop out the node
//					DEBUG_MESSAGE(find_new_node->second);
					find_new_node->second -= 1;

//					DEBUG_MESSAGE(find_new_node->second);
				}
			}
		}
	}
}

/**
 * Create train network from adjancency list and stations.
 */
void patNetworkTrain::buildNetworkFromArcsNodes(patNetworkElements* network,
		const unordered_map<const patNode*, set<const patArc*> > * arc_outgoing,
		const set<const patNode*>* railway_stations) {
//Build a Network adjancency list from arcs and nodes.

	for (set<const patNode*>::const_iterator station_iter =
			railway_stations->begin(); station_iter != railway_stations->end();
			++station_iter) {

		buildConnectionByShortestPath(*station_iter, arc_outgoing,
				railway_stations, network);//build connection for each station


//TODO: remove indirect connections.
	}
}


/**
 * Create train network.
 * @param network: the network elements.
 * @param bounding_box: the geographical bounding box.
 */
void patNetworkTrain::getFromNetwork(patNetworkElements* network,
		patGeoBoundingBox bounding_box) {
	unordered_map<const patNode*, set<const patArc*> > arc_outgoing;
	createAdjancencyList(network, &arc_outgoing);//create adjancency list from ways
	m_stops = *network->getRailwayStations();//get railway stations
	DEBUG_MESSAGE(m_stops.size()<<" stations");
	buildNetworkFromArcsNodes(network, &arc_outgoing,
			network->getRailwayStations());//create train network from adjacency list and stations.
	summarizeMembership();//finalize
	return;
}
patNetworkTrain::~patNetworkTrain() {
}

double patNetworkTrain::getMinSpeed() const {
	return patNBParameters::the()->trainNetworkMinSpeed;
}
double patNetworkTrain::getMaxSpeed() const {

	return patNBParameters::the()->trainNetworkMaxSpeed;
}
