/*
 * patGpsDDR.cc
 *
 *  Created on: Oct 12, 2011
 *      Author: jchen
 */

#include "patGpsDDR.h"
#include "patGeoCoordinates.h"
#include "patNBParameters.h"
#include "math.h"
#include "patNode.h"
#include "patArc.h"
#include <map>

patGpsDDR::patGpsDDR(patGpsPoint* the_gps) :
		m_gps(the_gps), m_ddr_arcs(), m_arc_ddr_ranges() {

	m_network_accuracy = patNBParameters::the()->networkAccuracy;
	m_min_point_ddr = patNBParameters::the()->maxDistanceGPSLoc;

	horizon_accuracy = sqrt(
			pow(m_gps->getHorizonAccuracy(), 2) + pow(m_network_accuracy, 2));
	distance_threshold_loc = horizon_accuracy * sqrt(-2 * log(m_min_point_ddr));
}

patGpsDDR::~patGpsDDR() {
}

void patGpsDDR::setAlgoParams(const map<string, double>* theAlgoParams) {
	if (theAlgoParams == NULL) {
		m_network_accuracy = patNBParameters::the()->networkAccuracy;
		m_min_point_ddr = patNBParameters::the()->maxDistanceGPSLoc;
		m_min_normal_speed = patNBParameters::the()->minNormalSpeed;
		m_max_normal_speed_heading = patNBParameters::the()->maxNormalSpeedHeading;
	} else {
		m_network_accuracy = (*theAlgoParams)["networkAccuracy"];
		m_min_point_ddr = (*theAlgoParams)["minPointDDR"];
		m_min_normal_speed = (*theAlgoParams)["minNormalSpeed"];
		m_max_normal_speed_heading = (*theAlgoParams)["maxNormalSpeedHeading"];
	}
	m_horizon_accuracy = sqrt(
			pow(m_gps->getHorizonAccuracy(), 2) + pow(m_network_accuracy, 2));
	m_distance_threshold_loc = m_horizon_accuracy * sqrt(-2 * log(m_min_point_ddr));
}
void patGpsDDR::setAlgoParams(string paramName, double paramValue) {
	if (paramName == "networkAccuracy") {
		m_network_accuracy = paramValue;
	} else if (paramName == "minPointDDR") {
		m_min_point_ddr = paramValue;
	} else if (paramName == "minNormalSpeed") {
		m_min_normal_speed = paramValue;
	} else if (paramName == "maxNormalSpeedHeading") {
		m_max_normal_speed_heading = paramValue;
	}

	m_horizon_accuracy = sqrt(
			pow(m_gps->getHorizonAccuracy(), 2) + pow(m_network_accuracy, 2));
	m_distance_threshold_loc = m_horizon_accuracy * sqrt(-2 * log(m_min_point_ddr));

}
struct arc_ddr_range patGpsDDR::detArcDDR(const patArc* arc) {

	const patNode* upNode = arc->getUpNode();
	const patNode* downNode = arc->getDownNode();
	const patGeoCoordinates * upNodeGeoCoord = upNode->getGeoCoord();
	const patGeoCoordinates* downNodeGeoCoord = downNode->getGeoCoord();

	map<char*, double> distance_to_arc;
	double heading_accuracy_bound = patNBParameters::the()->maxHeadingGPSArc;
	double arc_heading = arc->attributes.heading;

	if (arc_heading < 0.0) {
		DEBUG_MESSAGE("bad arc" << *arc);
		return false;
	}
	if (heading < 0.0 || type != "normal_speed"
			|| fabs(arc_heading - heading) < heading_accuracy_bound
			|| (heading < heading_accuracy_bound
					&& (360 - arc_heading + heading) < heading_accuracy_bound
					&& (360 - arc_heading + heading) > 0.0)
			|| (heading > 360 - heading_accuracy_bound
					&& (360 - heading + arc_heading) < heading_accuracy_bound
					&& (360 - heading + arc_heading) > 0.0)) {
		distance_to_arc = gps->distanceTo(upNodeGeoCoord, downNodeGeoCoord);
		struct link_ddr_range ldr = detArcDDRRange(distance_to_arc);
		//			DEBUG_MESSAGE(ldr.lower_bound<<"-"<<ldr.upper_bound);
		if (ldr.lower_bound == 0.0 && ldr.upper_bound == 0.0) {
			return true;
		} else if (ldr.upper_bound - ldr.lower_bound < 0.00001) {

			return false;
		} else {
			double measureDDRLink = measureDDR(distance_to_arc, ldr);

			m_ddr_arcs.insert(m_ddr_arcs.end(),
					pair<patArc*, double>(arc, measureDDRLink));
			m_arc_ddr_ranges.insert(arc_ddr_ranges.end(),
					pair<patArc*, struct link_ddr_range>(arc, ldr));
			return true;
		}

	}
	return false;
}

