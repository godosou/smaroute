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
class patSimulateProbabilisticPaths {
public:
	patSimulateProbabilisticPaths(const patMultiModalPath& path,
			const patRouter* router);
	MHPoints drawPoints(int n);
	map<patMultiModalPath,double> run(unsigned int count, double error);
	virtual ~patSimulateProbabilisticPaths();
protected:
	const patMultiModalPath& m_path;
	const patRouter* m_router;
	const patRandomNumber* m_rnd;
};

#endif /* PATSIMULATEPROBABILISTICPATHS_H_ */
