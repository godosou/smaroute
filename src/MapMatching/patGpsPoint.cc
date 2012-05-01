//----------------------------------------------------------------
// File: patGpsPoint.cc
// Author:
// Creation:
//----------------------------------------------------------------

#include "patGpsPoint.h"
#include "patArc.h"
#include "patConst.h"
#include "patGeoCoordinates.h"
#include <math.h>
#include "patDisplay.h"
#include "patNBParameters.h"
#include "patGpsDDR.h"
#include<sstream>
#include<sstream>
#include "kml/base/date_time.h"
#include "patNetworkEnvironment.h"
#include "kml/dom.h"
using kmldom::CoordinatesPtr;
using kmldom::KmlFactory;
using kmldom::PointPtr;
using kmldom::PlacemarkPtr;
using kmldom::TimeStampPtr;
using kmlbase::DateTime;
const double threshold = 0.90;

patGpsPoint::patGpsPoint(unsigned long theUserId, double theTimeStamp,
		double lat, double lon, double theSpeed, double thehorizonAccuracy,
		double theHeading, double theVerticalAccuracy, double theSpeedAccuracy,
		double theHeadingAccuracy, map<string, double>* theAlgoParams) :
		m_userId(theUserId), m_geoCoord(lat, lon), m_speed(theSpeed), m_heading(
				theHeading), m_verticalAccuracy(theVerticalAccuracy), m_coordsHorizontalAccuracy(
				thehorizonAccuracy), m_speedAccuracy(theSpeedAccuracy), m_headingAccuracy(
				theHeadingAccuracy) {

	//DEBUG_MESSAGE(this);
	m_timestamp = theTimeStamp;
	m_measurement_type = MeasurementType(GPS);
	if (theAlgoParams == NULL) {
		m_networkAccuracy = patNBParameters::the()->networkAccuracy;
		m_minPointDDR = patNBParameters::the()->maxDistanceGPSLoc;
		m_minNormalSpeed = patNBParameters::the()->minNormalSpeed;
		m_maxNormalSpeedHeading = patNBParameters::the()->maxNormalSpeedHeading;
	} else {
		m_networkAccuracy = (*theAlgoParams)["networkAccuracy"];
		m_minPointDDR = (*theAlgoParams)["minPointDDR"];
		m_minNormalSpeed = (*theAlgoParams)["minNormalSpeed"];
		m_maxNormalSpeedHeading = (*theAlgoParams)["maxNormalSpeedHeading"];
	}
	m_horizonAccuracy = sqrt(
			pow(m_coordsHorizontalAccuracy, 2) + pow(m_networkAccuracy, 2));
	m_distanceThreshold_Loc = m_horizonAccuracy * sqrt(-2 * log(m_minPointDDR));
	m_ddr = new patGpsDDR(this);
	//DEBUG_MESSAGE("distanceThreshold:"<<patNBParameters::the()->maxDistanceGPSLoc<<","<<distanceThreshold_Loc);
}
bool operator<(const patGpsPoint& g1, const patGpsPoint& g2) {
	if (g1.m_timestamp < g2.m_timestamp) {
		return true;
	}
	if (g1.m_timestamp > g2.m_timestamp) {
		return false;
	}
	if (g1.m_geoCoord.latitudeInRadians < g2.m_geoCoord.latitudeInRadians) {
		return true;
	}
	if (g1.m_geoCoord.latitudeInRadians > g2.m_geoCoord.latitudeInRadians) {
		return false;
	}
	if (g1.m_geoCoord.longitudeInRadians < g2.m_geoCoord.longitudeInRadians) {
		return true;
	}
	if (g1.m_geoCoord.longitudeInRadians > g2.m_geoCoord.longitudeInRadians) {
		return false;
	}

	return false;

}

