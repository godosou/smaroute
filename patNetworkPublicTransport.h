/*
 * patNetworkPublicTransport.h
 *
 *  Created on: Jul 22, 2011
 *      Author: jchen
 */

#ifndef PATNETWORKPUBLICTRANSPORT_H_
#define PATNETWORKPUBLICTRANSPORT_H_
#include "patWay.h"
#include "patNode.h"
#include <list>
#include <map>
#include "patPublicTransportSegment.h"


/**
 * Class for public transport routes. Abstract class for train and bus routes
 */
class patNetworkPublicTransport {
public:
	patNetworkPublicTransport();

	/**
	 * Virtual method for getting routes from patNetworkElements::patNetworkElements() and postgresql database;
	 */
	virtual void getRoutes(patNetworkElements* network)=0;
	void addPTSegment(patNode* up_node,patPublicTransportSegment the_seg);
	virtual ~patNetworkPublicTransport();
protected:
	map<patNode*,list<patPublicTransportSegment> > network;
};

#endif /* PATNETWORKPUBLICTRANSPORT_H_ */
