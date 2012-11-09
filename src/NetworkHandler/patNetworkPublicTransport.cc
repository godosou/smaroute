/*
 * patNetworkPublicTransport.cc
 *
 *  Created on: Jul 22, 2011
 *      Author: jchen
 */

#include "patNetworkPublicTransport.h"

#include "patPostGreSQLConnector.h"
#include "patPostGISDataType.h"
#include "patDisplay.h"
#include "patPublicTransportSegment.h"
#include "patNode.h"
#include "kml/dom.h"
#include <sstream>
using kmldom::DocumentPtr;
using kmldom::KmlFactory;
using kmldom::KmlPtr;
using kmldom::PlacemarkPtr;
using kmldom::FolderPtr;

using kmldom::StylePtr;
using kmldom::LineStylePtr;
#include "patCreateKmlStyles.h"
patNetworkPublicTransport::patNetworkPublicTransport() {

}

const patNode* patNetworkPublicTransport::findNearestNode(const patNode* node,
		set<const patNode*> nodes_set) {
	double min_distance = DBL_MAX;
	const patNode* nearest_node;
	for (set<const patNode*>::const_iterator node_iter = nodes_set.begin();
			node_iter != nodes_set.end(); ++node_iter) {
		double distance = node->getGeoCoord().distanceTo(
				(*node_iter)->getGeoCoord());
		if (distance < min_distance) {
			nearest_node = *node_iter;
			min_distance = distance;
		}
	}
	return nearest_node;
}
map<string, map<const patArc*, const patNode*> > patNetworkPublicTransport::findStopsIncomingLinks(
		map<const patNode*, int>& stops,
		map<const patNode*, list<const patArc*> >& outgoing_incidents,
		map<const patNode*, list<const patArc*> >& incoming_incidents,
		set<const patArc*>& forward_arcs,
		set<const patArc*>& backward_arcs) {

	map<string, map<const patArc*, const patNode*> > stops_incoming_links;
	for (map<const patNode*, int>::iterator stop_iter = stops.begin();
			stop_iter != stops.end(); ++stop_iter) {
		string stop_name = (stop_iter->first)->getName();
		map<const patNode*, list<const patArc*> >::iterator find_incoming =
				incoming_incidents.find(stop_iter->first);
		bool find_tag = false;
		if (find_incoming != incoming_incidents.end()) {
			stops_incoming_links[stop_name];
//			DEBUG_MESSAGE(stop_name << stop_iter->second);

			for (list<const patArc*>::iterator arc_iter =
					find_incoming->second.begin();
					arc_iter != find_incoming->second.end(); ++arc_iter) {
				/*
				 stops_incoming_links[stop_name][*arc_iter] = stop_iter->first;
				 find_tag = true;
				 */
				if (stop_iter->second == 1 or stop_iter->second == 3) {
					if (forward_arcs.find(*arc_iter) != forward_arcs.end()) {
//						DEBUG_MESSAGE("found one forward");
						stops_incoming_links[stop_name][*arc_iter] =
								stop_iter->first;
						find_tag = true;
					}
				}
				if (stop_iter->second == 2 or stop_iter->second == 3) {
					if (backward_arcs.find(*arc_iter) != backward_arcs.end()) {

//						DEBUG_MESSAGE("found one backward");
						stops_incoming_links[stop_name][*arc_iter] =
								stop_iter->first;
						find_tag = true;
					}
				}
			}

		}
		if (find_tag == false) {

			map<const patNode*, list<const patArc*> >::iterator find_outgoing =
					outgoing_incidents.find(stop_iter->first);
			if (find_outgoing == outgoing_incidents.end()
					or find_tag == false) {
//				DEBUG_MESSAGE("no stop on route" << *(stop_iter->first));
				double min_distance = DBL_MAX;
				const patArc* best_arc;
//				if (stop_iter->second == 1 or stop_iter->second == 3) {
				for (set<const patArc*>::iterator arc_iter =
						forward_arcs.begin(); arc_iter != forward_arcs.end();
						++arc_iter) {
					map<string, double> distance = (*arc_iter)->distanceTo(
							stop_iter->first);
					if (distance["right"] == 1
							&& distance["down"] < min_distance) {
						best_arc = *arc_iter;
						min_distance = distance["down"];
					}
				}

//				}
				//			if (stop_iter->second == 2 or stop_iter->second == 3) {
				for (set<const patArc*>::iterator arc_iter =
						backward_arcs.begin(); arc_iter != backward_arcs.end();
						++arc_iter) {
					if (*arc_iter != NULL) {
						map<string, double> distance = (*arc_iter)->distanceTo(
								stop_iter->first);
						if (distance["right"] == 1
								&& distance["down"] < min_distance) {
							best_arc = *arc_iter;
							min_distance = distance["down"];
						}

					} else {
						WARNING("wrong arc");
					}
					//			}

				}
				if (min_distance < 500.0) {
//					DEBUG_MESSAGE("nearest link distance" << min_distance);

					const_cast<patNode*>(best_arc->getDownNode())->setName(
							stop_name);

					stops_incoming_links[stop_name];
					stops_incoming_links[stop_name][best_arc] =
							stop_iter->first;
				}

			}
		}
	}
	return stops_incoming_links;

}

