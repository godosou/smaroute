/*
 * osm2kml.cc
 *
 *  Created on: Jul 11, 2012
 *      Author: jchen
 */

#include "patNBParameters.h"
#include "patDisplay.h"
#include "initParams.h"
#include "initNetworks.h"
#include "patNetworkEnvironment.h"
#include "patNetworkBase.h"
#include <vector>
#include <map>
using namespace std;
int main(int argc, char *argv[]) {

	DEBUG_MESSAGE("STARTED..")

	initParameters(argv[1]);

	patNetworkEnvironment network_environment = initiateNetworks();

	const map<string, patNetworkBase*>* all_networks =
			network_environment.getAllNetworks();
	DEBUG_MESSAGE("FINISHED..");

}

