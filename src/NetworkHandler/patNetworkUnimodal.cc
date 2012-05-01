/*
 * patNetworkV2.cc
 *
 *  Created on: Jul 20, 2011
 *      Author: jchen
 */

#include "patNetworkUnimodal.h"
#include "patType.h"
#include "patWay.h"
#include "patNode.h"
#include "patDisplay.h"
#include "patArc.h"

using namespace std;

patNetworkUnimodal::patNetworkUnimodal() {

}

bool patNetworkUnimodal::addWay(const patWay* the_way, bool reverse) {
	const vector<const patArc*>* list_of_arcs = the_way->getArcListPointer(
			!reverse);
	for (vector<const patArc*>::const_iterator arc_iter = list_of_arcs->begin();
			arc_iter != list_of_arcs->end(); ++arc_iter) {
		const patArc* the_arc = (*arc_iter);
		const patNode* up_node = the_arc->getUpNode();
		map<const patNode*, set<const patRoadBase*> >::iterator find_up_node =
				m_outgoing_incidents.find(up_node);
		if (find_up_node == m_outgoing_incidents.end()) {
			set<const patRoadBase*> outgoing_arcs_set;
			outgoing_arcs_set.insert(the_arc);
			m_outgoing_incidents.insert(
					pair<const patNode*, set<const patRoadBase*> >(up_node,
							outgoing_arcs_set));

		} else {
			find_up_node->second.insert(the_arc);
		}
	}
	return true;
}

patNetworkUnimodal::~patNetworkUnimodal() {

}

const patRoadBase* patNetworkUnimodal::findArc(
		const patArc* const an_arc) const {
	map<const patNode*, set<const patRoadBase*> >::const_iterator find_up_node =
			m_outgoing_incidents.find(an_arc->getUpNode());
	if (find_up_node != m_outgoing_incidents.end()) {
		for (set<const patRoadBase*>::const_iterator road_iter =
				find_up_node->second.begin();
				road_iter != find_up_node->second.end(); ++road_iter) {
			if ((*road_iter)->getDownNode() == an_arc->getDownNode()) {
				return *road_iter;
			}
		}
	}
	return NULL;
}

set<const patRoadBase*> patNetworkUnimodal::getRoadsContainArc(
		const patRoadBase* arc) const {
	set<const patRoadBase*> roads;
	map<const patNode*, set<const patRoadBase*> >::const_iterator find_up_node =
			m_outgoing_incidents.find(arc->getUpNode());

	if (find_up_node == m_outgoing_incidents.end()) {
		return roads;
	}
	set<const patRoadBase*>::const_iterator find_arc =
			find_up_node->second.find(arc);
	if (find_arc == find_up_node->second.end()) {
		return roads;
	} else {
		roads.insert(*find_arc);
	}
	return roads;

}
