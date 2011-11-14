/*
 * patPublicTransportSegment.cc
 *
 *  Created on: Jul 22, 2011
 *      Author: jchen
 */

#include "patPublicTransportSegment.h"

patPublicTransportSegment::patPublicTransportSegment():
error(false){
	// TODO Auto-generated constructor stub

}

patPublicTransportSegment::~patPublicTransportSegment() {
	// TODO Auto-generated destructor stub
}


/**
 * Append way_id way to the end;
 * @param way_id the id of the way
 */
bool patPublicTransportSegment::pushBack(patNetworkElements* network, patULong way_id){
	patWay* the_way = network.getWay(way_id);
	if (the_way==NULL){
		return false;
	}
	else{
		ways.push_back(the_way);
		return the_way;
	}
}

/**
 * Append way_id way to the end;
 * @param way_id the id of the way
 */
bool patPublicTransportSegment::pushFront(patNetworkElements* network,patULong way_id){
	patWay* the_way = network->getWay(way_id);
		if (the_way==NULL){
			return false;
		}
		else{
			ways.push_front(the_way);
			return the_way;
		}
}
bool patPublicTransportSegment::isError(){
	return error;
}
