/*
 * patNetworkMetro.cc
 *
 *  Created on: Mar 14, 2012
 *      Author: jchen
 */

#include "patNetworkMetro.h"

#include "patNBParameters.h"
patNetworkMetro::patNetworkMetro(){
	m_transport_mode=TransportMode(METRO);
}
void patNetworkMetro::getFromNetwork(patNetworkElements* network,patGeoBoundingBox bounding_box){

	getRoutes(network,"metro_routes_relations",bounding_box);
}

patNetworkMetro::~patNetworkMetro() {
	//
}

double patNetworkMetro::getMinSpeed() const{
	return patNBParameters::the()->metroNetworkMinSpeed;
}
double patNetworkMetro::getMaxSpeed() const{

	return patNBParameters::the()->metroNetworkMaxSpeed;
}
