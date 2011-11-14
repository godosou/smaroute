/*
 * patNetworkCar.cpp
 *
 *  Created on: Jul 21, 2011
 *      Author: jchen
 */

#include "patNetworkCar.h"
#include "patNetworkElements.h"
#include <map>
#include "patWay.h"
#include "patType.h"
#include "patDisplay.h"

void patNetworkCar::getFromNetwork(patNetworkElements* network) {
	DEBUG_MESSAGE("read car network");
	patULong car_way_count = 0;
	patULong not_car_way_count = 0;
	patULong car_oneway_count = 0;
	patULong car_reverse_way_count = 0;
	patULong car_doubleway_count = 0;


	const map<patULong, patWay>* all_ways = network->getWays();

	for (map<patULong, patWay>::const_iterator way_iter = all_ways->begin();
			way_iter != all_ways->end(); ++way_iter) {
		signed short int car_way_indicator = way_iter->second.isCarOneWay();
		if (car_way_indicator == -2) {
			++not_car_way_count;
			//DEBUG_MESSAGE("Not a car way;");
		} else if (car_way_indicator == -1) {
			++car_reverse_way_count;
			addWay(&(way_iter->second), true);
		} else if (car_way_indicator == 0) {
			++car_doubleway_count;
			addWay(&(way_iter->second), false);
			addWay(&(way_iter->second), true);
		} else if (car_way_indicator ==1) {
			++car_oneway_count;
			addWay(&(way_iter->second), true);

		}

	}DEBUG_MESSAGE("car double way: "<<car_doubleway_count);
	DEBUG_MESSAGE("car one way: "<<car_oneway_count);
	DEBUG_MESSAGE("car reverse way: "<<car_reverse_way_count);
	DEBUG_MESSAGE(
			"car way count: "<<car_doubleway_count*2 +car_oneway_count+ car_reverse_way_count);
	DEBUG_MESSAGE("not car way: "<<not_car_way_count);
	return;
}

patNetworkCar::~patNetworkCar() {

}
