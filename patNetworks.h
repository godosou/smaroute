/*
 * patNetworks.h
 *
 *  Created on: Aug 5, 2011
 *      Author: jchen
 */

#ifndef PATNETWORKS_H_
#define PATNETWORKS_H_
#include "patNetworkCar.h"
#include "patNetworkBus.h"
class patNetworks {
public:
	patNetworks();
	virtual ~patNetworks();
protected:
patNetworkCar car_network;
patNetworkBus bus_network;
};

#endif /* PATNETWORKS_H_ */
