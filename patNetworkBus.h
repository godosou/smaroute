/*
 * patNetworkBus.h
 *
 *  Created on: Jul 22, 2011
 *      Author: jchen
 */

#ifndef PATNETWORKBUS_H_
#define PATNETWORKBUS_H_

#include "patNetworkPublicTransport.h"
#include "patPublicTransportSegment.h"
class patNetworkBus: public patNetworkPublicTransport {
public:
	patNetworkBus();
	/**
	 * Method for getting routes from patNetworkElements::patNetworkElements() and postgresql database;
	 * @see patNetworkPublicTransport::getRoutes.
	 * @see patNetworkTrain::getRoutes.
	 */
	void getRoutes(patNetworkElements* network);
	virtual ~patNetworkBus();
};

#endif /* PATNETWORKBUS_H_ */