map<string, double> patGpsPoint::distanceTo(const patGeoCoordinates& upGeoCoord,
		const patGeoCoordinates& downGeoCoord) const {

	map<string, double> distance;


	distance["up"] = m_geoCoord.distanceTo(
			upGeoCoord);
	distance["down"] = m_geoCoord.distanceTo(
			downGeoCoord);
	distance["length"] =(&upGeoCoord)->distanceTo(
					downGeoCoord);

	double cosUpNode = (distance["up"] * distance["up"]
			+ distance["length"] * distance["length"]
			- distance["down"] * distance["down"])
			/ (2 * distance["up"] * distance["length"]);
	double cosDownNode = (distance["down"] * distance["down"]
			+ distance["length"] * distance["length"]
			- distance["up"] * distance["up"])
			/ (2 * distance["down"] * distance["length"]);
	double p = (distance["up"] + distance["down"] + distance["length"]) / 2;

	double s = sqrt(
			p * (p - distance["up"]) * (p - distance["down"])
					* (p - distance["length"]));
	distance["ver"] = 2 * s / distance["length"];
	/*
	 if(isnan(distance["ver"])){
	 DEBUG_MESSAGE("distance: s:"<<s<<"p:"<<p<<"length:"<<distance["length"]<<"up:"<<distance["up"]<<"length:"<<distance["length"]);
	 }*/

	if (distance["up"] + distance["down"] <= distance["length"]) {
		distance["position"] = 0;
		distance["ver"] = 0.0;
		distance["link"] = 0.0;
	} else if (distance["up"] + distance["length"] <= distance["down"]) {
		distance["position"] = -1;
		distance["ver"] = 0.0;
		distance["link"] = distance["up"];
	} else if (distance["down"] + distance["length"] <= distance["up"]) {
		distance["position"] = 1;
		distance["ver"] = 0.0;

		distance["link"] = distance["down"];
	} else if (cosUpNode < 0) {
		distance["link"] = distance["up"];
		distance["position"] = -1;
	} else if (cosDownNode < 0) {
		distance["link"] = distance["down"];
		distance["position"] = 1;
	} else {
		distance["link"] = distance["ver"];
		distance["position"] = 0;
	}

	return distance;
}

double patGpsPoint::distanceTo(const patGpsPoint* aGpsPoint) const {
	return m_geoCoord.distanceTo(
			*(aGpsPoint->getGeoCoord()));
}

map<string, double> patGpsPoint::distanceTo(const patArc* theArc) const {

	const patNode* up_node = theArc->getUpNode();
	const patNode* down_node = theArc->getDownNode();
	if (up_node == NULL || down_node == NULL) {
		WARNING("node doesn't exist");
		return map<string, double>();
	}
	return distanceTo(up_node->getGeoCoord(), down_node->getGeoCoord());
}

double patGpsPoint::distanceTo(const patNode* theNode) const {
	return m_geoCoord.distanceTo(
			theNode->getGeoCoord());
}

double patGpsPoint::calHeading(const patGpsPoint* nextGpsPoint) const {
	return calHeading(&m_geoCoord, nextGpsPoint->getGeoCoord());
}

