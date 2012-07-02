/*
 * simulatePath.cc
 *
 *  Created on: May 25, 2012
 *      Author: jchen
 */

/*
 * pathGeneration.cc
 *
 *  Created on: May 3, 2012
 *      Author: jchen
 */

#include "patException.h"
#include "patError.h"
#include "patNBParameters.h"
#include "patEnumeratePaths.h"
#include "patKMLPathWriter.h"
#include "patDisplay.h"
#include "patNetworkCar.h"
#include "patUtilityFunction.h"
#include "patSimulation.h"
#include "patLinkAndPathCost.h"
#include "patRouter.h"
#include "patNetworkBase.h"
int main(int argc, char *argv[]) {

	DEBUG_MESSAGE("STARTED..")

	/*
	 * (1) Extract single command line parameter.
	 */
	patError* err(NULL);

	//Read parameters
	if (argc == 1) {
		patNBParameters::the()->readFile(
				"../../playground/Synthetic/params/config.xml", err);

	} else {
		patNBParameters::the()->readFile(argv[1], err);

	}
	patNBParameters::the()->init(err);
	if (err != NULL) {
		exit(-1);
	}
	patGeoBoundingBox bb = patGeoBoundingBox(-100, 100, 100, -100);

	patNetworkEnvironment network_environment(bb, err);
	//Load networks
	if (err != NULL) {
		exit(-1);
	}


	patUtilityFunction utility_function(patNBParameters::the()->utility_link_scale,
			patNBParameters::the()->utility_length_coef,
			patNBParameters::the()->utility_ps_coef);

	patSimulation sim(&network_environment, &utility_function,
			patNBParameters::the()->randomSeed);

	patLinkAndPathCost router_link_cost(
			patNBParameters::the()->router_cost_link_scale,
			patNBParameters::the()->router_cost_length_coef, 0.0,patNBParameters::the()->router_cost_sb_coef);

	patRouter simulate_router(network_environment.getNetwork(CAR), &router_link_cost);

	sim.run(patNBParameters::the()->observationDirectory,&simulate_router);
	DEBUG_MESSAGE("..DONE");


}
