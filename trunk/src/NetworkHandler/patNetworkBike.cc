/*
 * patNetworkBike.cc
 *
 *  Created on: Oct 28, 2011
 *      Author: jchen
 */

#include "patNetworkBike.h"

#include "patNBParameters.h"
#include "patNetworkElements.h"
#include <map>
#include "patWay.h"
#include "patType.h"
#include "patDisplay.h"

patNetworkBike::patNetworkBike(){
	m_transport_mode=TransportMode(BIKE);
}
void patNetworkBike::getFromNetwork(patNetworkElements* network, patGeoBoundingBox bb) {
	DEBUG_MESSAGE("read bike network");
	unsigned long bike_way_count = 0;
	unsigned long not_bike_way_count = 0;

	const map<unsigned long, patWay>* all_ways = network->getWays();

	for (map<unsigned long, patWay>::const_iterator way_iter =
			all_ways->begin(); way_iter != all_ways->end(); ++way_iter) {
		signed short int bike_way_indicator = way_iter->second.isBikeOneWay();
		if (bike_way_indicator == -2) {
			++not_bike_way_count;
			//DEBUG_MESSAGE("Not a car way;");

		} else if (bike_way_indicator == 0) {
			bike_way_count += 2;
			addWay(&(way_iter->second), false);
			addWay(&(way_iter->second), true);
		} else {
			WARNING("wrong indicator" << bike_way_indicator);
		}

	}
	DEBUG_MESSAGE("bike  way: " << bike_way_count * 2);
	DEBUG_MESSAGE("not bike way: " << not_bike_way_count);
	return;
}

patNetworkBike::~patNetworkBike() {

}

double patNetworkBike::getMinSpeed() const{
	return patNBParameters::the()->bikeNetworkMinSpeed;
}
double patNetworkBike::getMaxSpeed() const{

	return patNBParameters::the()->bikeNetworkMaxSpeed;
}