double patGpsPoint::calHeading(const patGpsPoint* prevGpsPoint,
		const patGpsPoint* nextGpsPoint) const {
	double incoming = prevGpsPoint->calHeading(this);
	double outGoing = this->calHeading(nextGpsPoint);
	double ratio = 0.5;
	//	DEBUG_MESSAGE("incoming:"<<incoming<<",outgoing:"<<outGoing);
	if (fabs(incoming - outGoing) <= 180.0) {

		double s1 = prevGpsPoint->getSpeed();
		double s2 = m_speed;

		//ratio= s1 / (s1 + s2);
		//double ratio  = 0.5;
		return ratio * incoming + (1 - ratio) * outGoing;
	} else {
		double h1 = incoming;
		double h2 = outGoing;
		double s1 = prevGpsPoint->getSpeed();
		double s2 = m_speed;
		if (incoming > 180.0) {
			h2 = incoming;
			h1 = outGoing;

			s1 = m_speed;
			s2 = prevGpsPoint->getSpeed();
		}
		double diff = 360.0 - (h2 - h1);
		//double ratio = s1 / (s1 + s2);
		double rtn = h1 - (1.0 - ratio) * diff;
		if (rtn < 0.0) {
			rtn = h2 + ratio * diff;
		}
		return rtn;
	}

}
double patGpsPoint::calSpeed(const patGpsPoint* another_point) {

	double time_diff = fabs(another_point->getTimeStamp() - getTimeStamp());

	double speed =  3.6 * distanceTo(another_point) / time_diff;
	DEBUG_MESSAGE("speed"<<speed<<","<<distanceTo(another_point)<<"/"<<another_point->getTimeStamp() <<"-"<<getTimeStamp()<<"."<<time_diff);
	return speed;

}
double patGpsPoint::calSpeed(const patGpsPoint* prevGpsPoint,
		const patGpsPoint* nextGpsPoint) {
	if (prevGpsPoint==NULL && nextGpsPoint==NULL){
		WARNING("No valid GPS given");
		return 0.0;
	}
	else if (prevGpsPoint==NULL){
		return calSpeed(nextGpsPoint);
	}
	else if (nextGpsPoint==NULL){
		return calSpeed(prevGpsPoint);
	}
	else{
		double time_diff_prev = fabs( getTimeStamp()-prevGpsPoint->getTimeStamp() );
		double time_diff_next = fabs( getTimeStamp()-nextGpsPoint->getTimeStamp() );

		double speed_prev = calSpeed(prevGpsPoint);
		double speed_next  = calSpeed(nextGpsPoint);
		return time_diff_prev<=time_diff_next?speed_prev:speed_next;
	}
}
double patGpsPoint::calHeading(const patGeoCoordinates* startCoord,
		const patGeoCoordinates* nextCoord) const {

	double lng1 = startCoord->longitudeInRadians;
	double lat1 = startCoord->latitudeInRadians;
	double lng2 = nextCoord->longitudeInRadians;
	double lat2 = nextCoord->latitudeInRadians;

	double numerator = sin(lat1) * sin(lng2 - lng1);
	double denumerator = sin(lat2) * cos(lat1)
			- cos(lat2) * sin(lat1) * cos(lng2 - lng1);

	double theArcHeading = atan(numerator / denumerator) * 180 / pi;

	if (denumerator > 0) {
		theArcHeading += 360;
	} else {
		theArcHeading += 180;
	}
	if (theArcHeading < 0) {
		theArcHeading += 360;
	}
	if (theArcHeading >= 360) {
		theArcHeading -= 360;
	}
	return theArcHeading;
}

ostream & operator<<(ostream& str, const patGpsPoint& x) {
	str.precision(15);
	str << "Time:" << x.m_timestamp << ";Speed:" << x.m_speed
			<< ";Speed Accuracy:" << x.m_speedAccuracy << ";Heading:" << x.m_heading
			<< ";Horizontal horizonAccuracy:" << x.m_horizonAccuracy << "; type:"
			<< x.getType() << endl;
}

double patGpsPoint::getSpeed() const {
	return m_speed;
}

double patGpsPoint::getSpeedMS() const {
	return m_speed / 3.6;
}

double patGpsPoint::getSpeedAccuracyMS() const {
	return m_speedAccuracy / 3.6;
}

double patGpsPoint::getHeadingAccuracy() const {
	return m_headingAccuracy;
}

double patGpsPoint::getMaxSpeed(const patGpsPoint* prevGpsPoint) const {
	double maxSpeed = getSpeedMS();
	double geoSpeed = m_geoCoord.distanceTo(
			prevGpsPoint->m_geoCoord)
			/ (m_timestamp - prevGpsPoint->getTimeStamp());
	//geoSpeed = (geoSpeed < (120.0/3.6))?geoSpeed:(120.0/3.6);
	double nextSpeed = prevGpsPoint->getSpeedMS();
	maxSpeed = (maxSpeed < nextSpeed) ? nextSpeed : maxSpeed;
	maxSpeed = (maxSpeed < geoSpeed) ? geoSpeed : maxSpeed;

	return 3.6 * maxSpeed;

}

double patGpsPoint::getHorizonAccuracy() const {
	return m_horizonAccuracy;
}

const patGeoCoordinates* patGpsPoint::getGeoCoord() const {
	return &m_geoCoord;
}

