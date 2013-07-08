/*
 * patSimulation.h
 *
 *  Created on: May 25, 2012
 *      Author: jchen
 */

#ifndef PATSIMULATION_H_
#define PATSIMULATION_H_

#include "patOd.h"
#include "patPathGenerator.h"
#include "patMultiModalPath.h"
#include "patRandomNumber.h"
#include <vector>
using namespace std;
class patUtilityFunction;
class patRouter;
class patSimulation {
public:
	patSimulation(const patNetworkEnvironment* network_environment,patUtilityFunction* utility_function,int random_seed);
	void run(string result_folder, const patRouter* router);
	virtual ~patSimulation();
protected:
	patUtilityFunction* m_utility_function;
	const patRandomNumber m_rnd;
	const patNetworkEnvironment* m_network_environment;
};

#endif /* PATSIMULATION_H_ */
