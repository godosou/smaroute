//----------------------------------------------------------------
// File: patArc.cc
// Author: Michel Bierlaire, Jingmin Chen
// Creation: Thu Oct 30 10:53:34 2008
//----------------------------------------------------------------

#include "patArc.h"
#include "patNode.h"
#include "patError.h"
#include "kml/dom.h"

#include "patDisplay.h"
#include "patConst.h"
#include "patErrNullPointer.h"
#include "patGeoCoordinates.h"
#include "patNBParameters.h"
using kmldom::CoordinatesPtr;
using kmldom::KmlFactory;
using kmldom::LineStringPtr;
using kmldom::PlacemarkPtr;

patArc::patArc(unsigned long theId, patNode* theUpNode, patNode* theDownNode,
		patString theName, struct arc_attributes theAttr, patError*& err) :
		m_user_id(theId), m_internal_id(patBadId), m_name(theName), m_attributes(
				theAttr), m_up_node(theUpNode), m_down_node(theDownNode), generalizedCost(
				0.0) {

	if ((theUpNode == NULL) || (theDownNode == NULL)) {
		err = new patErrNullPointer("patNode");
		WARNING(err->describe());
		return;
	}
	m_up_node_id = theUpNode->userId;
	m_down_node_id = theDownNode->userId;
	computeLength();
	calHeading();
}
double patArc::computeLength() {
	m_length = m_up_node->geoCoord.distanceTo(m_down_node->geoCoord);
}
patArc::patArc(unsigned long theId, patNode* theUpNode, patNode* theDownNode) :
		m_user_id(theId), m_up_node(theUpNode), m_down_node(theDownNode), generalizedCost(
				0.0) {
	computeLength();
	calHeading();
}
patArc::patArc(unsigned long theId, patNode* theUpNode, patNode* theDownNode,
		patString theName, patError*& err) :
		m_user_id(theId), m_internal_id(patBadId), m_name(theName), m_up_node(
				theUpNode), m_down_node(theDownNode) {
	if ((theUpNode == NULL) || (theDownNode == NULL)) {
		err = new patErrNullPointer("patNode");
		WARNING(err->describe());
		return;
	}
	m_up_node_id = theUpNode->userId;
	m_down_node_id = theDownNode->userId;
	m_length = theUpNode->geoCoord.distanceTo(theDownNode->geoCoord);
	calHeading();
}
void patArc::calPriority() {
	if (m_attributes.type == "steps") {
		m_attributes.priority = patNBParameters::the()->stepsPriority;
	} else if (m_attributes.type == "footway") {
		m_attributes.priority = patNBParameters::the()->footwayPriority;
	} else if (m_attributes.type == "cycleway") {
		m_attributes.priority = patNBParameters::the()->cyclewayPriority;
	} else if (m_attributes.type == "primary_link") {
		m_attributes.priority = patNBParameters::the()->primary_linkPriority;
	} else if (m_attributes.type == "trunk_link") {
		m_attributes.priority = patNBParameters::the()->trunk_linkPriority;
	} else if (m_attributes.type == "motorway_link") {
		m_attributes.priority = patNBParameters::the()->motorway_linkPriority;
	} else if (m_attributes.type == "bridleway") {
		m_attributes.priority = patNBParameters::the()->bridlewayPriority;
	} else if (m_attributes.type == "residential") {
		m_attributes.priority = patNBParameters::the()->residentialPriority;
	} else if (m_attributes.type == "unclassified") {
		m_attributes.priority = patNBParameters::the()->unclassifiedPriority;
	} else if (m_attributes.type == "tertiary") {
		m_attributes.priority = patNBParameters::the()->tertiaryPriority;
	} else if (m_attributes.type == "secondary") {
		m_attributes.priority = patNBParameters::the()->secondaryPriority;
	}

	else if (m_attributes.type == "primary") {
		m_attributes.priority = patNBParameters::the()->primaryPriority;
	}

	else if (m_attributes.type == "trunk") {
		m_attributes.priority = patNBParameters::the()->trunkPriority;
	}

	else if (m_attributes.type == "motorway") {
		m_attributes.priority = patNBParameters::the()->motorwayPriority;
	}

	else if (m_attributes.type == "railway") {
		m_attributes.priority = patNBParameters::the()->railwayPriority;
	} else {
		m_attributes.priority = patNBParameters::the()->otherRoadTypePriority;

	}
}
double patArc::calHeading() {

	patGeoCoordinates startCoord = m_up_node->geoCoord;
	patGeoCoordinates nextCoord = m_down_node->geoCoord;
	double lng1 = startCoord.longitudeInRadians;
	double lat1 = startCoord.latitudeInRadians;
	double lng2 = nextCoord.longitudeInRadians;
	double lat2 = nextCoord.latitudeInRadians;

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
	m_attributes.heading = theArcHeading;
	return theArcHeading;
}

double patArc::getLength() const {
	return m_length;
}

ostream& operator<<(ostream& str, const patArc& x) {
	str << "Arc " << x.m_user_id << " [" << x.m_name << "]("
			<< x.getUpNode()->getUserId() << "->"
			<< x.getDownNode()->getUserId() << ")";
	str << " length=" << x.m_length << "m; ";
	return str;
}
const patNode* patArc::getUpNode() const {
	return m_up_node;
}
const patNode* patArc::getDownNode() const {
	return m_down_node;
}

list<const patArc*> patArc::getArcList() const {

	list<const patArc*> rtn;
	rtn.push_back(this);
	return rtn;
}
unsigned long patArc::getUserId() const {
	return m_user_id;
}

double patArc::getHeading() const {
	return m_attributes.heading;
}
patString patArc::getName() const {
	return m_name;
}
int patArc::size() const {
	return 1;
}
bool patArc::isValid() const {
	return true;
}
PlacemarkPtr patArc::getArcKML(string mode) const {
	KmlFactory* factory = KmlFactory::GetFactory();

	stringstream ss;
	CoordinatesPtr coordinates = factory->CreateCoordinates();
	coordinates->add_latlng(getUpNode()->getLatitude(), getUpNode()->getLongitude());
	coordinates->add_latlng(getDownNode()->getLatitude(), getDownNode()->getLongitude());

	LineStringPtr line_string = factory->CreateLineString();
	line_string->set_coordinates(coordinates); // point takes ownership

	PlacemarkPtr placemark = factory->CreatePlacemark();
	placemark->set_name(getName());
	ss<<*this;
	placemark->set_description(ss.str());
	placemark->set_styleurl("#"+mode);
	placemark->set_geometry(line_string); // placemark takes ownership
	return placemark;

}
