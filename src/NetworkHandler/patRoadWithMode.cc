/*
 * patRoadWithMode.cc
 *
 *  Created on: Feb 29, 2012
 *      Author: jchen
 */

#include "patRoadWithMode.h"

patRoadWithMode::patRoadWithMode() {


}

patRoadWithMode::~patRoadWithMode() {
}

bool operator==(const patRoadWithMode& a_road, const patRoadWithMode& b_road) {
	if (a_road.m_road == b_road.m_road && a_road.mode == b_road.m_road
			&& a_road.stop_time == b_road.stop_time) {
		return true;
	} else {
		return false;
	}
}
bool operator<(const RoadTravel& a_road, const RoadTravel& b_road) {
	if (a_road.road < b_road.road) {
		return true;
	}
	if (a_road.road > b_road.road) {
		return false;
	}
	if (a_road.mode < b_road.mode) {
		return true;
	}
	if (a_road.mode > b_road.mode) {
		return false;
	}
	if (a_road.stop_time < b_road.stop_time) {
		return true;
	}
	if (a_road.stop_time > b_road.stop_time) {
		return false;
	}
	return false;
}
