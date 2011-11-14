/*
 * patNetworkBike.cc
 *
 *  Created on: Oct 28, 2011
 *      Author: jchen
 */

#include "patNetworkBike.h"

#include "patNetworkElements.h"
#include <map>
#include "patWay.h"
#include "patType.h"
#include "patDisplay.h"

void patNetworkBike::getFromNetwork(patNetworkElements* network) {
	DEBUG_MESSAGE("read bike network");
	unsigned long bike_way_count = 0;
	unsigned long not_bike_way_count = 0;

	const map<unsigned long, patWay>* all_ways = network->getWays();

	for (map<unsigned long, patWay>::const_iterator way_iter = all_ways->begin();
			way_iter != all_ways->end(); ++way_iter) {
		signed short int bike_way_indicator = way_iter->second.isBikeOneWay();
		if (bike_way_indicator == -2) {
			++not_bike_way_count;
			//DEBUG_MESSAGE("Not a car way;");

		} else if (bike_way_indicator == 0) {
			bike_way_count+=2;
			addWay(&(way_iter->second), false);
			addWay(&(way_iter->second), true);
		}

	}DEBUG_MESSAGE("bike double way: "<<bike_doubleway_count);
	DEBUG_MESSAGE("not bike way: "<<not_bike_way_count);
	return;
}



patNetworkBike::~patNetworkBike() {
	// TODO Auto-generated destructor stub
}

