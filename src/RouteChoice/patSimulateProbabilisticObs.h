/*
 * patSimulateProbabilisticObs.h
 *
 *  Created on: Jul 27, 2012
 *      Author: jchen
 */

#ifndef PATSIMULATEPROBABILISTICOBS_H_
#define PATSIMULATEPROBABILISTICOBS_H_
#include "patObservation.h"
#include "patSimulateProbabilisticPaths.h"

class patNetworkBase;
class patRouter;
class patRandomNumber;

class patSimulateProbabilisticObs {
public:
	patSimulateProbabilisticObs(
			const patNetworkBase* network,
			const patRouter* router,
			const patRandomNumber* rnd);
	void run( patObservation& observation ) const;
	virtual ~patSimulateProbabilisticObs();
protected:
	const patNetworkBase* m_network;
	const patRouter* m_router;
	const patRandomNumber* m_rnd;

};

#endif /* PATSIMULATEPROBABILISTICOBS_H_ */