struct link_ddr_range patGpsDDR::detArcDDRRange(
		map<char*, double> arc_distance) {
	struct link_ddr_range ldr;
	ldr.lower_bound = 0.0;
	ldr.upper_bound = 1.0;
	if (arc_distance["link"] > distanceThreshold_Loc) {
		ldr.upper_bound = 0.0;
	} else {

		double e_d = calPerpendicularFootOnArc(arc_distance);
		if (e_d <= 0) {
			ldr.upper_bound = e_d
					+ sqrt(
							-2.0 * log(minPointDDR) * pow(horizonAccuracy, 2)
									- pow(arc_distance["ver"], 2))
							/ arc_distance["length"];

		}
		if (e_d >= 1) {
			ldr.lower_bound = e_d
					- sqrt(
							-2.0 * log(minPointDDR) * pow(horizonAccuracy, 2)
									- pow(arc_distance["ver"], 2))
							/ arc_distance["length"];

		} else {
			ldr.upper_bound = e_d
					+ sqrt(
							-2.0 * log(minPointDDR) * pow(horizonAccuracy, 2)
									- pow(arc_distance["ver"], 2))
							/ arc_distance["length"];
			ldr.lower_bound = e_d
					- sqrt(
							-2.0 * log(minPointDDR) * pow(horizonAccuracy, 2)
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


bool patGpsDDR::empty() {
	return m_ddr_arcs.empty();
}
int patGpsDDR::size() {
	return m_ddr_arcs.size();
}

set<patArc*> patGpsDDR::detInherentDDR(const patGpsDDR* prev_ddr) {
	//check the element of previous domain is also domain of the current gps point.
	set<patArc*> inherentDDR;
	//DEBUG_MESSAGE("previous point. "<< prevGpsPoint->getTimeStamp() ) ;

	const map<patArc*, double>* prev_ddr_arcs = prev_ddr->getDDRArcs();

	for (map<patArc*, double>::const_iterator arc_iter = prev_ddr_arcs->begin();
			arc_iter != prev_ddr_arcs->end(); ++arc_iter) {
		if (detArcDDR(arc_iter->first)) {
			inherentDDR.insert(arc_iter->first);
		}
	}

	return inherentDDR;
}

map<double, set<patArc*> > patGpsDDR::sortDDRByValue() {
	map<double, set<patArc*> > sorted_ddr;
	for (map<patArc*, double>::iterator iter = m_ddr_arcs.begin();
			iter != m_ddr_arcs.end(); ++iter) {
		if (sorted_ddr.find(iter->second) == sorted_ddr.end()) {
			sorted_ddr[iter->second] = set<patArc*>();
		}
		sorted_ddr[iter->second].insert(iter->first);
	}
	return sorted_ddr;

}

double patGpsDDR::measureDDR(map<char*, double> distance_to_arc
		, link_ddr_range ldr) {
	struct arc_ddr_params param;
	param.d = theDistance["ver"];
	param.l = theDistance["length"];
	param.e_d = m_gps->calPerpendicularFootOnArc(distance_to_arc);
	param.std_x = m_horizon_accuracy;
	param.ldr = ldr;
	//DEBUG_MESSAGE("d"<<param.d<<",l"<<param.l<<",e_d"<<param.e_d<<",deltax"<<param.std_x);
	patArcDDR theArcDDR(m_horizon_accuracy);
	return theArcDDR.errCDF(param);
}

double patGpsDDR::calDDRSum() {
	m_ddr_sum = 0.0;
	for (map<patArc*, double>::const_iterator iter = m_ddr_arcs.begin();
			iter != m_ddr_arcs.end(); ++iter) {

		m_ddr_sum += iter->second;

	}
	return m_ddr_sum;
}

double patGpsDDR::calArcDDR(const patArc* arc) {

	map<char*, double> arc_distance = distanceTo(arc);
	struct link_ddr_range ldr;
	ldr = detArcDDRRange(arc_distance);
	ddr_arcs_Range.insert(ddr_arcs_Range.end(),
			pair<patArc*, struct link_ddr_range>(arc, ldr));
	if (ldr.lower_bound == 0.0 && ldr.upper_bound == 0.0) {
		return 0.0;
	} else if (ldr.upper_bound - ldr.lower_bound < 0.00001) {

		return 0.0;
	}
	double measureDDRLink = measureDDR(arc_distance, ldr);
	return measureDDRLink;
}

double patGpsDDR::getDistanceCeilFromPrevDDR(const patGpsPoint* prevGpsPoint) {
	double maxSpeed = getMaxSpeed(prevGpsPoint);
	maxSpeed = (maxSpeed > 3.0) ? maxSpeed : 15.0;
	return maxSpeed * (timeStamp - prevGpsPoint->getTimeStamp())
			* patNBParameters::the()->maxDistanceRatioSP;
}

set<patArc*> patGpsDDR::getDDRArcSet() {
	set<patArc*> arc_set;
	for (map<patArc*, double>::iterator const_arc_iter = m_ddr_arcs.begin();
			arc_iter != m_ddr_arcs.end(); ++arc_iter) {

		arc_set.insert(arc_iter->first);
	}

	return arc_set;
}

void patGpsDDR::lowSpeedDDR(const set<patArc*>* arc_set) {
	for (set<patArc*>::const_iterator arc_iter = arc_set->begin();
			arc_iter != arc_set->end(); ++arc_iter) {
		detArcDDR(*arc_iter);
	}
	//selectDomainByCdf();
	//selectDomainByNumber(patNBParameters::the()->maxDomainSize);
	calDDRSum();

}

double patGpsDDR::getArcDDRValue(const_patArc* arc) {
	map<patArc*, double>::const_iterator arc_found = m_ddr_arcs.find(arc);
	if (arc_found == m_ddr_arcs.end()) {
		return calArcDDR(arc);
	} else {
		return arc_found->second;
	}
}
bool patGpsDDR::isArcInDomain(const patArc* arc) {
	if (m_ddr_arcs.find(arc) == m_ddr_arcs.end()) {
		return false;
	} else {
		return true;
	}
}

struct link_ddr_range patGpsDDR::getArcDDRRange(const patArc* arc, patError*& err) {

	map<patArc*, struct link_ddr_range>::const_iterator arc_ddr_found = m_arc_ddr_ranges.find(arc);
	if(arc_ddr_found==m_arc_ddr_ranges.end()){
	    err = new patErrNullPointer("Arc doesn't in ddr") ;
	    WARNING(err->describe()) ;
	    struct link_ddr_range d;
		return d;
	}
	else{
		return arc_ddr_found->second;
	}
}

map<patArc*, struct link_ddr_range> patGpsDDR::getArcDDRRange() {

	return m_arc_ddr_ranges;
}

void patGpsDDR::genDDRFromPaths(const set<patPathJ>* path_set) {
	set<const patArc*> excluded_arcs; //Arcs that are already excluded from domain. This is a cache to speed up the detection.

	for (set<patPathJ>::const_iterator path_iter = path_set->begin();
			path_iter != path_set->end(); ++path_iter) {

		const list<patArc*>* arc_list = pathIter->getArcList();
		for (list<patArc*>::const_iterator arc_iter = arc_list->begin();
				arc_iter != arc_list->end(); ++arc_iter) {
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

	calDDRSum();

}

void patGpsDDR::selectInitById(const set<patULong>* arc_ids) {
	map< patArc*, double> ddrTemp;
	for (map<patArc*, double>::const_iterator arc_iter = ddr_arcs.begin();
			arc_iter != ddr_arcs.end(); ++arc_iter) {

		if (arc_ids->find(arc_iter->first->getUserId()) != arc_ids->end()) {
			ddrTemp.insert(*arc_iter);
		}
	}

	m_ddr_arcs = ddrTemp;

}

void patGpsDDR::selectInitByName(const set<string>* arcName) {
	map< patArc*, double> ddrTemp;
	for (map<patArc*, double>::const_iterator arc_iter = ddr_arcs.begin();
			arc_iter != ddr_arcs.end(); ++arc_iter) {

		if (arcName->find(arc_iter->first->getName()) != arcName->end()) {
			ddrTemp.insert(*arc_iter);
		}
	}

	m_ddr_arcs = ddrTemp;

}

set<patArc*> patGpsDDR::selectDomainByCdf() {
	if (m_ddr_arcs.empty()
			|| m_ddr_arcs.size() < patNBParameters::the()->minDomainSize) {

		return getDomainSet();
	}

	set<patArc*> dSet = selectDomainByDistance();
	map<double, set<patArc*> > ddrTemp = sortDDRByValue();
	double total = calDDRSum();
	map<double, set<patArc*> >::iterator aIter = ddrTemp.end();
	double cdf = 0.0;
	double thre = total * patNBParameters::the()->minDomainDDRCdf;
	//DEBUG_MESSAGE("thre"<<thre<<"total:"<<total<<"ratio:"<<patNBParameters::the()->minDomainDDRCdf);
	while (cdf < thre && aIter != ddrTemp.begin()) {
		aIter--;
		cdf += aIter->first * aIter->second.size();
		dSet.insert(aIter->second.begin(), aIter->second.end());

	}

	DEBUG_MESSAGE(" select by cdf, domain size:" << dSet.size());
	return dSet;
}
void patGpsDDR::selectDomainByNumber(int k) {
	if (m_ddr_arcs.empty() || k <= 0) {
		return;
	}

	map<double, set<patArc*> > ddrTemp = sortDDRByValue();
	m_ddr_arcs = map<patArc*, double>();
	map<double, set<patArc*> >::iterator aIter = ddrTemp.end();
	for (unsigned int i = 0; i < k; ++i) {
		if (aIter != ddrTemp.begin()) {
			aIter--;
			for (set<patArc*>::iterator bIter = aIter->second.begin();
					bIter != aIter->second.end(); ++bIter) {
				m_ddr_arcs[const_cast<patArc*>(*bIter)] = aIter->first;

			}
		}

	}
	calDDRSum();

}

void patGpsDDR::selectInitByDDR(double lowerBound, double upperBound) {
	map<patArc*, double> ddrTemp;
	for (map<patArc*, double>::iterator arc_iter = m_ddr_arcs.begin();
			arc_iter != m_ddr_arcs.end(); ++arc_iter) {

		if ((arc_iter->second >= lowerBound && arc_iter->second <= upperBound)
				|| (arc_iter->second >= 39 && arc_iter->second <= 40)) {
			ddrTemp.insert(*arc_iter);
		}
	}

	ddr_arcs = ddrTemp;
}

void patGpsDDR::selectInitBySortDDR(int k) {
	map<double, set<patArc*> > ddrTemp;
	for (map<patArc*, double>::const_iterator arc_iter = m_ddr_arcs.begin();
			arc_iter != m_ddr_arcs.end(); ++arc_iter) {

		if (ddrTemp.find(arc_iter->second) == ddrTemp.end()) {
			ddrTemp[arc_iter->second] = set<patArc*>();
		}
		ddrTemp[arc_iter->second].insert(arc_iter->first);

	}

	m_ddr_arcs = map<patArc*, double>();
	map<double, set<patArc*> >::iterator aIter = ddrTemp.end();
	for (int i = 0; i < k; ++i) {
		aIter--;
		for (set<patArc*>::iterator bIter = aIter->second.begin();
				bIter != aIter->second.end(); ++bIter) {
			m_ddr_arcs[*bIter] = aIter->first;

		}

		if (aIter == ddrTemp.begin()) {
			break;
		}
	}
}

set<patArc*> patGpsDDR::selectDomainByDistance() {

	set<patArc*> dSet;
	if (ddr_arcs.empty()) {
		return dSet;
	}
	map<double, set<patArc*> > tmpDDRMap;
	for (map<patArc*, double>::const_iterator arc_iter = m_ddr_arcs.begin();
			arc_iter != m_ddr_arcs.end(); ++arc_iter) {
		double dl = distanceTo(arc_iter->first)["link"];
		//		DEBUG_MESSAGE("distance to link"<<dl);
		if (tmpDDRMap.find(dl) == tmpDDRMap.end()) {
			tmpDDRMap.insert(pair<double, set<patArc*> >(dl, set<patArc*>()));
		}
		tmpDDRMap[dl].insert(arc_iter->first);
	}
	map<double, set<patArc*> >::iterator tmpIter = tmpDDRMap.end();
	for (int k = 0; k < patNBParameters::the()->selectDDRByDistance;
			++k) {
		if (tmpIter == tmpDDRMap.begin()) {
			break;
		} else {
			tmpIter--;
			dSet.insert(tmpIter->second.begin(), tmpIter->second.end());
		}
	}

	return dSet;
}

void patGpsDDR::selectInitByOrigNode(set<unsigned long> origId) {
	map<patArc*, double> ddrTemp;
	for (map<patArc*, double>::const_iterator arc_iter = m_ddr_arcs.begin();
			arc_iter != m_ddr_arcs.end(); ++arc_iter) {

		if (origId.find(arc_iter->first->getUpNodeId()) != origId.end()) {
			ddrTemp.insert(*arc_iter);
		}
	}

	m_ddr_arcs = ddrTemp;

}
