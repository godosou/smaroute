/*
 * patNetworkElements.cc
 *
 *  Created on: Jul 20, 2011
 *      Author: jchen
 */

#include "patNetworkElements.h"
#include "patGeoBoundingBox.h"
#include "patPostGreSQLConnector.h"
#include "patPostGISDataType.h"
#include <map>
#include "patDisplay.h"
#include "patType.h"
#include "patConst.h"
#include "patErrMiscError.h"

using namespace std;
patNetworkElements::patNetworkElements() :
		total_nbr_of_arcs(0), total_nbr_of_pt_segs(0) {
	//

}
bool patNetworkElements::addNode(unsigned long theId, patNode& theNode,
		patError*& err) {

	map<unsigned long, patNode>::iterator found = m_nodes.find(
			theNode.getUserId());
	if (found != m_nodes.end()) {
		// The node Id already exists
		DEBUG_MESSAGE("node exists");
		return false;
	} else {
		pair<map<unsigned long, patNode>::iterator, bool> insert_result =
				m_nodes.insert(
						pair<unsigned long, patNode>(theNode.userId, theNode));
		;
		//    DEBUG_MESSAGE("Node '" << theNode.name << "' added") ;
		if (insert_result.second == false) {
			err = new patErrMiscError("insert node not successful");
			WARNING(err->describe());
			return false;
		}

		return true;
	}

	return true;
}

patArc* patNetworkElements::addArc(patNode* up_node, patNode* down_node,
		patWay* the_way, patError*& err) {

	patArc new_arc(total_nbr_of_arcs, up_node, down_node);
	new_arc.m_way_id = the_way->getId();
	pair<map<unsigned long, patArc>::iterator, bool> insert_result =
			m_arcs.insert(
					pair<unsigned long, patArc>(total_nbr_of_arcs, new_arc));
	if (insert_result.second == false) {
		err = new patErrMiscError("insert arc not successful");
		WARNING(err->describe());
		return NULL;
	} else {
		//DEBUG_MESSAGE("New arc: "<<up_node_id<<" - "<<down_node_id<<"");
		patArc* new_arc_pointer = &(insert_result.first->second);
		up_node->outgoingArcs.insert(
				pair<unsigned long, patArc*>(down_node->userId,
						new_arc_pointer));
		down_node->incomingArcs.insert(
				pair<unsigned long, patArc*>(up_node->userId, new_arc_pointer));
		total_nbr_of_arcs++;
		return new_arc_pointer;
	}
}

patPublicTransportSegment* patNetworkElements::addPTSegment(patPublicTransportSegment* ptSeg,patError*& err) {

	pair<map<unsigned long, patPublicTransportSegment>::iterator, bool> insert_result =
			m_pt_segments.insert(
					pair<unsigned long, patPublicTransportSegment>(
							total_nbr_of_pt_segs, *ptSeg));
	if (insert_result.second == false) {
		err = new patErrMiscError("insert arc not successful");
		WARNING(err->describe());
		return NULL;
	} else {
		patPublicTransportSegment* new_pt_pointer = &(insert_result.first->second);

		++total_nbr_of_pt_segs;
		return new_pt_pointer;
	}
}
const map<unsigned long, patWay>* patNetworkElements::getWays() {
	return &m_ways;
}

bool patNetworkElements::addProcessedWay(patWay& the_way,
		list<unsigned long> the_list_of_nodes_ids, unsigned long source,
		unsigned long target, patError*& err) {
	if (source == target) {
		DEBUG_MESSAGE("a loop");
		return false;
	}
	if (m_processed_ways.find(the_way.getId()) != m_processed_ways.end()) {
		DEBUG_MESSAGE("duplicate m_ways" << the_way.getId());
		return true;
	}
	//DEBUG_MESSAGE(source<<"-"<<target);

	//Get effective nodes of the list node using source and target.
	//For example, the_list_of_nodes_ids=[1 2 3 4 5]
	//source = 2, target =4
	//effective_nodes = [2 3 4].
	list<unsigned long> effective_nodes;
	bool start = false;
	for (list<unsigned long>::iterator iter = the_list_of_nodes_ids.begin();
			iter != the_list_of_nodes_ids.end(); ++iter) {
		//DEBUG_MESSAGE(*iter);
		if (start) {
			effective_nodes.push_back(*iter);
			if (*iter == target) {
				break;
			}
		} else {

			if (*iter == source) {
				effective_nodes.push_back(*iter);
				start = true;
			}
		}

	}
	if (the_way.readFromNodesIds(this, effective_nodes, err)) {

		m_processed_ways[the_way.getId()] = the_way;
		return true;
	} else {
		return false;
	}

}

