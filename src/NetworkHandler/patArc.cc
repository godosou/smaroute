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
#include "patCoordinates.h"
#include "patNBParameters.h"
#include "patException.h"
#include <boost/lexical_cast.hpp>
using kmldom::CoordinatesPtr;
using kmldom::KmlFactory;
using kmldom::LineStringPtr;
using kmldom::PlacemarkPtr;
patArc::patArc(){

}
patArc::patArc(unsigned long theId, const patNode* theUpNode,
		const patNode* theDownNode, string theName,
		struct arc_attributes theAttr, patError*& err) :
		m_user_id(theId), m_internal_id(patBadId), m_name(theName), m_attributes(
				theAttr), m_up_node(theUpNode), m_down_node(theDownNode), generalizedCost(
				0.0) {

	if ((theUpNode == NULL) || (theDownNode == NULL)) {
		err = new patErrNullPointer("patNode");
		WARNING(err->describe());
		return;
	}
	computeLength();
	calHeading();
	genArcString();
}
double patArc::computeLength() {
	m_length = m_up_node->getGeoCoord().distanceTo(m_down_node->geoCoord);
}
patArc::patArc(unsigned long theId, const patNode* theUpNode,
		const patNode* theDownNode) :
		m_user_id(theId), m_up_node(theUpNode), m_down_node(theDownNode), generalizedCost(
				0.0) {
	computeLength();
	calHeading();
	genArcString();
}
patArc::patArc(unsigned long theId, const patNode* theUpNode,
		const patNode* theDownNode, string theName, patError*& err) :
		m_user_id(theId), m_internal_id(patBadId), m_name(theName), m_up_node(
				theUpNode), m_down_node(theDownNode) {
	if ((theUpNode == NULL) || (theDownNode == NULL)) {
		err = new patErrNullPointer("patNode");
		WARNING(err->describe());
		return;
	}
	m_length = theUpNode->getGeoCoord().distanceTo(theDownNode->geoCoord);
	calHeading();
	genArcString();
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

	patCoordinates startCoord = m_up_node->getGeoCoord();
	patCoordinates nextCoord = m_down_node->getGeoCoord();
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

vector<const patArc*> patArc::getArcList() const {

	vector<const patArc*> rtn;
	rtn.push_back(this);
	return rtn;
}
unsigned long patArc::getUserId() const {
	return m_user_id;
}

double patArc::getHeading() const {
	return m_attributes.heading;
}
string patArc::getName() const {
	return m_name;
}
int patArc::size() const {
	return 1;
}
bool patArc::isValid() const {
	return true;
}
vector<PlacemarkPtr> patArc::getArcKML(string mode) const {
	KmlFactory* factory = KmlFactory::GetFactory();

	stringstream ss;
	CoordinatesPtr coordinates = factory->CreateCoordinates();
	coordinates->add_latlng(getUpNode()->getLatitude(),
			getUpNode()->getLongitude());
	coordinates->add_latlng(getDownNode()->getLatitude(),
			getDownNode()->getLongitude());

	LineStringPtr line_string = factory->CreateLineString();
	line_string->set_coordinates(coordinates); // point takes ownership

	PlacemarkPtr placemark = factory->CreatePlacemark();
	placemark->set_name(getName());
	ss << *this;
	placemark->set_description(ss.str());
	placemark->set_styleurl("#" + mode);
	placemark->set_geometry(line_string); // placemark takes ownership

	vector<PlacemarkPtr> rtn;
	rtn.push_back(placemark);
	return rtn;

}

map<string, double> patArc::distanceTo(const patNode* a_node) const {

	map<string, double> distance;

	patCoordinates up_geo = getUpNode()->getGeoCoord();
	patCoordinates down_geo = getDownNode()->getGeoCoord();
	patCoordinates a_node_geo = a_node->getGeoCoord();
	distance["up"] = a_node_geo.distanceTo(up_geo);
	distance["down"] = a_node_geo.distanceTo(down_geo);
	distance["length"] = up_geo.distanceTo(down_geo);

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

	double arc_heading = getHeading();
	double up_heading = getUpNode()->calHeading(a_node);
	if (arc_heading <= 180.0) {
		if (up_heading - arc_heading >= 0
				and up_heading - arc_heading <= 180.0) {

			distance["right"] = 1;
		} else {

			distance["right"] = 0;
		}
	} else {

		if (up_heading - arc_heading >= 0
				or up_heading + arc_heading <= 360.0) {

			distance["right"] = 1;
		} else {

			distance["right"] = 0;
		}
	}

	return distance;
}

double patArc::getAttribute(ARC_ATTRIBUTES_TYPES attribute_name) const {
    switch (attribute_name) {
        case ENUM_LENGTH:
            return m_length;
            break;
        case ENUM_SPEED_BUMP:
        {
            unordered_map<string, string>::const_iterator find_sb = m_tags.find("speed_bump");
            if (find_sb != m_tags.end() && find_sb->second == "yes") {
                return 1.0;
            } else {
                return 0.0;
            }
        }   break;

        case ENUM_TRAFFIC_SIGNAL:
        {
            if (getDownNode()->hasTrafficSignal()) {
                return 1.0;
            } else {
                return 0.0;
            }
        }   break;
        default:
            throw RuntimeException("Non valid attribute");
            return 0.0;
    }

}

void patArc::setTags(const unordered_map<string, string>& tags) {
	m_tags = tags;
}
string patArc::getAttributeTypeString(ARC_ATTRIBUTES_TYPES type_name){
    switch (type_name) {
        case ENUM_LENGTH:
            return "length";
            break;
        case ENUM_SPEED_BUMP:
return "speed_bump";
            break;

        case ENUM_TRAFFIC_SIGNAL:
return "traffic_signal";
            break;
        default:
            throw RuntimeException("Non valid attribute");
    }


}


double patArc::computeGeneralizedCost(const map<ARC_ATTRIBUTES_TYPES, double>& link_coef){
	m_generalized_cost = 0.0;
	for (map<ARC_ATTRIBUTES_TYPES, double>::const_iterator a_iter =
			link_coef.begin(); a_iter != link_coef.end(); ++a_iter) {
		if (a_iter->second != 0.0) {
//			DEBUG_MESSAGE(a_iter->second<<"*"<<getAttribute(a_iter->first));
			m_generalized_cost += a_iter->second
					* ((double) getAttribute(a_iter->first));
		}
	}

	return m_generalized_cost;
//	DEBUG_MESSAGE(m_generalized_cost);
}

vector<const patArc*>  patArc::getOriginalArcList() const{
	return getArcList();
}
void patArc::genArcString(){
	m_arc_string = boost::lexical_cast<string>(getUserId())+string(";");
//	cout<<"arc:"<<getArcString()<<endl;
}
