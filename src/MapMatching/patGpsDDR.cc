/*
 * patGpsDDR.cc
 *
 *  Created on: Oct 12, 2011
 *      Author: jchen
 */
//outside
#include <map>
#include "math.h"
#include "patDisplay.h"
//biogeme
#include "patCoordinates.h"
#include "patDisplay.h"
//same folder
#include "patGpsDDR.h"
//Object
#include "patGpsPoint.h"
//Utilities
#include "patNBParameters.h"
#include "patMultiModalPath.h"
//NetworkHandler
#include "patNode.h"
#include "patArc.h"
#include "patArcDDR.h"
#include "patErrNullPointer.h"
#include "patErrMiscError.h"
#include "patTransportMode.h"
#include <sstream>

#include "kml/dom.h"
using kmldom::KmlFactory;
using kmldom::PlacemarkPtr;
using kmldom::FolderPtr;
patGpsDDR::patGpsDDR(patGpsPoint* the_gps) :
		m_gps(the_gps), patMeasurementDDR::patMeasurementDDR() {

	m_min_point_ddr = patNBParameters::the()->maxDistanceGPSLoc;

	m_horizon_accuracy = the_gps->getHorizonAccuracy();
	m_distance_threshold_loc = m_horizon_accuracy
			* sqrt(-2 * log(m_min_point_ddr));

	DEBUG_MESSAGE(m_distance_threshold_loc);
}

patGpsDDR::~patGpsDDR() {
}

double patGpsDDR::measureDDR(const patArc* arc, TransportMode mode) {

	const patNode* upNode = arc->getUpNode();
	const patNode* downNode = arc->getDownNode();
	patCoordinates upNodeGeoCoord = upNode->getGeoCoord();
	patCoordinates downNodeGeoCoord = downNode->getGeoCoord();
	map<string, double> distance_to_arc = m_gps->distanceTo(upNodeGeoCoord,
			downNodeGeoCoord);
	struct link_ddr_range ldr = detArcDDRRange(distance_to_arc);
	return measureDDR(distance_to_arc, ldr);
}
bool patGpsDDR::detArcDDR(const patArc* arc, TransportMode mode) {
	const patNode* upNode = arc->getUpNode();
	const patNode* downNode = arc->getDownNode();
	patCoordinates upNodeGeoCoord = upNode->getGeoCoord();
	patCoordinates downNodeGeoCoord = downNode->getGeoCoord();

	map<string, double> distance_to_arc;
	double heading_accuracy_bound = patNBParameters::the()->maxHeadingGPSArc;
	double arc_heading = arc->getHeading();

	if (arc_heading < 0.0) {
		DEBUG_MESSAGE("bad arc with heading <0");
		return false;
	}

	if (m_gps->getHeading() < 0.0 || m_gps->getType() != "normal_speed"
			|| fabs(arc_heading - m_gps->getHeading()) < heading_accuracy_bound
			|| (m_gps->getHeading() < heading_accuracy_bound
					&& (360 - arc_heading + m_gps->getHeading())
							< heading_accuracy_bound
					&& (360 - arc_heading + m_gps->getHeading()) > 0.0)
			|| (m_gps->getHeading() > 360 - heading_accuracy_bound
					&& (360 - m_gps->getHeading() + arc_heading)
							< heading_accuracy_bound
					&& (360 - m_gps->getHeading() + arc_heading) > 0.0)) {
		distance_to_arc = m_gps->distanceTo(upNodeGeoCoord, downNodeGeoCoord);
		struct link_ddr_range ldr = detArcDDRRange(distance_to_arc);
//		DEBUG_MESSAGE(ldr.lower_bound<<"-"<<ldr.upper_bound);
		if (ldr.lower_bound == 0.0 && ldr.upper_bound == 0.0) {
			return false;
		} else if (ldr.upper_bound - ldr.lower_bound < 0.00001) {
			return false;
		} else {
			double measureDDRLink = measureDDR(distance_to_arc, ldr);
			//DEBUG_MESSAGE(measureDDRLink);
			if (measureDDRLink <= 0.0) {
				return false;
			}
//			DEBUG_MESSAGE(getHeading() << " " << m_gps->getHeading());
//			DEBUG_MESSAGE("in ddr"<<measureDDRLink);

			m_ddr_arcs[mode];
			m_ddr_arcs[mode].insert(
					pair<const patArc*, double>(arc, measureDDRLink));
			m_arc_ddr_ranges[mode];
			m_arc_ddr_ranges[mode].insert(
					pair<const patArc*, struct link_ddr_range>(arc, ldr));

			return true;
		}

	}
	return false;
}