map<const patNode*, int> patNetworkPublicTransport::findStops(
		map<const patNode*, int>& stops
		,map<const patNode*, map<const patArc*, int> >& incidents) {
	map<const patNode*, int> stops_on_route;
	set<const patNode*> nodes_on_route;
	for (map<const patNode*, map<const patArc*, int> >::iterator node_iter =
			incidents.begin(); node_iter != incidents.end(); ++node_iter) {
		nodes_on_route.insert(nodes_on_route.end(), node_iter->first);
	}DEBUG_MESSAGE("original stops: " << stops.size());
	for (map<const patNode*, int>::const_iterator stop_iter = stops.begin();
			stop_iter != stops.end(); ++stop_iter) {
		const patNode* nearest_node = findNearestNode(stop_iter->first,
				nodes_on_route);
		if (nearest_node != stop_iter->first) {
			DEBUG_MESSAGE("original node" << *stop_iter->first);
			DEBUG_MESSAGE("changed node" << *nearest_node);
			const_cast<patNode*>(nearest_node)->setName(
					stop_iter->first->getName());

			unordered_map<string, string> tags = stop_iter->first->getTags();
			const_cast<patNode*>(nearest_node)->setTags(tags);
		}
		int direction;
		//DEBUG_MESSAGE("nearest node"<<*nearest_node);
		if (stops_on_route.find(nearest_node) != stops_on_route.end()) {
			stops_on_route[nearest_node] = 3;
			direction = 3;
		} else {
			direction = stop_iter->second;
			stops_on_route.insert(stops_on_route.end(),
					pair<const patNode*, int>(nearest_node, stop_iter->second));
		}
		stringstream ss; //create a stringstream
		ss << direction; //add number to the stream

		const_cast<patNode*>(nearest_node)->setTag("stop_direction", ss.str());
		//DEBUG_MESSAGE("stops on route"<<stops_on_route.size());
	}

	return stops_on_route;
}
map<const patNode*, int> patNetworkPublicTransport::getStops(
		patNetworkElements* network_elements,
		vector<pair<unsigned long, string> >& node_ids) {
	map<const patNode*, int> stops;
	for (vector<pair<unsigned long, string> >::iterator node_iter =
			node_ids.begin(); node_iter != node_ids.end(); ++node_iter) {
		const patNode* node = network_elements->getNode(node_iter->first);
		if (node == NULL) {
			WARNING("no node" << node_iter->first);
		} else {
			//stops.insert(node);

			int direction;
//			DEBUG_MESSAGE(
//					node_iter->second << ":" << node->getUserId() << " " << node->getName() << " " << node << " " << node->getTagString());

			if (node_iter->second.find("forward") != string::npos) {
				direction = 1;
			} else if (node_iter->second.find("backward") != string::npos) {
				direction = 2;
			} else {
				direction = 3;
			}
			if (stops.find(node) != stops.end()) {
				stops[node] = 3;
			} else {
				stops.insert(stops.end(),
						pair<const patNode*, int>(node, direction));
			}

		}
	}
	return stops;
}
int patNetworkPublicTransport::getDirection(string direction_string) {
	int direction;
	if (direction_string.find("forward") != string::npos) {
		direction = 1;
	} else if (direction_string.find("backward") != string::npos) {
		direction = 2;
	} else {
		direction = 3;
	}
	return direction;
}

