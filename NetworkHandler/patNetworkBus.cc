/*
 * patNetworkBus.cc
 *
 *  Created on: Jul 22, 2011
 *      Author: jchen
 */

#include "patNetworkBus.h"

patNetworkBus::patNetworkBus() {
	//

}

void patNetworkBus::getFromNetwork(patNetworkElements* network){

	getRoutes(network,"pt_routes_relations");
}

patNetworkBus::~patNetworkBus() {
	//
}

