/*
 * patNetworkBus.cc
 *
 *  Created on: Jul 22, 2011
 *      Author: jchen
 */

#include "patNetworkBus.h"


#include "patNBParameters.h"
patNetworkBus::patNetworkBus(){
	m_transport_mode=TransportMode(BUS);
}
void patNetworkBus::getFromNetwork(patNetworkElements* network,patGeoBoundingBox bounding_box){

	getRoutes(network,"bus_routes_relations",bounding_box);
}

patNetworkBus::~patNetworkBus() {
	//
}

double patNetworkBus::getMinSpeed() const{
	return patNBParameters::the()->busNetworkMinSpeed;
}
double patNetworkBus::getMaxSpeed() const{

	return patNBParameters::the()->busNetworkMaxSpeed;
}
