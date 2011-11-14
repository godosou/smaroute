/*
 * patReadPathFromShp.cc
 *
 *  Created on: Aug 15, 2011
 *      Author: jchen
 */

#include "patReadPathFromShp.h"
#include <fstream>
#include <sstream>
#include "patTransportMode.h"
#include <list>
#include "patDisplay.h"
patReadPathFromShp::patReadPathFromShp() {

}

patReadPathFromShp::~patReadPathFromShp() {
}

void patReadPathFromShp::readIndex(DBFHandle& file_handler) {
 edge_id_index = DBFGetFieldIndex(file_handler, "edge_id");
 path_id_index = DBFGetFieldIndex(file_handler, "path_id");
source_id_index = DBFGetFieldIndex(file_handler, "source");
target_id_index = DBFGetFieldIndex(file_handler, "target");
mode_index = DBFGetFieldIndex(file_handler, "mode");
	sequence_id_index = DBFGetFieldIndex(file_handler, "sequence");
}

TransportMode patReadPathFromShp::readTransportMode(patString mode_string) {
	TransportMode t_m=CAR;
	if (mode_string == "BUS") {
		t_m = BUS;
	} else if (mode_string == "CAR") {
		t_m = CAR;
	} else if (mode_string == "WALK") {
		t_m = WALK;
	} else if (mode_string=="BIKE") {
		t_m = TRAIN;
	} else if (mode_string == "BIKE") {
		t_m = BIKE;
	} else {
		WARNING("not recognized transport mode" << mode_string<<", use CAR instead.");

	}
	return t_m;

}

int patReadPathFromShp::detFirstEdgeForwardDirection(DBFHandle& file_handler) {
	if (DBFGetRecordCount(file_handler) == 1) {
		return false;
	}

	int first_source = DBFReadIntegerAttribute(file_handler, sequence_list[1],
			source_id_index);
	int first_target = DBFReadIntegerAttribute(file_handler, sequence_list[1],
			target_id_index);

	int second_source = DBFReadIntegerAttribute(file_handler, sequence_list[2],
			source_id_index);
	int second_target = DBFReadIntegerAttribute(file_handler, sequence_list[2],
			target_id_index);

	if (first_target == second_source || first_target == second_target) {
		return 1;
	} else if (first_source == second_source || first_source == second_target) {
		return -1;
	} else {
		return 0;
	}
}


void patReadPathFromShp::readSequence(DBFHandle& file_handler){
	int line_counts = DBFGetRecordCount(file_handler);

	for (int i = 0; i < line_counts; ++i) {
		int sequence =DBFReadIntegerAttribute(file_handler, i, sequence_id_index);
		sequence_list[sequence]=i;
	}
}
bool patReadPathFromShp::read(patPathJ* path, string file_path,
		patNetworkElements* network, patError*& err) {

	patPathJ new_path;
	DBFHandle file_handler = DBFOpen(file_path.c_str(), "rb");
	readIndex(file_handler); //read file indics

	//a path should contain more than one edge in order to determin the direction.
	int line_counts = DBFGetRecordCount(file_handler);
	if (line_counts == 0) {
		WARNING("empty path.");
		return false;
	} else if (line_counts == 1) {
		WARNING("only one arc, can not determin direction");
		return false;
	}
	DEBUG_MESSAGE("number of edges"<<line_counts);

	//temporal storage;
	list<bool> edges_forward_direction;
	list<TransportMode> tmp_modes;
	list<patWay*> tmp_ways;
	readSequence(file_handler);

	//Read edges and directions;

	int last_edge = DBFReadIntegerAttribute(file_handler, sequence_list[1], edge_id_index);
	int last_source = DBFReadIntegerAttribute(file_handler, sequence_list[1], source_id_index);
	int last_target = DBFReadIntegerAttribute(file_handler, sequence_list[1], target_id_index);
	string last_mode_string = DBFReadStringAttribute(file_handler, sequence_list[1],mode_index);
	TransportMode last_mode = readTransportMode(last_mode_string);

	int fist_edge_forward = detFirstEdgeForwardDirection(file_handler);
	DEBUG_MESSAGE("first link is forward? "<<fist_edge_forward);

	patWay* a_way = network->getProcessedWay(last_edge);
	if (fist_edge_forward == 1) {
		edges_forward_direction.push_back(true);
	} else if (fist_edge_forward == -1) {
		edges_forward_direction.push_back(false);
	} else {
		WARNING("Not a valid path at " << 0);
		return false;
	}
	tmp_ways.push_back(a_way);
	tmp_modes.push_back(last_mode);
	for (int i = 2; i <= line_counts; ++i) {
		int current_edge = DBFReadIntegerAttribute(file_handler, sequence_list[i],
				edge_id_index);
		int current_source = DBFReadIntegerAttribute(file_handler, sequence_list[i],
				source_id_index);
		int current_target = DBFReadIntegerAttribute(file_handler, sequence_list[i],
				target_id_index);
		string mode_string = DBFReadStringAttribute(file_handler, sequence_list[i],
				mode_index);
		patWay* a_way = network->getProcessedWay(current_edge);
		//DEBUG_MESSAGE(current_edge << "," << mode_string);
		if (a_way == NULL) {
			WARNING("wrong way id" << current_edge);
			return false;
		}
		bool forward;

		if (current_source == last_target) {
			forward=true;
		} else if (current_target == last_target) {
			forward=false;
		}else {
			WARNING("Not a valid path at " << i);
			return false;
		}
		//DEBUG_MESSAGE("forward? "<<forward)
		edges_forward_direction.push_back(forward);

		tmp_ways.push_back(a_way);

		TransportMode mode = readTransportMode(mode_string);
		tmp_modes.push_back(mode);
		if (forward){
		last_target = current_target;
		last_source = current_source;
		}
		else{

			last_target = current_source;
			last_source = current_target;
		}
	}

	//Read into the path as list of arcs and modes.

	if (edges_forward_direction.size()!=tmp_modes.size() || edges_forward_direction.size()!=tmp_ways.size()){
		WARNING("wrong in temporal storage. sizes don't match");
		return false;
	}
	int edges_size = tmp_ways.size();
	for (int i=0; i< edges_size;++i){
		patWay* way = tmp_ways.front();
		TransportMode mode = tmp_modes.front();
		bool forward = edges_forward_direction.front();
		tmp_ways.pop_front();
		tmp_modes.pop_front();
		edges_forward_direction.pop_front();

		bool reverse = !forward;
		const list<patArc*>* the_arc_list = way->getListOfArcs(reverse);
//		DEBUG_MESSAGE("way size: " << the_arc_list->size()<<", forward direction: "<<forward);
		for (list<patArc*>::const_iterator arc_iter = the_arc_list->begin();
				arc_iter != the_arc_list->end(); ++arc_iter) {
//			DEBUG_MESSAGE(**arc_iter);
			path->addArcToBack(*arc_iter,mode);
		}
	}


	return true;
}

