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
patNetworkElements::patNetworkElements():
		total_nbr_of_arcs(0){
	// TODO Auto-generated constructor stub

}
patBoolean patNetworkElements::addNode(patULong theId,patNode& theNode, patError*& err){

	  map<patULong, patNode>::iterator found = nodes.find(theNode.getUserId()) ;
	  if (found != nodes.end()) {
	    // The node Id already exists
		  DEBUG_MESSAGE("node exists");
	    return patFALSE ;
	  }
	  else {
		  pair<map<patULong,patNode>::iterator, bool>  insert_result=nodes.insert(pair<patULong, patNode>(theNode.userId,theNode)) ; ;
	    //    DEBUG_MESSAGE("Node '" << theNode.name << "' added") ;
		  if(insert_result.second==false){
		  			    err = new patErrMiscError("insert node not successful") ;
		  			    WARNING(err->describe()) ;
		  			    return patFALSE ;
		  			}

	    return patTRUE ;
	  }

	  return patTRUE;
}

patArc* patNetworkElements::addArc(patNode* up_node, patNode* down_node, patWay* the_way, patError*& err){

	patArc new_arc = patArc(total_nbr_of_arcs, up_node, down_node);
	new_arc.way_id = the_way->getId();
	pair<map<patULong,patArc>::iterator, bool> insert_result = arcs.insert(pair<patULong,patArc> (total_nbr_of_arcs,new_arc));
	if(insert_result.second==false){
	    err = new patErrMiscError("insert arc not successful") ;
	    WARNING(err->describe()) ;
	    return NULL ;
	}
	else{
		//DEBUG_MESSAGE("New arc: "<<up_node_id<<" - "<<down_node_id<<"");
		patArc* new_arc_pointer = &(insert_result.first->second);
		up_node->outgoingArcs.insert(pair<patULong,patArc*>(down_node->userId,new_arc_pointer));
		down_node->incomingArcs.insert(pair<patULong,patArc*>(up_node->userId,new_arc_pointer));
		total_nbr_of_arcs++;
		return new_arc_pointer;
	}
}
const map<patULong, patWay>* patNetworkElements::getWays(){
	return &ways;
}
patBoolean patNetworkElements::addWay(patWay* the_way,list<patULong> the_list_of_nodes_ids, patError*& err){
	if(the_list_of_nodes_ids.size()<=1){
		return patTRUE;
	}
	list<patULong>::iterator node_iter=the_list_of_nodes_ids.begin();
	patULong up_node_id = *node_iter;
	patULong down_node_id;
	map<patULong, patNode>::iterator find_up_node = nodes.find(up_node_id) ;
	map<patULong, patNode>::iterator find_down_node ;
	node_iter++;
	patULong wrong_arc_counts=0;
	for(; node_iter!=the_list_of_nodes_ids.end();++node_iter){
		down_node_id = *node_iter;

		find_down_node= nodes.find(down_node_id);
		if (find_up_node==nodes.end() || find_down_node==nodes.end()){
			//WARNING("Nodes "<<up_node_id<<" or "<<down_node_id<<"not found;");
			wrong_arc_counts++;
		}
		else{
			patNode&  up_node = find_up_node->second;
			patNode& down_node = find_down_node->second;
			patArc* new_arc = addArc(&up_node,&down_node,the_way,err);

			if (new_arc!=NULL){
				the_way->appendArc(new_arc);
			}
			new_arc=addArc(&down_node,&up_node,the_way,err);
			if (new_arc!=NULL){
				the_way->appendReverseArc(new_arc);
			}
		}
		up_node_id = down_node_id;
		find_up_node= find_down_node;
	}
	ways[the_way->getId()]=*the_way;
	return patTRUE;
}

patULong patNetworkElements::getArcSize(){
	return arcs.size();
}
patULong patNetworkElements::getWaySize(){
	return ways.size();
}

