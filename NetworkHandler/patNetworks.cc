/*
 * patNetworks.cc
 *
 *  Created on: Aug 5, 2011
 *      Author: jchen
 */

#include "patNetworks.h"

patNetworks::patNetworks() :
car_network(),
train_network(),
bus_network()
{

}

patNetworks::~patNetworks() {
	//
}


void patNetworks::readNetworks(patNetworkElements* network_elements){
	car_network.getFromNetwork(network_elements);
	bus_network.getRoutes(network_elements);
	train_network.getRoutes(network_elements);
}