bool patNetworkElements::addWay(patWay* the_way,
		list<unsigned long> the_list_of_nodes_ids, patError*& err) {
	if (m_ways.find(the_way->getId()) != m_ways.end()) {
		DEBUG_MESSAGE("duplicate m_ways" << the_way->getId());
		return true;
	}
	the_way->readFromNodesIds(this, the_list_of_nodes_ids, err);

	m_ways[the_way->getId()] = *the_way;
	return true;
}

unsigned long patNetworkElements::getArcSize() const {
	return m_arcs.size();
}
unsigned long patNetworkElements::getWaySize()  const{
	return m_ways.size();
}

void patNetworkElements::readNetworkFromPostGreSQL(
		patGeoBoundingBox bounding_box, patError*& err) {
	readNodesFromPostGreSQL(bounding_box, err);
	readWaysFromPostGreSQL(bounding_box, err);
	summarizeMembership();
}
void patNetworkElements::readNodesFromPostGreSQL(patGeoBoundingBox bounding_box,
		patError*& err) {

	string bb_box;
	stringstream query_stream(bb_box);
	query_stream << "SELECT id, tags, tags -> 'name'::text AS name, ST_AsText(geom) as geom from nodes"
			<< " where geom && 'BOX3D(" << bounding_box.toString()
			<< ")'::box3d;";
	DEBUG_MESSAGE(query_stream.str());
	result R = patPostGreSQLConnector::makeSelectQuery(query_stream.str());
	DEBUG_MESSAGE("Total nodes: " << R.size());
	for (result::const_iterator i = R.begin(); i != R.end(); ++i) {

		pair<double, double> lon_lat = patPostGISDataType::PointToLonLat(
				(*i)["geom"].c_str());
		map<patString, patString> tags = patPostGISDataType::hstoreToMap((*i)["tags"].c_str());
		unsigned long id;
		(*i)["id"].to(id);
		string name;

		(*i)["name"].to(name);
		//DEBUG_MESSAGE(id<<":"<<lon_lat.first<<","<<lon_lat.second);
		patNode new_node(id, lon_lat.second, lon_lat.first);
		new_node.setTags(tags);
		new_node.setName(name);
		addNode(id, new_node, err);
	}

}

void patNetworkElements::readWaysFromPostGreSQL(patGeoBoundingBox bounding_box,
		patError*& err) {
/*
 	 //swiss_edges table has some problem that does not include all the ways (probably pt are excluded
	string bb_box;
	stringstream query_stream(bb_box);
	query_stream
			<< "select swiss_edges.id as edge_id, ways.id, source, target, nodes, ways.tags as tags from ways,swiss_edges where "
			<< " the_geom && 'BOX3D(" << bounding_box.toString() << ")'::box3d"
			<< " and ways.id= swiss_edges.osm_id;";
	DEBUG_MESSAGE(query_stream.str());

	result R = patPostGreSQLConnector::makeSelectQuery(query_stream.str());
	DEBUG_MESSAGE("Total Records: " << R.size());
	unsigned long last_way_id = 0;
	for (result::const_iterator i = R.begin(); i != R.end(); ++i) {
		unsigned long way_id;

		(*i)["id"].to(way_id);
		int edge_id;
		(*i)["edge_id"].to(edge_id);

		int source, target;
		(*i)["source"].to(source);
		(*i)["target"].to(target);

		list<unsigned long> nodes_list =
				patPostGISDataType::IntArrayToULongList((*i)["nodes"].c_str());
		map < patString, patString > tags = patPostGISDataType::hstoreToMap(
				(*i)["tags"].c_str());
		patWay new_processed_way = patWay(edge_id, tags);
		addProcessedWay(new_processed_way, nodes_list, source, target, err);
		if (way_id != last_way_id) {
			patWay new_way = patWay(way_id, tags);
			addWay(&new_way, nodes_list, err);
		}
		last_way_id = way_id;
	}
	*/

	string bb_box;
	stringstream query_stream(bb_box);
	query_stream
	<< "select ways.tags,ways.nodes,ways.id from ways "
	<< "JOIN( "
	<< "select distinct(osm_id) as osm_id from pg_ways where the_geom && 'BOX3D("
	<< bounding_box.toString() << ")'::box3d) ways_in_region"
	<< " on ways.id=ways_in_region.osm_id;";
	DEBUG_MESSAGE(query_stream.str());

	result R = patPostGreSQLConnector::makeSelectQuery(query_stream.str());
	DEBUG_MESSAGE("Total ways: " << R.size());
	unsigned long last_way_id = 0;
	for (result::const_iterator i = R.begin(); i != R.end(); ++i) {
		unsigned long way_id;

		(*i)["id"].to(way_id);

		list<unsigned long> nodes_list =
				patPostGISDataType::IntArrayToULongList((*i)["nodes"].c_str());
		map < patString, patString > tags = patPostGISDataType::hstoreToMap(
				(*i)["tags"].c_str());
		if (way_id != last_way_id) {
			patWay new_way = patWay(way_id, tags);
			addWay(&new_way, nodes_list, err);
		}
		last_way_id = way_id;
	}
	DEBUG_MESSAGE("network node size: " << getNodeSize());
	DEBUG_MESSAGE("network arc size: " << getArcSize());
	DEBUG_MESSAGE("network way size: " << getWaySize());
	DEBUG_MESSAGE("network processed way size: " << m_processed_ways.size());
}

