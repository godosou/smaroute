/*
 * patNetworkV2.h
 *
 *  Created on: Jul 20, 2011
 *      Author: jchen
 */

#ifndef PATNETWORKUNIMODAL_H_
#define PATNETWORKUNIMODAL_H_
#include <set>
#include <map>
#include "patArc.h"
#include "patWay.h"
#include "patType.h"
#include "patNetworkElements.h"
using namespace std;

class patNetworkUnimodal {
public:
	patNetworkUnimodal();
	virtual void getFromNetwork(patNetworkElements* network)=0;
	patBoolean addWay(const patWay*  the_way,patBoolean reverse);
	patULong size();
	virtual ~patNetworkUnimodal();
protected:

	map<patNode*,set<patArc*> > outgoing_arcs;

};

#endif /* PATNETWORKV2_H_ */