pair<bool, double> patGpsPoint::isGpsPointInZone(
		const patGpsPoint* prevGpsPoint, const patGpsPoint* aGpsPoint) {
	double radiusCeiling = patNBParameters::the()->zoneRadius; //meter
	double timeCeiling = patNBParameters::the()->zoneTime;
	pair<bool, double> rtnValue = pair<bool, double>(false, 0.0);
	patGeoCoordinates center(
			(m_geoCoord.latitudeInDegrees
					+ prevGpsPoint->getGeoCoord()->latitudeInDegrees) / 2,
			(m_geoCoord.longitudeInDegrees
					+ prevGpsPoint->getGeoCoord()->longitudeInDegrees) / 2);
	double distance = center.distanceTo(*(aGpsPoint->getGeoCoord()));
	double distance_prev = prevGpsPoint->distanceTo(aGpsPoint);
	if ((fabs(m_timestamp - aGpsPoint->getTimeStamp()) < timeCeiling
			|| fabs(prevGpsPoint->getTimeStamp() - aGpsPoint->getTimeStamp())
					< timeCeiling) && distance < radiusCeiling) {
		rtnValue.first = true;
		rtnValue.second = distance;
	}

	return rtnValue;
}

map<patGpsPoint*, double> patGpsPoint::detGpsPointsInZone(
		const patGpsPoint* prevGpsPoint, vector<patGpsPoint>* gpsSequence) {
	map<patGpsPoint*, double> rtnValue;

	for (int i = 0; i < gpsSequence->size(); ++i) {
		//if(gpsSequence->at(i).getType()!= "normal_speed" && ){
		//		continue;
		//	}
		pair<bool, double> detTemp = isGpsPointInZone(prevGpsPoint,
				&(gpsSequence->at(i)));
		if (detTemp.first == true || &(gpsSequence->at(i)) == prevGpsPoint
				|| &(gpsSequence->at(i)) == &(*this)) {
			rtnValue[&(gpsSequence->at(i))] = detTemp.second;
		}

	}

	return rtnValue;
}

pair<double, double> patGpsPoint::calSpeedInZone(
		const patGpsPoint* prevGpsPoint, vector<patGpsPoint>* gpsSequence) {
	map<patGpsPoint*, double> gpsPointsInZone = detGpsPointsInZone(prevGpsPoint,
			gpsSequence);

	pair<double, double> speedProfile(0.0, 0.0);
	for (map<patGpsPoint*, double>::iterator gpsIter = gpsPointsInZone.begin();
			gpsIter != gpsPointsInZone.end(); ++gpsIter) {
		speedProfile.first += gpsIter->first->getSpeedMS();
	}
	speedProfile.first /= gpsPointsInZone.size();

	//double speedVariance;
	for (map<patGpsPoint*, double>::iterator gpsIter = gpsPointsInZone.begin();
			gpsIter != gpsPointsInZone.end(); ++gpsIter) {
		speedProfile.second += pow(gpsIter->first->getSpeedAccuracyMS(), 2);

		//speedProfile.second += pow( (gpsIter->first->getSpeedMS()-speedProfile.first),2);
	}
	speedProfile.second /= pow(gpsPointsInZone.size(), 2);

	//speedProfile.second/=(gpsPointsInZone.size()-1);

	speedProfile.second = sqrt(speedProfile.second);

	return speedProfile;
}

void patGpsPoint::setGpsParams(struct gps_params * p,
		const patGpsPoint* prevGpsPoint) const {
	p->time_diff = double(m_timestamp) - double(prevGpsPoint->getTimeStamp());

	p->time_prev = double(prevGpsPoint->getTimeStamp());
	p->time_curr = double(m_timestamp);

	p->mu_v_curr = getSpeedMS();
	p->std_v_curr = getSpeedAccuracyMS();

	p->mu_v_prev = prevGpsPoint->getSpeedMS();
	p->std_v_prev = prevGpsPoint->getSpeedAccuracyMS();

	p->std_x_prev = prevGpsPoint->getHorizonAccuracy();
	p->std_x_curr = getHorizonAccuracy();

}

double patGpsPoint::calPerpendicularFootOnArc(
		map<string, double> distanceToArc) const {
	if (isnan(distanceToArc["ver"])) {
		DEBUG_MESSAGE("wrong ver" << distanceToArc["position"]);
		if (distanceToArc["position"] == -1) {
			return 0;
		}
		if (distanceToArc["position"] == 1) {
			return 1;
		}
		return distanceToArc["ver"];
	} else if (distanceToArc["position"] == -1) {
		return -sqrt(
				pow(distanceToArc["link"], 2) - pow(distanceToArc["ver"], 2))
				/ distanceToArc["length"];
	} else if (distanceToArc["position"] == 0) {
		return sqrt(pow(distanceToArc["up"], 2) - pow(distanceToArc["ver"], 2))
				/ distanceToArc["length"];
	} else if (distanceToArc["position"] == 1) {
		return 1
				+ sqrt(
						pow(distanceToArc["link"], 2)
								- pow(distanceToArc["ver"], 2))
						/ distanceToArc["length"];
	}

}

