/*
 * patPublicTransportSegment.cc
 *
 *  Created on: Jul 22, 2011
 *      Author: jchen
 */

#include "patPublicTransportSegment.h"

patPublicTransportSegment::patPublicTransportSegment():
error(false),
length(-1.0){
	//

}

patPublicTransportSegment::~patPublicTransportSegment() {
	//
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

double patPublicTransportSegment::getLength(){
	if(length<0){
		return calLength();
	}
}

double patPublicTransportSegment::calLength(){
	length=0.0;
	list<patWay*> ways;
	for(list<patWay*>::iterator way_iter = ways.begin();
			way_iter!=ways.end();
			++way_iter){
		length += (*way_iter)->getLength();
	}
	return length;
}
