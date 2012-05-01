/*
 * patNetworkTrain.cc
 *
 *  Created on: Oct 11, 2011
 *      Author: jchen
 */

#include "patNetworkTrain.h"

#include "patNBParameters.h"
patNetworkTrain::patNetworkTrain() {

		m_transport_mode=TransportMode(TRAIN);

}

void patNetworkTrain::getFromNetwork(patNetworkElements* network,patGeoBoundingBox bounding_box){

	getRoutes(network,"train_routes_relations",bounding_box);
}
patNetworkTrain::~patNetworkTrain() {
}


double patNetworkTrain::getMinSpeed() const{
	return patNBParameters::the()->trainNetworkMinSpeed;
}
double patNetworkTrain::getMaxSpeed() const{

	return patNBParameters::the()->trainNetworkMaxSpeed;
}
