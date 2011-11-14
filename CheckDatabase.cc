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
#include "patPathJ.h"
#include "patTripParser.h"
#include "patReadPathFromShp.h"
#include "patCalculateProbaForPaths.h"
#include "patNBParameters.h"
int main(int argc, char *argv[]) {
	patString gps_file="data/5451/5451.csv";

	patGeoBoundingBox bb = patGeoBoundingBox(6.56, 46.53, 6.58, 46.51);
	//DEBUG_MESSAGE(bb.toString());
	patWay::initiateNetworkTypeRules();
	patNetworkElements network;
	patError* err(NULL);
	patNBParameters::the()->readFile(argv[1],err);
	patNBParameters::the()->init(err);

	patPostGreSQLConnector db_conn = patPostGreSQLConnector();
	network.readNetworkFromPostGreSQL(bb, err);
	DEBUG_MESSAGE("Network topology has been read;");
	patNetworkCar carNetwork = patNetworkCar();
	carNetwork.getFromNetwork(&network);
	DEBUG_MESSAGE("Car network size" << carNetwork.size());
	DEBUG_MESSAGE("finished");
	patTripParser theTrip=patTripParser();
	theTrip.readGPSFromFile(gps_file,err);

	set<patPathJ> a_path_set;
	patPathJ new_path = patPathJ();
	patReadPathFromShp().read(&new_path, string("network/path.dbf"), &network, err);
	new_path.detChangePoints();
	DEBUG_MESSAGE("path arcs" << new_path.nbrOfArcs());
	DEBUG_MESSAGE("path length" << new_path.getPathLength());
	DEBUG_MESSAGE("change points:" << new_path.getNbrOfChangePoints());
	DEBUG_MESSAGE(new_path);
	a_path_set.insert(new_path);

	DEBUG_MESSAGE("GPS POINTS: "<<  theTrip.getGpsSequence()->size());
	patCalculateProbaForPaths pathCalculation(NULL, theTrip.getGpsSequence());
	pathCalculation.fromPaths(a_path_set);
	vector<patReal> probas= pathCalculation.calculateProbas();
	for (vector<patReal>::iterator iter=probas.begin(); iter!=probas.end(); ++iter) {
		DEBUG_MESSAGE(*iter);
	}
	return 1;
}
