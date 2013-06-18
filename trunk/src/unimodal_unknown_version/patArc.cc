//----------------------------------------------------------------
// File: patArc.cc
// Author: Michel Bierlaire, Jingmin Chen
// Creation: Thu Oct 30 10:53:34 2008
//----------------------------------------------------------------

#include "patArc.h"
#include "patNode.h"
#include "patError.h"

#include "patDisplay.h"
#include "patConst.h"
#include "patErrNullPointer.h"
#include "patNetwork.h"
#include "patGeoCoordinates.h"
#include "patNBParameters.h"

patArc::patArc(unsigned long theId, patNode* theUpNode, patNode* theDownNode,
		patString theName, struct arc_attributes theAttr, patError*& err) :
		m_user_id(theId), internalId(patBadId), m_name(theName), m_attributes(theAttr), m_up_node(
				theUpNode), m_down_node(theDownNode), generalizedCost(0.0) {

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

patArc::patArc(unsigned long theId, patNode* theUpNode, patNode* theDownNode) :
		m_user_id(theId), m_up_node(theUpNode), m_down_node(theDownNode), generalizedCost(
				0.0) {
	m_length = theUpNode->geoCoord.distanceTo(theDownNode->geoCoord);
	calHeading();
}
patArc::patArc(unsigned long theId, patNode* theUpNode, patNode* theDownNode,
		patString theName, patError*& err) :
		m_user_id(theId), internalId(patBadId), m_name(theName), m_up_node(theUpNode), m_down_node(
				theDownNode) {
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
void patArc::setLength(double l) {
	m_length = l;
	generalizedCost = l;
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
double patArc::calHeading(patNetwork* theNetwork, patError*& err) {

	patNode* theUpNode = theNetwork->getNodeFromUserId(m_up_node_id);
	patNode* theDownNode = theNetwork->getNodeFromUserId(m_down_node_id);
	if ((theUpNode == NULL) || (theDownNode == NULL)) {

		err = new patErrNullPointer("patNode");
		DEBUG_MESSAGE(
				"node" << m_up_node_id << "," << theUpNode << "-" << m_down_node_id
						<< "," << theDownNode);
		WARNING(err->describe());
		return -1.0;
	}

	patGeoCoordinates startCoord = theUpNode->geoCoord;
	patGeoCoordinates nextCoord = theDownNode->geoCoord;
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
	str << "Arc " << x.m_user_id << " [" << x.m_name << "](" << x.m_up_node->getUserId()
			<< "->" << x.m_down_node->getUserId() << ")";
	str << " length=" << m_length << "m; ";
	return str;
}
unsigned long patArc::computeTurn(patArc* downArc, patNetwork* theNetwork) {
	patNode* intersection = theNetwork->getNodeFromUserId(m_down_node_id);
	if (intersection == NULL) {
		return 100;
	}
	if (intersection->userId != downArc->m_up_node_id) {
		return 100;
	}

	set < unsigned long > *successors = &(intersection->userSuccessors);
	map<patArc*, double> downHeading;
	unsigned long left = 0;
	unsigned long right = 0;
	double headingChange = 180.0
			- (downArc->m_attributes.heading - m_attributes.heading);
	headingChange =
			(headingChange > 360.0) ? (headingChange - 360.0) : headingChange;
	headingChange =
			(headingChange < 0.0) ? (headingChange + 360.0) : headingChange;

	for (set<unsigned long>::iterator iter1 = successors->begin();
			iter1 != successors->end(); ++iter1) {

		patArc* downStream = theNetwork->getArcFromNodesUserId(
				intersection->userId, *iter1);
		if (downStream == NULL || downStream == downArc) {
			continue;
		}
		double h1 = 180.0
				- (downStream->m_attributes.heading - m_attributes.heading);
		h1 = (h1 > 360.0) ? (h1 - 360.0) : h1;
		h1 = (h1 < 0.0) ? (h1 + 360.0) : h1;

		if (fabs(h1) < 10.0) {
			continue;
		}

		if (h1 > headingChange) {
			right += 1;
		}
		if (h1 < headingChange) {
			left += 1;
		}
	}
	if (left + right == 0) {
		return 0;
	} else if (left + right == 1) {
		if (left == 1) {
			if (headingChange >= 215.0) {
				return 3; //right turn
			} else {
				return 2;
			}

		} else if (right == 1) {
			if (headingChange <= 145.0) {
				return 1; //left turn
			} else {
				return 2; //
			}

		}

	} else if (left + right >= 2) {
		if (right == 0) {
			return 3;
		} else if (right >= 1 && left >= 1) {
			return 2;
		} else {
			return 1;
		}
	}
}

patNode* patArc::getUpNode() const {
	return m_up_node;
}
patNode* patArc::getDownNode() const {
	return m_down_node;
}

list<patArc*> patArc::getArcList() const{

	list<patArc*> rtn;
	rtn.push_back(this);
	return rtn;
}
