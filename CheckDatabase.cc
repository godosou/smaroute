
/*
 * CheckDatabase.cc
 *
 *  Created on: Jul 20, 2011
 *      Author: jchen
 */



#include "patPostGreSQLConnector.h"
#include "patGeoBoundingBox.h"
#include "patDisplay.h"
#include "patNetworkElements.h"
#include "patNetworkUnimodal.h"
#include "patNetworkCar.h"
#include "patError.h"
int main(int argc, char *argv[]) {

	patGeoBoundingBox bb = patGeoBoundingBox( 6.7454, 46.5192, 6.7686, 46.4830);
	//DEBUG_MESSAGE(bb.toString());
	 patWay::initiateNetworkTypeRules();
	patNetworkElements network;
	 patError* err(NULL) ;
	 patPostGreSQLConnector db_conn= patPostGreSQLConnector();
	 network.readNetworkFromPostGreSQL(bb,err);
	 DEBUG_MESSAGE("Network topology has been read;");
	 patNetworkCar carNetwork = patNetworkCar();
	 carNetwork.getFromNetwork(&network);
	 DEBUG_MESSAGE("Car network size"<<carNetwork.size());
	DEBUG_MESSAGE("finished");
	return 1;
}