void patNetworkElements::readNetworkFromPostGreSQL(patGeoBoundingBox bounding_box,
		patError*& err){
	readNodesFromPostGreSQL(bounding_box,err);
	readWaysFromPostGreSQL(bounding_box,err);

}
void patNetworkElements::readNodesFromPostGreSQL(patGeoBoundingBox bounding_box,
		patError*& err){

	string bb_box;
    stringstream query_stream(bb_box);
	query_stream<<"SELECT id, ST_AsText(geom) as geom from nodes"
				<<" where geom && 'BOX3D("
				<<bounding_box.toString()<<")'::box3d;";
	DEBUG_MESSAGE(query_stream.str());
	result R = patPostGreSQLConnector::makeSelectQuery(query_stream.str());
	DEBUG_MESSAGE("Total Records: "<<R.size());
	for (result::const_iterator
			i=R.begin();
			i!=R.end();++i){

		patPostGISDataType tt= patPostGISDataType();
		pair<patReal,patReal> lon_lat= patPostGISDataType::PointToLonLat((*i)["geom"].c_str());
		patULong id;
		(*i)["id"].to(id);
		//DEBUG_MESSAGE(id<<":"<<lon_lat.first<<","<<lon_lat.second);
		patNode new_node(id,lon_lat.second,lon_lat.first);
		addNode(id,new_node, err);
	}

}

void patNetworkElements::readWaysFromPostGreSQL(patGeoBoundingBox bounding_box,
		patError*& err){

	string bb_box;
    stringstream query_stream(bb_box);
	query_stream<<"select id, nodes, ways.tags as tags from ways,pg_ways where "
				<<" the_geom && 'BOX3D("
				<<bounding_box.toString()<<")'::box3d"
				<<	" and ways.id= pg_ways.osm_id;";
	DEBUG_MESSAGE(query_stream.str());

	result R = patPostGreSQLConnector::makeSelectQuery(query_stream.str());
	DEBUG_MESSAGE("Total Records: "<<R.size());
	for (result::const_iterator
				i=R.begin();
				i!=R.end();++i){
			patULong way_id;
			(*i)["id"].to(way_id);
			list<patULong> nodes_list= patPostGISDataType::IntArrayToULongList((*i)["nodes"].c_str());
			map<patString,patString> tags = patPostGISDataType::hstoreToMap((*i)["tags"].c_str());
			/*
			for(map<patString,patString>::iterator iter=tags.begin();
					iter!=tags.end();
					++iter){

				DEBUG_MESSAGE(iter->first<<"=>"<<iter->second);
			}
			*/
			patWay new_way = patWay(way_id,tags);
			//DEBUG_MESSAGE(tags.begin()->first);
			//DEBUG_MESSAGE(id<<":"<<lon_lat.first<<","<<lon_lat.second);
			addWay(&new_way,nodes_list,err);
		}
	DEBUG_MESSAGE("network arc size: "<<getArcSize());
	DEBUG_MESSAGE("network way size: "<<getWaySize());
}

patNetworkElements::~patNetworkElements() {
	// TODO Auto-generated destructor stub
}

patNode* patNetworkElements::getNode(patULong node_id){

	//map<patULong, patNode> nodes;
	map<patULong, patNode>::iterator node_find = nodes.find(node_id);
	if(node_find==nodes.end()){
		return NULL;
	}
	else{
		return &(node_find->second);
	}
}
patArc* patNetworkElements::getArc(patULong arc_id){

	//map<patULong, patArc> arcs;
	map<patULong, patArc>::iterator arc_find = arcs.find(arc_id);
	if(arc_find==arcs.end()){
		return NULL;
	}
	else{
		return &(arc_find->second);
	}
}
patWay* patNetworkElements::getWay(patULong way_id){

	//map<patULong, patWay> ways;
	map<patULong, patWay>::iterator way_find = ways.find(way_id);
	if(way_find==ways.end()){
		return NULL;
	}
	else{
		return &(way_find->second);
	}
}


