//----------------------------------------------------------------
// File: patNode.cc
// Author: Michel Bierlaire
// Creation: Thu Oct 30 10:51:19 2008
//----------------------------------------------------------------

#include "patConst.h"
#include "patNode.h"
#include "patGeoCoordinates.h"
#include "patDisplay.h"
#include "patArc.h"
#include "patStlSetIterator.h"

#include "kml/dom.h"
using kmldom::CoordinatesPtr;
using kmldom::KmlFactory;
using kmldom::PointPtr;
using kmldom::PlacemarkPtr;
patNode::patNode(unsigned long theId, patString theName, double lat, double lon,
		struct node_attributes theAttr) :
		userId(theId), internalId(patBadId), name(theName), geoCoord(lat, lon), attributes(
				theAttr), isCentroid(false) {
}
patNode::patNode(unsigned long theId, double lat, double lon) :
		userId(theId), internalId(patBadId), geoCoord(lat, lon), isCentroid(
				false) {

}

void patNode::addSuccessor(unsigned long aSucc) {
	userSuccessors.insert(aSucc);
}

void patNode::addPredecessor(unsigned long aPred) {
	userPredecessors.insert(aPred);
}

string patNode::getTagString() const {
	string str;
	for (map<string, string>::const_iterator tag_iter = m_tags.begin();
			tag_iter != m_tags.end(); ++tag_iter) {
		str += tag_iter->first + ": " + tag_iter->second + ",";
	}
	return str;
}
ostream& operator<<(ostream& str, const patNode& x) {

	str << "Node " << x.userId << " [" << x.name << "] " << x.geoCoord;
	if (!x.userPredecessors.empty()) {
		str << " P(";
		for (set<unsigned long>::iterator i = x.userPredecessors.begin();
				i != x.userPredecessors.end(); ++i) {
			if (i != x.userPredecessors.begin()) {
				str << ",";
			}
			str << *i;
		}
		str << ")";
	}
	if (!x.userSuccessors.empty()) {
		str << " P(";
		for (set<unsigned long>::iterator i = x.userSuccessors.begin();
				i != x.userSuccessors.end(); ++i) {
			if (i != x.userSuccessors.begin()) {
				str << ",";
			}
			str << *i;
		}
		str << ")";
	}
}

bool patNode::disconnected() const {
	if (userSuccessors.size() > 0) {
		return false;
	}
	if (userPredecessors.size() > 0) {
		return false;
	}
	return true;
}

patString patNode::getName() const {
	return name;
}

void patNode::setName(string the_name) {
	name = the_name;
}

patIterator<unsigned long>* patNode::getSuccessors() {
	patIterator<unsigned long>* ptr = new patStlSetIterator<unsigned long>(
			userSuccessors);
	return ptr;
}

unsigned long patNode::getUserId() const {
	return userId;
}
const patArc* patNode::getOutgoingArc(unsigned long down_node_id) const {
	map<unsigned long, patArc*>::const_iterator find = outgoingArcs.find(
			down_node_id);
	if (find == outgoingArcs.end()) {
		return NULL;
	} else {
		return find->second;
	}
}

patGeoCoordinates patNode::getGeoCoord() const {
	return geoCoord;
}

double patNode::getLatitude() const {
	return getGeoCoord().latitudeInDegrees;
}
double patNode::getLongitude() const {
	return getGeoCoord().longitudeInDegrees;
}

void patNode::setTags(map<string, string>& tags) {
	m_tags = tags;
}

PlacemarkPtr patNode::getKML() const {
	// Create <coordinates>.

	KmlFactory* factory = KmlFactory::GetFactory();
	CoordinatesPtr coordinates = factory->CreateCoordinates();
	// Create <coordinates>-122.0816695,37.42052549<coordinates>
	coordinates->add_latlng(geoCoord.latitudeInDegrees,
			geoCoord.longitudeInDegrees);

	// Create <Point> and give it <coordinates>.
	PointPtr point = factory->CreatePoint();
	point->set_coordinates(coordinates); // point takes ownership
	PlacemarkPtr placemark = factory->CreatePlacemark();
	// Create <Placemark> and give it a <name> and the <Point>.
	stringstream desc;
	desc << *this;
	placemark->set_styleurl("#gps");
	placemark->set_name(name);
	placemark->set_description(getTagString());
	placemark->set_geometry(point); // placemark takes ownership
	return placemark;
}

string patNode::getTag(string tag_key) const {
	map<string, string>::const_iterator find_tag_key = m_tags.find(tag_key);
	if (find_tag_key == m_tags.end()) {
		return "";
	} else {
		return find_tag_key->second;
	}
}
map<string, string> patNode::getTags() const {
	return m_tags;
}

void patNode::setTag(string key, string value) {
	m_tags[key] = value;
}

double patNode::calHeading(const patNode* b_node) const {

	patGeoCoordinates startCoord = getGeoCoord();
	patGeoCoordinates nextCoord = b_node->getGeoCoord();
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
	return theArcHeading;
}

bool patNode::hasTrafficSignal() const {
	map<string, string>::const_iterator find_signal = m_tags.find("highway");
	if (find_signal != m_tags.end()
			&& find_signal->second == "traffic_signals") {
		return true;
	} else {
		return false;
	}
}