map<const patNode*, map<const patNode*, patArcSequence*> > patNetworkPublicTransport::getRouteWaysOutgoingIncidents(
		list<patArcSequence>& ways, bool outgoing) {
	map<const patNode*, map<const patNode*, patArcSequence*> > incidents;
	for (list<patArcSequence>::iterator way_iter = ways.begin();
			way_iter != ways.end(); ++way_iter) {
		const patNode* up_node = (*way_iter).getUpNode();
		const patNode* down_node = (*way_iter).getDownNode();
		if (outgoing) {
			incidents[up_node];
			incidents[up_node][down_node] = &(*way_iter);
		} else {

			incidents[down_node];
			incidents[down_node][up_node] = &(*way_iter);
		}

	}
	return incidents;

}

list<patArcSequence> patNetworkPublicTransport::getRouteWays(
		patNetworkElements* network_elements,
		vector<pair<unsigned long, string> >& way_ids) {
	list<patArcSequence> incidents; //int is the direction.

	for (vector<pair<unsigned long, string> >::iterator way_iter =
			way_ids.begin(); way_iter != way_ids.end(); ++way_iter) {
		patWay* way = network_elements->getWay(way_iter->first);
		if (way != NULL) {
			int direction = getDirection(way_iter->second);
			if (direction == 1 or direction == 3) {
				//DEBUG_MESSAGE("forward");

				patArcSequence arc_seq;
				const vector<const patArc*>* arc_list = way->getArcListPointer(
						true);
				arc_seq.addArcsToBack(arc_list);
				incidents.push_back(arc_seq);
			}
			if (direction == 2 or direction == 3) {

				patArcSequence arc_seq;
				const vector<const patArc*>* arc_list = way->getArcListPointer(
						false);
				arc_seq.addArcsToBack(arc_list);
				incidents.push_back(arc_seq);
			}

		}
	}
	return incidents;
}
void patNetworkPublicTransport::getDownStream(
		map<const patNode*, map<const patNode*, patArcSequence*> >& incidents,
		list<patArcSequence*>& line , const patNode* up_up_node
		,const patNode* up_node, const patNode* origin, bool forward) {

	if (incidents.find(up_node) != incidents.end()) {
		for (map<const patNode*, patArcSequence*>::iterator down_stream_iter =
				incidents[up_node].begin();
				down_stream_iter != incidents[up_node].end();
				++down_stream_iter) {
			int i = 0;
			DEBUG_MESSAGE(
					origin->getUserId() << ":" << up_node->getUserId() << "-" << down_stream_iter->first->getUserId());
			if (down_stream_iter->first == up_up_node) {

				continue;
			}

			else {
				i += 1;
				if (forward) {
					line.push_back(down_stream_iter->second);
				} else {
					line.push_front(down_stream_iter->second);
				}
				if (down_stream_iter->first == origin) {
					DEBUG_MESSAGE("reach origin");
					return;
				}
				getDownStream(incidents, line, up_node, down_stream_iter->first,
						origin, forward);
				DEBUG_MESSAGE(line.size());
//				line.pop_back();
			}
		}
	} else {
		DEBUG_MESSAGE("no down stream");
	}
}
/*
 void patNetworkPublicTransport::getRoute(patNetworkElements* network_elements,
 vector<pair<unsigned long, string> >& way_ids
 , vector<pair<unsigned long, string> >& node_ids,
 map<string, string>& tags) {

 if (tags["ref"] != "m1") {
 return;
 }DEBUG_MESSAGE("ways: "<<way_ids.size());
 map<const patNode*, int> stops = getStops(network_elements, node_ids);

 list<patArcSequence> ways = getRouteWays(network_elements, way_ids);
 DEBUG_MESSAGE("ways"<<ways.size());
 map<const patNode*, map<const patNode*, patArcSequence*> > ways_outgoing_incidents =
 getRouteWaysOutgoingIncidents(ways, true);
 map<const patNode*, map<const patNode*, patArcSequence*> > ways_incoming_incidents =
 getRouteWaysOutgoingIncidents(ways, false);
 list<patArcSequence*> line;
 for(list<patArcSequence>::iterator way_iter= ways.begin();way_iter!=ways.end();++way_iter){
 line.clear();
 line.push_back(&(*way_iter));
 const patNode* up_node = line.front()->getUpNode();
 const patNode* down_node = line.front()->getDownNode();
 DEBUG_MESSAGE(*up_node);
 DEBUG_MESSAGE(*down_node);
 getDownStream(ways_outgoing_incidents, line, up_node, down_node, up_node,
 true);
 DEBUG_MESSAGE("change direction");
 getDownStream(ways_incoming_incidents, line, down_node, up_node, down_node,
 false);

 if(line.size()>= ways.size()/1.5){
 break;
 }
 }
 string file_path = tags["ref"];
 string kml_file_path = file_path + ".kml";
 ofstream kml_file(kml_file_path.c_str());
 patCreateKmlStyles doc;
 DocumentPtr document = doc.createStylesForKml();

 KmlFactory* factory = KmlFactory::GetFactory();
 FolderPtr connected_ways = factory->CreateFolder();
 connected_ways->set_name("connected");
 connected_ways->set_styleurl("bus");

 FolderPtr original_ways = factory->CreateFolder();
 original_ways->set_name("original");
 original_ways->set_styleurl("bus");
 for (list<patArcSequence*>::iterator seq_iter = line.begin();
 seq_iter != line.end(); ++seq_iter) {
 connected_ways->add_feature((*seq_iter)->getKML());
 }
 for (list<patArcSequence>::iterator seq_iter = ways.begin();
 seq_iter != ways.end(); ++seq_iter) {
 original_ways->add_feature((*seq_iter).getKML());
 }

 document->add_feature(connected_ways);
 document->add_feature(original_ways);
 KmlPtr kml = factory->CreateKml();
 kml->set_feature(document);

 kml_file << kmldom::SerializePretty(kml);
 kml_file.close();
 }
 */

