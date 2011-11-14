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
#include "patNetworkTrain.h"
#include "patNetworkElements.h"
class patNetworks {
public:
	patNetworks();
	void readNetworks(patNetworkElements* network_elements);
	virtual ~patNetworks();
protected:
patNetworkCar car_network;
patNetworkBus bus_network;
patNetworkTrain train_network;
};

#endif /* PATNETWORKS_H_ */
