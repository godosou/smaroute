/*
 * patNetworkBus.h
 *
 *  Created on: Jul 22, 2011
 *      Author: jchen
 */

#ifndef PATNETWORKBUS_H_
#define PATNETWORKBUS_H_

#include "patGeoBoundingBox.h"
#include "patNetworkPublicTransport.h"
class patNetworkBus: public patNetworkPublicTransport {
public:
	patNetworkBus();

	void getFromNetwork(patNetworkElements* network,patGeoBoundingBox bounding_box);
	virtual ~patNetworkBus();
	 double getMinSpeed() const;
	 double getMaxSpeed() const;
};

#endif /* PATNETWORKBUS_H_ */
