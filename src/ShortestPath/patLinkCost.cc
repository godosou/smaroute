/*
 * patLinkCost.cc
 *
 *  Created on: Apr 20, 2012
 *      Author: jchen
 */

#include "patLinkCost.h"

patLinkCost::patLinkCost() {
	// TODO Auto-generated constructor stub

}

patLinkCost::~patLinkCost() {
	// TODO Auto-generated destructor stub
}

 double patLinkCost::getCost(const patRoadBase* road) const {
	vector<const patArc*> arc_list = road->getArcList();
	double cost = 0.0;
	for (vector<const patArc*>::const_iterator arc_iter = arc_list.begin();
			arc_iter != arc_list.end(); ++arc_iter) {
		cost += getCost(*arc_iter);
	}
	return cost;
}
// double patLinkCost::getCost(const patRoadBase* road,
//		const TransportMode& mode) const {
//
//	vector<const patArc*> arc_list = road->getArcList();
//	double cost = 0.0;
//	for (vector<const patArc*>::const_iterator arc_iter = arc_list.begin();
//			arc_iter != arc_list.end(); ++arc_iter) {
//		cost += getCost(*arc_iter, mode);
//	}
//	return cost;
//}