void patNetworkPublicTransport::getRoute(patNetworkElements* network_elements,
		vector<pair<unsigned long, string> >& way_ids
		, vector<pair<unsigned long, string> >& node_ids,
		unordered_map<string, string>& tags) {
	if (tags["ref"] != "m1") {
		//return;
	}
	map<const patNode*, int> stops = getStops(network_elements, node_ids);
//	DEBUG_MESSAGE("links:");
	map<const patNode*, list<const patArc*> > outgoing_incidents;
	map<const patNode*, list<const patArc*> > incoming_incidents; //int is the direction.
	set<const patArc*> forward_arcs;
	set<const patArc*> backward_arcs;

	for (vector<pair<unsigned long, string> >::iterator way_iter =
			way_ids.begin(); way_iter != way_ids.end(); ++way_iter) {
		patWay* way = network_elements->getWay(way_iter->first);
		if (way == NULL) {
			//WARNING("no way" << way);
		} else {
			int direction = getDirection(way_iter->second);
			if (direction == 1 or direction == 3) {
				const vector<const patArc*>* arc_list = way->getArcListPointer(
						true);
				for (vector<const patArc*>::const_iterator arc_iter =
						arc_list->begin(); arc_iter != arc_list->end();
						++arc_iter) {
					if (*arc_iter != NULL) {
						const patNode* up_node = (*arc_iter)->getUpNode();
						const patNode* down_node = (*arc_iter)->getDownNode();
						outgoing_incidents[up_node];
						outgoing_incidents[up_node].push_back(*arc_iter);
						incoming_incidents[down_node];
						incoming_incidents[down_node].push_back(*arc_iter);
						forward_arcs.insert(*arc_iter);
					} else {
						WARNING("NULL ARC");
					}
				}
			}
			if (direction == 2 or direction == 3) {
				const vector<const patArc*>* arc_list = way->getArcListPointer(
						false);
				for (vector<const patArc*>::const_iterator arc_iter =
						arc_list->begin(); arc_iter != arc_list->end();
						++arc_iter) {
					if (*arc_iter != NULL) {
						const patNode* up_node = (*arc_iter)->getUpNode();
						const patNode* down_node = (*arc_iter)->getDownNode();
						outgoing_incidents[up_node];
						outgoing_incidents[up_node].push_back(*arc_iter);
						incoming_incidents[down_node];
						incoming_incidents[down_node].push_back(*arc_iter);
						backward_arcs.insert(*arc_iter);
					} else {
						WARNING("NULL ARC");
					}
				}
			}
		}
	}

	map<string, map<const patArc*, const patNode*> > stops_incoming =
			findStopsIncomingLinks(stops, outgoing_incidents,
					incoming_incidents, forward_arcs, backward_arcs);
	list<const patNode*> true_stops;

	//TODO use the sequence information about the stops
//	DEBUG_MESSAGE("stops: " << stops_incoming.size());

	map<const patNode*, map<const patNode*, patPublicTransportSegment> > lines;

	string file_path = tags["ref"];
	string kml_file_path = file_path + ".kml";
	ofstream kml_file(kml_file_path.c_str());
	patCreateKmlStyles doc;
	DocumentPtr document = doc.createStylesForKml();

	KmlFactory* factory = KmlFactory::GetFactory();
	FolderPtr stops_folder = factory->CreateFolder();
	FolderPtr segs_folder = factory->CreateFolder();

	map<pair<string, string> , set<patPublicTransportSegment> > pt_incidents;
	for (map<string, map<const patArc*, const patNode*> >::iterator stop_iter =
			stops_incoming.begin(); stop_iter != stops_incoming.end();
			++stop_iter) {
		for (map<const patArc*, const patNode*>::iterator arc_iter =
				stop_iter->second.begin(); arc_iter != stop_iter->second.end();
				++arc_iter) {
			patNode* true_stop =
					const_cast<patNode*>(arc_iter->first->getDownNode());true_stops.push_back(true_stop);
					const patNode* original_stop = arc_iter->second;
					FolderPtr incoming_arc_kml = arc_iter->first->getKML("bus");
					incoming_arc_kml->set_name(original_stop->getName());
					stops_folder->add_feature(incoming_arc_kml);

					PlacemarkPtr node = original_stop->getKML();
					node->set_styleurl("#stop");
					incoming_arc_kml->add_feature(node);
					list<string> stop_names;
					stop_names.push_back(true_stop->getName());
					//DEBUG_MESSAGE("from up node"<<*up_node);
					patPublicTransportSegment new_seg;
					m_pt_outgoing_incidents[true_stop];
					list<map<const patArc*, const patNode*> > arcs_to_stop;
					arcs_to_stop.push_back(stops_incoming[true_stop->getName()]);
					getSegFromNode(network_elements, outgoing_incidents, true_stop, arc_iter->first,new_seg,
							stops_incoming,arcs_to_stop, stop_names,pt_incidents);

				}
			}
//	DEBUG_MESSAGE("segs:" << pt_incidents.size());

	for (map<pair<string, string> , set<patPublicTransportSegment> >::iterator pt_iter =
			pt_incidents.begin(); pt_iter != pt_incidents.end(); ++pt_iter) {
		if (pt_iter->second.size() > 1) {

//			DEBUG_MESSAGE("confusion" << pt_iter->second.size());

		}
		for (set<patPublicTransportSegment>::iterator seg_iter =
				pt_iter->second.begin(); seg_iter != pt_iter->second.end();
				++seg_iter) {
			patPublicTransportSegment new_pt = *seg_iter;
//			DEBUG_MESSAGE(
//					pt_iter->first.first << "->" << pt_iter->first.second);
			const patNode* up_node = new_pt.getUpNode();
			m_pt_outgoing_incidents[up_node];

			patPublicTransportSegment* pt_added_pointer =
					network_elements->addPTSegment(&new_pt);
				m_pt_outgoing_incidents[up_node].insert(pt_added_pointer);
				FolderPtr seg_folder = factory->CreateFolder();
				vector<const patArc*> arc_list = pt_added_pointer->getArcList();
				seg_folder->set_name(
						pt_iter->first.first + "->" + pt_iter->first.second);
				for (vector<const patArc*>::iterator arc_iter = arc_list.begin();
						arc_iter != arc_list.end(); ++arc_iter) {

					vector<PlacemarkPtr> arc_pts = (*arc_iter)->getArcKML("bus");

					for (vector<PlacemarkPtr>::const_iterator pt_iter = arc_pts.begin();
							pt_iter != arc_pts.end(); ++pt_iter) {
						seg_folder->add_feature(*pt_iter);
					}
				}
				segs_folder->add_feature(seg_folder);
		}
	}
	document->add_feature(stops_folder);
	document->add_feature(segs_folder);
	KmlPtr kml = factory->CreateKml();
	kml->set_feature(document);

	kml_file << kmldom::SerializePretty(kml);
	kml_file.close();
	DEBUG_MESSAGE("bus route"<<kml_file_path << " written");

//	DEBUG_MESSAGE("a route end");
	return;
}

