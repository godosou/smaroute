/*
 * patNetworkBus.h
 *
 *  Created on: Jul 22, 2011
 *      Author: jchen
 */

#ifndef PATNETWORKBUS_H_
#define PATNETWORKBUS_H_

#include "patNetworkPublicTransport.h"
class patNetworkBus: public patNetworkPublicTransport {
public:
	patNetworkBus();

	void getFromNetwork(patNetworkElements* network);
	virtual ~patNetworkBus();
};

#endif /* PATNETWORKBUS_H_ */