patNetworkElements::~patNetworkElements() {
	//
}

patNode* patNetworkElements::getNode(int node_id) {

	//map<patULong, patNode> nodes;
	map<unsigned long, patNode>::iterator node_find = m_nodes.find(node_id);
	if (node_find == m_nodes.end()) {
		return NULL;
	} else {
		return &(node_find->second);
	}
}
patArc* patNetworkElements::getArc(int arc_id) {

	//map<patULong, patArc> m_arcs;
	map<unsigned long, patArc>::iterator arc_find = m_arcs.find(arc_id);
	if (arc_find == m_arcs.end()) {
		return NULL;
	} else {
		return &(arc_find->second);
	}
}

patWay* patNetworkElements::getProcessedWay(int way_id) {

	//map<patULong, patWay> m_ways;
	map<unsigned long, patWay>::iterator way_find = m_processed_ways.find(
			way_id);
	if (way_find == m_processed_ways.end()) {
		return NULL;
	} else {
		return &(way_find->second);
	}
}
patWay* patNetworkElements::getWay(int way_id) {
	//DEBUG_MESSAGE(way_id);
	//map<patULong, patWay> m_ways;
	map<unsigned long, patWay>::iterator way_find = m_ways.find(way_id);
	if (way_find == m_ways.end()) {
		//DEBUG_MESSAGE("way not found");
		return NULL;
	} else {

	//	DEBUG_MESSAGE("way  found");
		return &(way_find->second);
	}
}
unsigned long patNetworkElements::getNodeSize() const{
	return m_nodes.size();
}
const map<unsigned long, patArc>* patNetworkElements::getAllArcs() const {
	return &m_arcs;
}

void patNetworkElements::summarizeMembership() {
	for (map<unsigned long, patArc>::const_iterator arc_iter = m_arcs.begin();
			arc_iter != m_arcs.end(); ++arc_iter) {
		m_node_arc_membership[arc_iter->second.getUpNode()];
		m_node_arc_membership[arc_iter->second.getUpNode()][arc_iter->second.getDownNode()] =
				&arc_iter->second;
	}

}
void patNetworkElements::computeLength() {
	for(map<unsigned long, patArc>::iterator arc_iter = m_arcs.begin();
			arc_iter!=m_arcs.end();
			++arc_iter){
		arc_iter->second.computeLength();
	}
	for(map<unsigned long, patWay>::iterator arc_iter = m_ways.begin();
			arc_iter!=m_ways.end();
			++arc_iter){
		arc_iter->second.computeLength();
	}
	for(map<unsigned long, patWay>::iterator arc_iter = m_processed_ways.begin();
			arc_iter!=m_processed_ways.end();
			++arc_iter){
		arc_iter->second.computeLength();
	}
	for(map<unsigned long, patPublicTransportSegment>::iterator arc_iter = m_pt_segments.begin();
			arc_iter!=m_pt_segments.end();
			++arc_iter){
		arc_iter->second.computeLength();
	}
}
const patArc* patNetworkElements::findArcByNodes(const patNode* up_node,
		const patNode* down_node) const {

	map<const patNode*, map<const patNode*, const patArc*> >::const_iterator find_up_node =
			m_node_arc_membership.find(up_node);
	if (find_up_node == m_node_arc_membership.end()) {
		return NULL;
	}
	map<const patNode*, const patArc*>::const_iterator find_down_node =
			find_up_node->second.find(down_node);
	if (find_down_node == find_up_node->second.end()) {
		return NULL;
	}
	return find_down_node->second;
}
