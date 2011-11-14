/*
 * patNetworkTrain.cc
 *
 *  Created on: Oct 11, 2011
 *      Author: jchen
 */

#include "patNetworkTrain.h"

patNetworkTrain::patNetworkTrain() {


}

void patNetworkBus::getFromNetwork(patNetworkElements* network){

	getRoutes(network,"train_routes_relations");
}
patNetworkTrain::~patNetworkTrain() {
}

