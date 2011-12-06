/*
 * patPublicTransportSegment.cc
 *
 *  Created on: Jul 22, 2011
 *      Author: jchen
 */

#include "patPublicTransportSegment.h"
#include "patNetworkElements.h"
#include "patDisplay.h"
patPublicTransportSegment::patPublicTransportSegment() {
	m_length = -1.0;
}

patPublicTransportSegment::~patPublicTransportSegment() {
	//
}

ostream& operator<<(ostream& str, const patPublicTransportSegment& x) {
	list<const patArc*> arc_list = x.getArcList();
	for (list<const patArc*>::iterator arc_iter = arc_list.begin();
			arc_iter != arc_list.end(); ++arc_iter) {
		str << **arc_iter << "\n";
	}
}

 bool patPublicTransportSegment::addArcToBack(const patArc* arc,
		int direction) {
	if (m_directions.back() != 3 && direction != 3) {
		if (m_directions.back() != direction) {
			return false;
		}
	}
	if (patArcSequence::addArcToBack(arc) == false) {
		return false;
	}
	m_directions.push_back(direction);
	return true;
}
 const patArc* patPublicTransportSegment::deleteArcToBack() {

	if (m_arcs.empty() || m_directions.empty()) {
		return NULL;
	}
	const patArc* backArc = m_arcs.back();
	m_arcs.pop_back();
	m_directions.pop_back();
	return backArc;
}
