/*
 * patSimulateProbabilisticPaths.h
 *
 *  Created on: Jun 8, 2012
 *      Author: jchen
 */

#ifndef PATSIMULATEPROBABILISTICPATHS_H_
#define PATSIMULATEPROBABILISTICPATHS_H_
#include "MHPoints.h"
#include <map>
using namespace std;
class patMultiModalPath;
class patRouter;
class patRandomNumber;
class patNetworkBase;

class patSimulateProbabilisticPaths {
public:
	patSimulateProbabilisticPaths(const patNetworkBase* network,
			const patRouter* router, const patRandomNumber* rnd);
	MHPoints drawPoints(int n) const;
	map<patMultiModalPath, double> run(const patMultiModalPath& path,
			const unsigned int& count, const double& error) const;
	virtual ~patSimulateProbabilisticPaths();
protected:
	const double m_distance_scale;
	const double m_obs_error_distance;
	const patNetworkBase* m_network;
	const patRouter* m_router;
	const patRandomNumber* m_rnd;
};

#endif /* PATSIMULATEPROBABILISTICPATHS_H_ */