void patNetworkPublicTransport::getSegFromNode(
		patNetworkElements* network_elements,
		map<const patNode*, list<const patArc*> >& incidents
		,
		const patNode* up_node
		,
		const patArc* incoming_arc
		,
		patPublicTransportSegment& seg
		,
		map<string, map<const patArc*, const patNode*> >& stops
		,
		list<map<const patArc*, const patNode*> >& arcs_to_stop
		,
		list<string>& stop_names
		,
		map<pair<string, string> , set<patPublicTransportSegment> >& pt_incidents) {

	if (up_node == NULL) {
		return;
	}
	map<const patNode*, list<const patArc*> >::iterator find_up_node =
			incidents.find(up_node);
	if (find_up_node == incidents.end() || find_up_node->second.empty()) {
		//no outgoing links
		//DEBUG_MESSAGE("reach the end"<<up_node->getUserId());
		return;
	} else {
		for (list<const patArc*>::iterator arc_iter =
				find_up_node->second.begin();
				arc_iter != find_up_node->second.end(); ++arc_iter) {
			const patArc* down_arc = *arc_iter;
			const patNode* down_node = down_arc->getDownNode();
			if (down_node == NULL || down_arc == NULL) {
				continue;
			}
			if (seg.isNodeInPath(down_node)) {
				//If there is a loop
				continue;
			} else {
				if (incoming_arc->getUpNode() == down_arc->getDownNode()
						&& down_arc->getUpNode() == incoming_arc->getDownNode()) {
					continue;
				}
				if (seg.addArcToBack(down_arc, 3) == false) {
					continue;
				}
				const patNode *up_node = seg.getUpNode();
				string up_name = up_node->getName();
				string down_name = down_node->getName();

				pair<string, string> stop_pair(up_name, down_name);
//				DEBUG_MESSAGE(stop_pair.first<<"->"<<stop_pair.second);
				if (stops.find(down_name) != stops.end()
						and stops[down_name].find(down_arc)
								!= stops[down_name].end()) {
					stop_names.push_back(down_name);
					set<string> unique_stops;
					unique_stops.insert(stop_names.begin(), stop_names.end());
					if (!findArcInForbidenList(arcs_to_stop, down_arc)
							and unique_stops.size() == 2) {
						seg.computeLength();
						pt_incidents[stop_pair];
						pt_incidents[stop_pair].insert(seg);
						arcs_to_stop.push_back(stops[down_name]);
						getSegFromNode(network_elements, incidents, down_node,
								down_arc, seg, stops, arcs_to_stop, stop_names,
								pt_incidents);
						arcs_to_stop.pop_back();
					} else {
						//				DEBUG_MESSAGE(unique_stops.size());
					}
					stop_names.pop_back();

				} else {
					getSegFromNode(network_elements, incidents, down_node,
							down_arc, seg, stops, arcs_to_stop, stop_names,
							pt_incidents);
				}
				seg.deleteArcFromBack();
			}
		}

	}
}