string patGpsPoint::getType() const {
	return m_type;
}

void patGpsPoint::setSpeedType() {
	if (m_type != "low_speed" && m_speed > m_minNormalSpeed) {
		m_type = "normal_speed";
	} else {
		m_type = "slow_speed";
	}
	//  DEBUG_MESSAGE(type);
	/*
	 * Originally also use min average speed.
	 if (headingAccuracy < maxNormalSpeedHeading){
	 type = "normal_speed";
	 }
	 else{
	 type = "slow_speed";
	 }
	 */
}

void patGpsPoint::setType(string theType) {
	m_type = theType;
}

double patGpsPoint::getLatitude() const {
	return m_geoCoord.latitudeInRadians;
}

double patGpsPoint::getLongitude() const {
	return m_geoCoord.longitudeInRadians;
}

double patGpsPoint::getHeading() const {
	//DEBUG_MESSAGE(heading);
	return m_heading;
}

double patGpsPoint::getSpeedAccuracy() const {
	return m_speedAccuracy;
}

void patGpsPoint::setSpeed(double v) {
	m_speed = v;
}

void patGpsPoint::setSpeedAccuracy(double v) {
	m_speedAccuracy = v;
}

void patGpsPoint::setHeading(double v) {
	m_heading = v;
}

PlacemarkPtr patGpsPoint::getKML(int point_id) const {
	// Create <coordinates>.

	KmlFactory* factory = KmlFactory::GetFactory();
	CoordinatesPtr coordinates = factory->CreateCoordinates();
	// Create <coordinates>-122.0816695,37.42052549<coordinates>
	coordinates->add_latlng(m_geoCoord.latitudeInDegrees,
			m_geoCoord.longitudeInDegrees);

	// Create <Point> and give it <coordinates>.
	PointPtr point = factory->CreatePoint();
	point->set_coordinates(coordinates); // point takes ownership
	time_t rawtime = getTimeStamp();
	tm* ptm = gmtime(&rawtime);
	char buffer[50];
	sprintf(
			buffer,
			"%4d-%02d-%02dT%02d:%02d:%02dZ",
			(ptm->tm_year + 1900), (ptm->tm_mon + 1), ptm->tm_mday, (ptm->tm_hour) % 24, ptm->tm_min, ptm->tm_sec);
	kmlbase::DateTime* date_time = kmlbase::DateTime::Create(buffer);

	TimeStampPtr time_stamp = factory->CreateTimeStamp();
	time_stamp->set_when(string(buffer));
	PlacemarkPtr placemark = factory->CreatePlacemark();
	placemark->set_timeprimitive(time_stamp);
	// Create <Placemark> and give it a <name> and the <Point>.
	stringstream ss;
	ss << point_id;
	stringstream desc;
	desc << *this;
	placemark->set_styleurl("#gps");
	placemark->set_name(ss.str());
	placemark->set_description(desc.str());
	placemark->set_geometry(point); // placemark takes ownership
	return placemark;
}

void patGpsPoint::setMeasurementType() {
	m_measurement_type = MeasurementType(GPS);
}

MeasurementModelParam patGpsPoint::getMeasurementModelParam(const patArc* arc,
		TransportMode mode) const {
	MeasurementModelParam param;

	param.mode = mode;
	param.length_of_arc = arc->getLength();
	map<string, double> distance_to_arc = distanceTo(arc);
	param.foot_on_arc = calPerpendicularFootOnArc(distance_to_arc);
//	DEBUG_MESSAGE(param.length_of_arc);
	param.distance_to_arc = distance_to_arc["ver"];
	param.std_measurement = getHorizonAccuracy();
	return param;
}

double patGpsPoint::getSimpleValue() const {
	return getTimeStamp();
}
bool patGpsPoint::isGPS() const {
	return true;
}
