/*
 * patVirtualNetwork.h
 *
 *  Created on: Oct 28, 2011
 *      Author: jchen
 */

#ifndef PATVIRTUALNETWORK_H_
#define PATVIRTUALNETWORK_H_
#include "patNetworkUnimodal.h"
#include "patArc.h"
#include <map>
#include "patNode.h"
class patVirtualNetwork: public patNetworkUnimodal {
public:
	patVirtualNetwork();
	virtual ~patVirtualNetwork();
protected:
	map<unsigned long, patArc> virtual_arcs;
	map<patNode*, set<patArc*> > virtual_links;

};

#endif /* PATVIRTUALNETWORK_H_ */