bool patNetworkPublicTransport::findArcInForbidenList(
		list<map<const patArc*, const patNode*> >& arcs_to_stop
		,const patArc* arc) {
	for (list<map<const patArc*, const patNode*> >::iterator list_iter =
			arcs_to_stop.begin(); list_iter != arcs_to_stop.end();
			++list_iter) {
		if ((*list_iter).find(arc) != (*list_iter).end()) {
			return true;
		}
	}
	return false;
}
void patNetworkPublicTransport::getRoutes(patNetworkElements* network_elements,
		string table_name, patGeoBoundingBox bounding_box) {
	/*
	 string query_string ="select "+table_name+".*, "+table_name+".tags -> 'ref'::text AS ref, "
	 +table_name+".tags -> 'network'::text AS network"
	 +"	from "+table_name
	 +" inner JOIN("
	 +"select distinct(osm_id) as osm_id from pg_ways where the_geom && 'BOX3D("
	 +bounding_box.toString()+")'::box3d) ways_in_region"
	 +" on "+table_name+".member_id=ways_in_region.osm_id"
	 +" order by id, sequence_id;";
	 */
	string query_string;
	/*
	 query_string =
	 "select " + table_name + ".* " + " from " + table_name
	 + " inner JOIN( "
	 + "(select distinct(osm_id) as osm_id from pg_ways where the_geom && 'BOX3D("
	 + bounding_box.toString() + ")'::box3d) " + "union all"
	 + "(select distinct(id) as osm_id from nodes where geom && 'BOX3D("
	 + bounding_box.toString() + ")'::box3d) "
	 + ") all_in_region " + "on " + table_name
	 + ".member_id=all_in_region.osm_id "
	 + " order by id, sequence_id";
	 */
	query_string =
			"select " + table_name + ".* " + " from " + table_name
					+ " inner JOIN( "
					+ "(select distinct(id) as osm_id from ways where geom && 'BOX3D("
					+ bounding_box.toString() + ")'::box3d) " + "union all"
					+ "(select distinct(id) as osm_id from nodes where geom && 'BOX3D("
					+ bounding_box.toString() + ")'::box3d) "
					+ ") all_in_region " + "on " + table_name
					+ ".member_id=all_in_region.osm_id "
					+ " order by id, sequence_id";
	DEBUG_MESSAGE(query_string);
	result R = patPostGreSQLConnector::makeSelectQuery(query_string);

	DEBUG_MESSAGE("Total Records: " << R.size());

//parse result

//initial set up
	unsigned long last_id = -1;

	vector<pair<unsigned long, string> > way_ids;
	vector<pair<unsigned long, string> > node_ids;
	string prev_route_type;
	unordered_map<string, string> tags;
	for (result::const_iterator i = R.begin(); i != R.end(); ++i) {

		//get new information
		string route_type;
		unsigned long current_id;
		unsigned long osm_id;
		string member_type;
		string member_role;
		int short current_sequence_id;
		(*i)["route_type"].to(route_type);
		(*i)["id"].to(current_id);
		(*i)["member_id"].to(osm_id);
		(*i)["member_type"].to(member_type); //W,N,R
		(*i)["sequence_id"].to(current_sequence_id);
		(*i)["member_role"].to(member_role);
		if (last_id != -1 && current_id != last_id) {
			//if(prev_route_type=="subway");
			getRoute(network_elements, way_ids, node_ids, tags);
			way_ids.clear();
			node_ids.clear();
		}
		tags = patPostGISDataType::hstoreToMap((*i)["tags"].c_str());
		prev_route_type = route_type;
		//if it is a way
		if (member_type == "W") {
			way_ids.push_back(pair<unsigned long, string>(osm_id, member_role));

		}
		//If it is a node;
		else if (member_type == "N") {
			node_ids.push_back(
					pair<unsigned long, string>(osm_id, member_role));
		} else {
			WARNING("not valid relation member" << member_type);
		}
		last_id = current_id;
	}

	getRoute(network_elements, way_ids, node_ids, tags);
	DEBUG_MESSAGE("FINISH");
	summarizeMembership();
}
patNetworkPublicTransport::~patNetworkPublicTransport() {
//
}

