/*
 * patNetworkBase.h
 *
 *  Created on: Nov 1, 2011
 *      Author: jchen
 */

#ifndef PATNETWORKBASE_H_
#define PATNETWORKBASE_H_
#include <map>
#include <set>
#include "patNode.h"
#include "patRoadBase.h"
#include "patArc.h"
#include "patTransportMode.h"
class patNetworkBase {
public:
	patNetworkBase();
	virtual ~patNetworkBase();
	const map<patNode*,set<patRoadBase*> >* getOutgoingIncidents();
	bool isArcInNetwork(const patArc* an_arc);
	bool hasDownStream(const patNode* a_node);
	unsigned long getNodeSize();
	const patTransportMode getTransportMode();
	virtual void getFromNetwork(patNetworkElements* network)=0;
protected:

	map<patNode*,set<patRoadBase*> > outgoing_incidents;
	string network_type;
	patTransportMode transport_mode;
};

#endif /* PATNETWORKBASE_H_ */
