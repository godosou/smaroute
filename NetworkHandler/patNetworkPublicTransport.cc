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
	double min_distance = patMaxReal;
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
map<const patNode*, int> patNetworkPublicTransport::findStops(
		map<const patNode*, int>& stops
		,map<const patNode*, map<const patArc*, int> >& incidents) {
	map<const patNode*, int> stops_on_route;
	set<const patNode*> nodes_on_route;
	for (map<const patNode*, map<const patArc*, int> >::iterator node_iter =
			incidents.begin(); node_iter != incidents.end(); ++node_iter) {
		nodes_on_route.insert(nodes_on_route.end(), node_iter->first);
	}
	for (map<const patNode*, int>::const_iterator stop_iter = stops.begin();
			stop_iter != stops.end(); ++stop_iter) {
		const patNode* nearest_node = findNearestNode(stop_iter->first,
				nodes_on_route);
		const_cast<patNode*>(nearest_node)->setName(
				stop_iter->first->getName());

		map<string, string> tags = stop_iter->first->getTags();
		const_cast<patNode*>(nearest_node)->setTags(tags);
		stops_on_route.insert(stops_on_route.end(),
				pair<const patNode*, int>(nearest_node, stop_iter->second));
	}

	return stops_on_route;
}
void patNetworkPublicTransport::getRoute(patNetworkElements* network_elements,
		vector<pair<unsigned long, string> >& way_ids
		, vector<pair<unsigned long, string> >& node_ids,
		map<patString, patString>& tags) {
	if(tags["ref"]!="m1"){
		return;
	}
	DEBUG_MESSAGE("a route");
	DEBUG_MESSAGE("stops:");
	patString file_path = tags["ref"];
	string kml_file_path = file_path + ".kml";
	ofstream kml_file(kml_file_path.c_str());
	patCreateKmlStyles doc;
	DocumentPtr document = doc.createStylesForKml();

	KmlFactory* factory = KmlFactory::GetFactory();
	FolderPtr stop_folder = factory->CreateFolder();
	FolderPtr links_folder = factory->CreateFolder();

	stop_folder->set_name(string("Stop"));
	links_folder->set_name(string("Links"));

	map<const patNode*, int> stops;
	for (vector<pair<unsigned long, string> >::iterator node_iter =
			node_ids.begin(); node_iter != node_ids.end(); ++node_iter) {
		const patNode* node = network_elements->getNode(node_iter->first);
		if (node == NULL) {
			WARNING("no node" << node_iter->first);
		} else {
			int direction;
			DEBUG_MESSAGE(
					node_iter->second<<":"<< node->getUserId() << " " << node->getName() << " " << node<<" " <<node->getTagString());
			stop_folder->add_feature(node->getKML());

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

	DEBUG_MESSAGE("links:");
	map<const patNode*, map<const patArc*, int> > incidents; //int is the direction.
	for (vector<pair<unsigned long, string> >::iterator way_iter =
			way_ids.begin(); way_iter != way_ids.end(); ++way_iter) {
		//DEBUG_MESSAGE(way_iter->second);
		patWay* way = network_elements->getWay(way_iter->first);
		if (way == NULL) {
			//WARNING("no way" << way);
		} else {
			int direction;
			if (way_iter->second.find("forward") != string::npos) {
				direction = 1;
			} else if (way_iter->second.find("backward") != string::npos) {
				direction = 2;
			} else {
				direction = 3;
			}
			//DEBUG_MESSAGE(direction);
			if (direction == 1 or direction == 3) {
				//DEBUG_MESSAGE("forward");
				const list<const patArc*>* arc_list = way->getArcListPointer(
						true);
				for (list<const patArc*>::const_iterator arc_iter =
						arc_list->begin(); arc_iter != arc_list->end();
						++arc_iter) {
					//DEBUG_MESSAGE(**arc_iter);
					DEBUG_MESSAGE(
							(*arc_iter)->getUpNode()->getName() << " " << (*arc_iter)->getUpNode()->getUserId() << " " << (*arc_iter)->getUpNode()->getTagString());
					DEBUG_MESSAGE(
							(*arc_iter)->getDownNode()->getName() << " " << (*arc_iter)->getDownNode()->getUserId() << " " << (*arc_iter)->getUpNode()->getTagString());
					const patNode* up_node = (*arc_iter)->getUpNode();
					links_folder->add_feature(((*arc_iter)->getKML("bus")));
					;
					incidents[up_node];
					incidents[up_node][*arc_iter] = direction;
					//DEBUG_MESSAGE(up_node.size());
				}
			}
			if (direction == 2 or direction == 3) {
				//DEBUG_MESSAGE("backward");
				const list<const patArc*>* arc_list = way->getArcListPointer(
						false);
				for (list<const patArc*>::const_iterator arc_iter =
						arc_list->begin(); arc_iter != arc_list->end();
						++arc_iter) {
					//DEBUG_MESSAGE(**arc_iter);
					DEBUG_MESSAGE(
							(*arc_iter)->getUpNode()->getName() << " " << (*arc_iter)->getUpNode()->getUserId() << " " << (*arc_iter)->getUpNode()->getTagString());
					DEBUG_MESSAGE(
							(*arc_iter)->getDownNode()->getName() << " " << (*arc_iter)->getDownNode()->getUserId() << " " << (*arc_iter)->getUpNode()->getTagString());
					const patNode* up_node = (*arc_iter)->getUpNode();

					links_folder->add_feature(((*arc_iter)->getKML("bus")));
					;
					incidents[up_node];
					incidents[up_node][*arc_iter] = direction;
					//DEBUG_MESSAGE( incidents[up_node].size());
				}
			}
		}
	} //DEBUG_MESSAGE("stops: "<<stops.size());
	  //DEBUG_MESSAGE("nodes: "<<incidents.size());
	document->add_feature(stop_folder);
	document->add_feature(links_folder);
	KmlPtr kml = factory->CreateKml();
	kml->set_feature(document);

	kml_file << kmldom::SerializePretty(kml);
	kml_file.close();
	DEBUG_MESSAGE(kml_file_path << " written");

	stops = findStops(stops, incidents);
	map<const patNode*, map<const patNode*, patPublicTransportSegment> > lines;
	for (map<const patNode*, int>::iterator stop_iter = stops.begin();
			stop_iter != stops.end(); ++stop_iter) {
		const patNode* up_node = stop_iter->first;
		//DEBUG_MESSAGE("from up node"<<*up_node);
		patPublicTransportSegment new_seg;
		m_outgoing_incidents[up_node];
//		DEBUG_MESSAGE("OK");
		getSegFromNode(network_elements, incidents, up_node, new_seg, stops);
		//DEBUG_MESSAGE(
		//	up_node->getName()<<" "<<m_outgoing_incidents[up_node].size());
	}

	DEBUG_MESSAGE("a route end");
	return;
}
void patNetworkPublicTransport::getSegFromNode(
		patNetworkElements* network_elements,
		map<const patNode*, map<const patArc*, int> >& incidents,
		const patNode* up_node,
		patPublicTransportSegment& seg,
		map<const patNode*, int>& stops) {

	if (up_node == NULL) {
		return;
	}
	map<const patNode*, map<const patArc*, int> >::iterator find_up_node =
			incidents.find(up_node);
	if (find_up_node == incidents.end() || find_up_node->second.empty()) {
		//DEBUG_MESSAGE("reach the end"<<up_node->getUserId());
		return;
	} else {
		for (map<const patArc*, int>::iterator arc_iter =
				find_up_node->second.begin();
				arc_iter != find_up_node->second.end(); ++arc_iter) {
			const patNode* down_node = arc_iter->first->getDownNode();
			if (down_node == NULL) {
				continue;
			}
			if (seg.isNodeInPath(down_node)) {
				continue;
			} else {

				//DEBUG_MESSAGE("down"<<*(*arc_iter));
				if (seg.addArcToBack(arc_iter->first, 3) == false) {
					//DEBUG_MESSAGE("wrong append");
					continue;
				}
				if(stops.find(down_node)!=stops.en){

				}
				if (stops.find(down_node) != stops.end()
						&& (stops[down_node] == stops[seg.getUpNode()]
								|| (stops[down_node] == 3
										|| stops[seg.getUpNode()] == 3))) { //TODO direction check
					///Condition 1: the node is in stop.
					///and
					///Condition 2: the directions are not in conflict
					///Condition 2.1: directions are the same
					///or
					///Condition 2.2: directions are 3 (both directions)

					DEBUG_MESSAGE(seg);
					patError* err = NULL;
					m_outgoing_incidents[seg.front()->getUpNode()];

					if (err == NULL) {
						patPublicTransportSegment* inserted =
								network_elements->addPTSegment(&seg, err);
						m_outgoing_incidents[seg.front()->getUpNode()].insert(
								inserted);
					}
				} else {
					//DEBUG_MESSAGE("OK"<<seg.size());
					//	DEBUG_MESSAGE(down_node->getUserId()<<" "<<seg.size());
					getSegFromNode(network_elements, incidents, down_node, seg,
							stops);
				}
				seg.deleteArcFromBack();
			}
		}

	}
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
	string query_string =
			"select " + table_name + ".* " + " from " + table_name
					+ " inner JOIN( "
					+ "(select distinct(osm_id) as osm_id from pg_ways where the_geom && 'BOX3D("
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
	map<patString, patString> tags;
	for (result::const_iterator i = R.begin(); i != R.end(); ++i) {

		//get new information
		string route_type;
		unsigned long current_id;
		unsigned long osm_id;
		patString member_type;
		patString member_role;
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
			m_outgoing_incidents.begin(); iter_1 != m_outgoing_incidents.end();
			++iter_1) {
		for (set<const patRoadBase*>::iterator iter_2 = iter_1->second.begin();
				iter_2 != iter_1->second.end(); ++iter_2) {
			list<const patArc*> arc_list = (*iter_2)->getArcList();
			for (list<const patArc*>::const_iterator arc_iter =
					arc_list.begin(); arc_iter != arc_list.end(); ++arc_iter) {
				m_pt_arc_membership[*arc_iter];
				m_pt_arc_membership[*arc_iter].insert(*iter_2);
			}
		}
	}

}
set<const patRoadBase*> patNetworkPublicTransport::getRoadsContainArc(
		const patRoadBase* arc) const {
	map<const patRoadBase*, set<const patRoadBase*> >::const_iterator find_arc =
			m_pt_arc_membership.find(arc);
	if (find_arc == m_pt_arc_membership.end()) {
		return set<const patRoadBase*>();
	} else {
		return find_arc->second;
	}

}