void patNetworkPublicTransport::summarizeMembership() {
	for (map<const patNode*, set<const patRoadBase*> >::iterator iter_1 =
			m_pt_outgoing_incidents.begin();
			iter_1 != m_pt_outgoing_incidents.end(); ++iter_1) {
		if (!iter_1->second.empty()) {
			m_stops.insert(iter_1->first);

		}
		for (set<const patRoadBase*>::iterator iter_2 = iter_1->second.begin();
				iter_2 != iter_1->second.end(); ++iter_2) {
			m_stops.insert((*iter_2)->getDownNode());
			vector<const patArc*> arc_list = (*iter_2)->getArcList();
			for (vector<const patArc*>::const_iterator arc_iter =
					arc_list.begin(); arc_iter != arc_list.end(); ++arc_iter) {
				m_pt_arc_membership[*arc_iter];
				m_pt_arc_membership[*arc_iter].insert(*iter_2);
				m_outgoing_incidents[(*arc_iter)->getUpNode()];
				m_outgoing_incidents[(*arc_iter)->getUpNode()].insert(
						*arc_iter);
			}
		}
	}

}
set<const patRoadBase*> patNetworkPublicTransport::getRoadsContainArc(
		const patRoadBase* arc) const {

	set<const patRoadBase*> roads;
	unordered_map<const patNode*, set<const patRoadBase*> >::const_iterator find_up_node =
			m_outgoing_incidents.find(arc->getUpNode());

	if (find_up_node == m_outgoing_incidents.end()) {
		return roads;
	}
	set<const patRoadBase*>::const_iterator find_arc =
			find_up_node->second.find(arc);
	if (find_arc == find_up_node->second.end()) {
		return roads;
	} else {
		roads.insert(*find_arc);
	}
	return roads;
}