struct link_ddr_range patGpsDDR::detArcDDRRange(
		map<string, double> arc_distance) const {
	struct link_ddr_range ldr;
	ldr.lower_bound = 0.0;
	ldr.upper_bound = 1.0;
	if (arc_distance["link"] > m_distance_threshold_loc) {
		ldr.upper_bound = 0.0;
	} else {

		double e_d = m_gps->calPerpendicularFootOnArc(arc_distance);
		if (e_d <= 0) {
			ldr.upper_bound = e_d
					+ sqrt(
							-2.0 * log(m_min_point_ddr)
									* pow(m_horizon_accuracy, 2)
									- pow(arc_distance["ver"], 2))
							/ arc_distance["length"];

		}
		if (e_d >= 1) {
			ldr.lower_bound = e_d
					- sqrt(
							-2.0 * log(m_min_point_ddr)
									* pow(m_horizon_accuracy, 2)
									- pow(arc_distance["ver"], 2))
							/ arc_distance["length"];

		} else {
			ldr.upper_bound = e_d
					+ sqrt(
							-2.0 * log(m_min_point_ddr)
									* pow(m_horizon_accuracy, 2)
									- pow(arc_distance["ver"], 2))
							/ arc_distance["length"];
			ldr.lower_bound = e_d
					- sqrt(
							-2.0 * log(m_min_point_ddr)
									* pow(m_horizon_accuracy, 2)
									- pow(arc_distance["ver"], 2))
							/ arc_distance["length"];

		}
		//	DEBUG_MESSAGE(1<<":"<<ldr.lower_bound<<"-"<<ldr.upper_bound);
		ldr.upper_bound = (ldr.upper_bound > 1.0) ? 1.0 : ldr.upper_bound;
		ldr.lower_bound = (ldr.lower_bound < 0.0) ? 0.0 : ldr.lower_bound;
		//	DEBUG_MESSAGE(2<<":"<<ldr.lower_bound<<"-"<<ldr.upper_bound);
	}

	return ldr;

}

double patGpsDDR::measureDDR(map<string, double> distance_to_arc
		, link_ddr_range ldr) const {
	struct arc_ddr_params param;
	param.d = distance_to_arc["ver"];
	param.l = distance_to_arc["length"];
	param.e_d = m_gps->calPerpendicularFootOnArc(distance_to_arc);
	param.std_x = m_horizon_accuracy;
	param.ldr = ldr;
	//	DEBUG_MESSAGE("d"<<param.d<<",l"<<param.l<<",e_d"<<param.e_d<<",deltax"<<param.std_x);
	//DEBUG_MESSAGE(param.ldr.lower_bound<<" "<<param.ldr.upper_bound);
	patArcDDR theArcDDR(m_horizon_accuracy);
	double ddr_v = 0.0;

	try {
		ddr_v = theArcDDR.errCDF(param);
	}
	catch (...) {
		DEBUG_MESSAGE(ldr.lower_bound<<"-"<<ldr.upper_bound);
	}
//DEBUG_MESSAGE(param.l<<" "<<ddr_v);
	return ddr_v;
}
/*
 void patGpsDDR::genDDRFromPaths(const set<patMultiModalPath>* path_set) {
 set<const patArc*> excluded_arcs; //Arcs that are already excluded from domain. This is a cache to speed up the detection.

 for (set<patMultiModalPath>::const_iterator path_iter = path_set->begin();
 path_iter != path_set->end(); ++path_iter) {

 list<const patArc*> arc_list = (*path_iter).getArcList();
 for (list<const patArc*>::const_iterator arc_iter = arc_list.begin();
 arc_iter != arc_list.end(); ++arc_iter) {
 const patArc* the_arc = *arc_iter;

 if (excluded_arcs.find(the_arc) != excluded_arcs.end()) {
 //If the arc is already excluded, skip it.
 continue;
 }
 bool is_in_ddr = detArcDDR(the_arc);

 if (is_in_ddr == false) {
 excluded_arcs.insert(the_arc);
 }
 }
 }

 }
 */
