/*
 * patNetworkWalk.cpp
 *
 *  Created on: Oct 28, 2011
 *      Author: jchen
 */

#include "patNetworkWalk.h"
#include "patNetworkElements.h"
#include <map>
#include "patWay.h"
#include "patType.h"
#include "patDisplay.h"
#include "patNBParameters.h"
patNetworkWalk::patNetworkWalk(){
	m_transport_mode=TransportMode(WALK);
}

void patNetworkWalk::getFromNetwork(patNetworkElements* network, patGeoBoundingBox bb) {
	DEBUG_MESSAGE("read walk network");
	unsigned long walk_way_count = 0;
	unsigned long not_walk_way_count = 0;

	const map<unsigned long, patWay>* all_ways = network->getWays();

	for (map<unsigned long, patWay>::const_iterator way_iter = all_ways->begin();
			way_iter != all_ways->end(); ++way_iter) {
		signed short int walk_way_indicator = way_iter->second.isWalkOneWay();
		if (walk_way_indicator == -2) {
			++not_walk_way_count;
			//DEBUG_MESSAGE("Not a car way;");

		} else if (walk_way_indicator == 0) {
			walk_way_count+=2;
			addWay(&(way_iter->second), false);
			addWay(&(way_iter->second), true);
		}
		else{
			WARNING("wrong indicator"<<walk_way_indicator);
		}

	}DEBUG_MESSAGE("walk double way: "<<walk_way_count);
	DEBUG_MESSAGE("not walk way: "<<not_walk_way_count);
	return;
}


patNetworkWalk::~patNetworkWalk() {
}


double patNetworkWalk::getMinSpeed() const{
	return patNBParameters::the()->walkNetworkMinSpeed;
}
double patNetworkWalk::getMaxSpeed() const{

	return patNBParameters::the()->walkNetworkMaxSpeed;
}