set<const patRoadBase*> patNetworkPublicTransport::getPTSegsContainArc(
		const patRoadBase* arc) const {

	map<const patRoadBase*, set<const patRoadBase*> >::const_iterator find_arc =
			m_pt_arc_membership.find(arc);
	if (find_arc == m_pt_arc_membership.end()) {
		return set<const patRoadBase*>();
	} else {
		return find_arc->second;
	}
}

bool patNetworkPublicTransport::isStop(const patNode* node) const {
	if (m_stops.find(node) == m_stops.end()) {
		return false;
	} else {
		//DEBUG_MESSAGE(node->getName());
		return true;
	}
}
/*
 set<const patNode*> patNetworkPublicTransport::getStops() const {
 set<const patNode*> stops;
 for (map<const patNode*, set<const patRoadBase*> >::const_iterator stop_iter =
 m_pt_outgoing_incidents.begin();
 stop_iter != m_pt_outgoing_incidents.end(); ++stop_iter) {
 stops.insert(stop_iter->first);
 for (set<const patRoadBase*>::const_iterator segs_iter =
 stop_iter->second.begin(); segs_iter != stop_iter->second.end();
 ++segs_iter) {
 stops.insert((*segs_iter)->getDownNode());
 }
 }
 return stops;
 }
 */
void patNetworkPublicTransport::walkFromToStops(
		patNetworkElements* network_elements, patNetworkBase* walk_network) {
	DEBUG_MESSAGE(
			"Buil walk connection " << getTransportModeString(getTransportMode()));
	for (set<const patNode*>::const_iterator stop_iter = m_stops.begin();
			stop_iter != m_stops.end(); ++stop_iter) {
		set<const patNode*> nearby_nodes = network_elements->getNearbyNode(
				(*stop_iter)->getGeoCoord(), 100.0, 5);
		//DEBUG_MESSAGE("Nearby nodes for stop "<<(*stop_iter)->getName()<<": "<<nearby_nodes.size());
		for (set<const patNode*>::const_iterator near_iter =
				nearby_nodes.begin(); near_iter != nearby_nodes.end();
				++near_iter) {
			const patArc* found_arc = network_elements->findArcByNodes(
					*stop_iter, *near_iter);
			if (found_arc == NULL) {
				found_arc = network_elements->addArc(*stop_iter, *near_iter,
						NULL);
				if (found_arc == NULL) {
					continue;
				}
			}
			walk_network->addArc(found_arc);

			const patArc* reverse_arc = network_elements->findArcByNodes(
					*stop_iter, *near_iter);
			if (reverse_arc == NULL) {
				reverse_arc = network_elements->addArc(*near_iter, *stop_iter,
						NULL);
				if (reverse_arc == NULL) {
					continue;
				}
			}
			walk_network->addArc(reverse_arc);

		}

	}
}

void patNetworkPublicTransport::walkOnTrack(
		patNetworkElements* network_elements, patNetworkBase* walk_network) const{
	DEBUG_MESSAGE(
			"Buil walk access " << getTransportModeString(getTransportMode()));


	for(unordered_map<const patNode*, set<const patRoadBase*> >::const_iterator node_iter = m_outgoing_incidents.begin();
				node_iter!=m_outgoing_incidents.end();
				++node_iter){
		for(set<const patRoadBase*>::const_iterator road_iter = node_iter->second.begin();
				road_iter!=node_iter->second.end();
				++road_iter){
			vector<const patArc*> arcs = (*road_iter)->getArcList();
			for(vector<const patArc*>::const_iterator arc_iter = arcs.begin();
				arc_iter!=arcs.end();
				++arc_iter){

				walk_network->addArc(*arc_iter);
			}
		}
	}
}
