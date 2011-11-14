/*
 * patNetworkBase.cc
 *
 *  Created on: Nov 1, 2011
 *      Author: jchen
 */

#include "patNetworkBase.h"

patNetworkBase::patNetworkBase() {

}

unsigned long patNetworkBase::getNodeSize() {
	return outgoing_incidents.size();
}
patNetworkBase::~patNetworkBase() {
}


const map<patNode*,set<patRoadBase*> >* patNetworkBase::getOutgoingIncidents(){
	return &outgoing_incidents;
}

bool patNetworkBase::isArcInNetwork(const patArc* an_arc){
	map<patNode*,set<patRoadBase*> >::const_iterator find_up_node = outgoing_incidents.find(an_arc->getUpNode);
	if(find_up_node!=outgoing_incidents.end()){
		if (find_up_node->second.find(an_arc)!=find_up_node->second.end()){
			return true;
		}
		else{
			return false;
		}
	}
	else{
		return false;
	}

}
