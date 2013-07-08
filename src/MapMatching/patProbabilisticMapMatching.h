/*
 * patProbabilisticMapMatching.h
 *
 *  Created on: Nov 10, 2011
 *      Author: jchen
 */

#ifndef PATPROBABILISTICMAPMATCHING_H_
#define PATPROBABILISTICMAPMATCHING_H_
#include "patGpsSequence.h"
#include "patArcTransition.h"
#include "patGpsPoint.h"
#include "patError.h"
#include "patMultiModalPathMatching.h"
#include "patNetworkEnvironment.h"

#include <map>
#include <set>
#include <vector>
#include "patMeasurement.h"
#include "patObservations.h"
#include<tr1/unordered_map>
using namespace std::tr1;

class patProbabilisticMapMatching {
public:

	/**
	 * Initiator: initiate gps seuqence
	 * @param gps_fiel_name the path to the gps file
	 * @para err error handler
	 */
	patProbabilisticMapMatching(const patNetworkEnvironment* environment,string gps_file_name, patError*& err);
	/*
	 * Algorithm:
	 * Loop over all GPS point, ignore empty GPS point.
	 */
	/**
	 * Run the algorithm
	 */

	void setNetworkEnvironment(const patNetworkEnvironment* environment);
	void run(patError*& err);
	virtual ~patProbabilisticMapMatching();
protected:
	patObservations m_observations;
	string m_file_name; //The gps file path
	vector<patMeasurement*> m_running_measurement_sequence;//The sequence up to the GPS that is being processed

	map<patMultiModalPathMatching, double> m_paths;//The map matched paths
	set<patOd> m_od_set;//The generated od set.
	const patNetworkEnvironment* m_environment;
};

#endif /* PATPROBABILISTICMAPMATCHING_H_ */
