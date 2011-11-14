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
patBoolean patNetworkUnimodal::addWay(const patWay* the_way,
		patBoolean reverse) {
	const list<patArc*>* list_of_arcs;
	list_of_arcs = the_way->getListOfArcs(!reverse);
	for (list<patArc*>::const_iterator arc_iter = list_of_arcs->begin();
			arc_iter != list_of_arcs->end();
			++arc_iter) {
		patArc* the_arc = (*arc_iter);
		patNode* up_node = the_arc->getUpNode();
		//	map<patNode*,vector<patArc*>> outgoing_arcs;
		map<patNode*, set<patArc*> >::iterator find_up_node =outgoing_incidents.find(up_node);
		if (find_up_node == outgoing_incidents.end()) {
			set<patArc*> outgoing_arcs_set;
			outgoing_arcs_set.insert(the_arc);
			outgoing_incidents.insert(pair<patNode*, set<patArc*> >(up_node, outgoing_arcs_set));

		}
		else{
			find_up_node->second.insert(the_arc);
		}
	}
	return patTRUE;
}

patNetworkUnimodal::~patNetworkUnimodal() {

}


