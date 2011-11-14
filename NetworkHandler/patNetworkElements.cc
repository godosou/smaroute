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
		total_nbr_of_arcs(0) {
	//

}
patBoolean patNetworkElements::addNode(patULong theId, patNode& theNode,
		patError*& err) {

	map<patULong, patNode>::iterator found = nodes.find(theNode.getUserId());
	if (found != nodes.end()) {
		// The node Id already exists
		DEBUG_MESSAGE("node exists");
		return patFALSE;
	} else {
		pair<map<patULong, patNode>::iterator, bool> insert_result =
				nodes.insert(pair<patULong, patNode>(theNode.userId, theNode));
		;
		//    DEBUG_MESSAGE("Node '" << theNode.name << "' added") ;
		if (insert_result.second == false) {
			err = new patErrMiscError("insert node not successful");
			WARNING(err->describe());
			return patFALSE;
		}

		return patTRUE;
	}

	return patTRUE;
}

patArc* patNetworkElements::addArc(patNode* up_node, patNode* down_node,
		patWay* the_way, patError*& err) {

	patArc new_arc = patArc(total_nbr_of_arcs, up_node, down_node);
	new_arc.way_id = the_way->getId();
	pair<map<patULong, patArc>::iterator, bool> insert_result = arcs.insert(
			pair<patULong, patArc>(total_nbr_of_arcs, new_arc));
	if (insert_result.second == false) {
		err = new patErrMiscError("insert arc not successful");
		WARNING(err->describe());
		return NULL;
	} else {
		//DEBUG_MESSAGE("New arc: "<<up_node_id<<" - "<<down_node_id<<"");
		patArc* new_arc_pointer = &(insert_result.first->second);
		up_node->outgoingArcs.insert(
				pair<patULong, patArc*>(down_node->userId, new_arc_pointer));
		down_node->incomingArcs.insert(
				pair<patULong, patArc*>(up_node->userId, new_arc_pointer));
		total_nbr_of_arcs++;
		return new_arc_pointer;
	}
}
const map<patULong, patWay>* patNetworkElements::getWays() {
	return &ways;
}

