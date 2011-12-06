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
#include "patMultiModalPath.h"
#include "patNetworkEnvironment.h"
#include "patOdJ.h"
#include <map>
#include <set>
#include <vector>
#include "patMeasurement.h"
#include "patObservations.h"
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
	void run(patError*& err);
	virtual ~patProbabilisticMapMatching();
protected:
	patObservations m_observations;
	string m_file_name; //The gps file path
	vector<patMeasurement*> m_running_measurement_sequence;//The sequence up to the GPS that is being processed
	set<patArcTransition> m_arc_tran_set;//The arc transition set for the map matching (cache purpose)
	map<patMultiModalPath, double> m_paths;//The map matched paths
	set<patOdJ> m_od_set;//The generated od set.
	const patNetworkEnvironment* m_environment;
};

#endif /* PATPROBABILISTICMAPMATCHING_H_ */