patBoolean patNetworkElements::addProcessedWay(patWay& the_way,
		list<patULong> the_list_of_nodes_ids, patULong source, patULong target,
		patError*& err) {
	if (source == target) {
		DEBUG_MESSAGE("a loop");
		return patFALSE;
	}
	if (processed_ways.find(the_way.getId()) != processed_ways.end()) {
		DEBUG_MESSAGE("duplicate ways"<<the_way.getId());
		return patTRUE;
	}
	//DEBUG_MESSAGE(source<<"-"<<target);

	//Get effective nodes of the list node using source and target.
	//For example, the_list_of_nodes_ids=[1 2 3 4 5]
	//source = 2, target =4
	//effective_nodes = [2 3 4].
	list<patULong> effective_nodes;
	bool start = false;
	for (list<patULong>::iterator iter = the_list_of_nodes_ids.begin();
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
	if(the_way.readFromNodesIds(this, effective_nodes, err)){

		processed_ways[the_way.getId()] = the_way;
		return patTRUE;
	}
	else{
		return patFALSE;
	}

}

patBoolean patNetworkElements::addWay(patWay* the_way,
		list<patULong> the_list_of_nodes_ids, patError*& err) {
	if (ways.find(the_way->getId()) != ways.end()) {
		DEBUG_MESSAGE("duplicate ways"<<the_way->getId());
		return patTRUE;
	}
	the_way->readFromNodesIds(this, the_list_of_nodes_ids, err);

	ways[the_way->getId()] = *the_way;
	return patTRUE;
}

patULong patNetworkElements::getArcSize() {
	return arcs.size();
}
patULong patNetworkElements::getWaySize() {
	return ways.size();
}

void patNetworkElements::readNetworkFromPostGreSQL(
		patGeoBoundingBox bounding_box, patError*& err) {
	readNodesFromPostGreSQL(bounding_box, err);
	readWaysFromPostGreSQL(bounding_box, err);

}
void patNetworkElements::readNodesFromPostGreSQL(patGeoBoundingBox bounding_box,
		patError*& err) {

	string bb_box;
	stringstream query_stream(bb_box);
	query_stream << "SELECT id, ST_AsText(geom) as geom from nodes"
			<< " where geom && 'BOX3D(" << bounding_box.toString()
			<< ")'::box3d;";
	DEBUG_MESSAGE(query_stream.str());
	result R = patPostGreSQLConnector::makeSelectQuery(query_stream.str());
	DEBUG_MESSAGE("Total Records: "<<R.size());
	for (result::const_iterator i = R.begin(); i != R.end(); ++i) {

		patPostGISDataType tt = patPostGISDataType();
		pair<patReal, patReal> lon_lat = patPostGISDataType::PointToLonLat(
				(*i)["geom"].c_str());
		patULong id;
		(*i)["id"].to(id);
		//DEBUG_MESSAGE(id<<":"<<lon_lat.first<<","<<lon_lat.second);
		patNode new_node(id, lon_lat.second, lon_lat.first);
		addNode(id, new_node, err);
	}

}

void patNetworkElements::readWaysFromPostGreSQL(patGeoBoundingBox bounding_box,
		patError*& err) {

	string bb_box;
	stringstream query_stream(bb_box);
	query_stream
			<< "select swiss_edges.id as edge_id, ways.id, source, target, nodes, ways.tags as tags from ways,swiss_edges where "
			<< " the_geom && 'BOX3D(" << bounding_box.toString() << ")'::box3d"
			<< " and ways.id= swiss_edges.osm_id;";
	DEBUG_MESSAGE(query_stream.str());

	result R = patPostGreSQLConnector::makeSelectQuery(query_stream.str());
	DEBUG_MESSAGE("Total Records: "<<R.size());
	patULong last_way_id = 0;
	for (result::const_iterator i = R.begin(); i != R.end(); ++i) {
		patULong way_id;

		(*i)["id"].to(way_id);
		int edge_id;
		(*i)["edge_id"].to(edge_id);

		int source, target;
		(*i)["source"].to(source);
		(*i)["target"].to(target);

		list<patULong> nodes_list = patPostGISDataType::IntArrayToULongList(
				(*i)["nodes"].c_str());
		map<patString, patString> tags = patPostGISDataType::hstoreToMap(
				(*i)["tags"].c_str());
		/*
		 for(map<patString,patString>::iterator iter=tags.begin();
		 iter!=tags.end();
		 ++iter){

		 DEBUG_MESSAGE(iter->first<<"=>"<<iter->second);
		 }
*/
		patWay new_processed_way = patWay(edge_id, tags);
		addProcessedWay(new_processed_way, nodes_list, source, target, err);
		if (way_id != last_way_id) {
			patWay new_way = patWay(way_id, tags);
			addWay(&new_way, nodes_list, err);
		}
		last_way_id = way_id;
	}
	DEBUG_MESSAGE("network node size: "<<getNodeSize());
	DEBUG_MESSAGE("network arc size: "<<getArcSize());
		DEBUG_MESSAGE("network way size: "<<getWaySize());
	DEBUG_MESSAGE("network processed way size: "<<processed_ways.size());
}

patNetworkElements::~patNetworkElements() {
	//
}

patNode* patNetworkElements::getNode(int node_id) {

	//map<patULong, patNode> nodes;
	map<patULong, patNode>::iterator node_find = nodes.find(node_id);
	if (node_find == nodes.end()) {
		return NULL;
	} else {
		return &(node_find->second);
	}
}
patArc* patNetworkElements::getArc(int arc_id) {

	//map<patULong, patArc> arcs;
	map<patULong, patArc>::iterator arc_find = arcs.find(arc_id);
	if (arc_find == arcs.end()) {
		return NULL;
	} else {
		return &(arc_find->second);
	}
}

patWay* patNetworkElements::getProcessedWay(int way_id) {

	//map<patULong, patWay> ways;
	map<patULong, patWay>::iterator way_find = processed_ways.find(way_id);
	if (way_find == processed_ways.end()) {
		return NULL;
	} else {
		return &(way_find->second);
	}
}patWay* patNetworkElements::getWay(int way_id) {

	//map<patULong, patWay> ways;
	map<patULong, patWay>::iterator way_find = ways.find(way_id);
	if (way_find == ways.end()) {
		return NULL;
	} else {
		return &(way_find->second);
	}
}
patULong patNetworkElements::getNodeSize(){
	return nodes.size();
}
const map<patULong, patArc>* patNetworkElements::getAllArcs(){
	return &arcs
}
