/*


 * simulateObservations.cc
 *
 *  Created on: Jul 5, 2012
 *      Author: jchen
 */
#include "patException.h"
#include "patError.h"
#include "patNBParameters.h"
#include "patNetworkEnvironment.h"
#include "patNode.h"
#include "MHPathGenerator.h"
#include "patNBParameters.h"
#include "patObservationWritter.h"
#include "patDisplay.h"
#include "patNetworkCar.h"
#include "patWay.h"
#include "MHObservationWritterWrapper.h"
int main(int argc, char *argv[]) {

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

	const patNode* origin = network_environment.getNetworkElements().getNode(
			patNBParameters::the()->OriginId);
	const patNode* destination =
			network_environment.getNetworkElements().getNode(
					patNBParameters::the()->DestinationId);
	if (origin == NULL || destination == NULL) {
		WARNING("od not specified");
		exit(-1);
	}
	string folder = patNBParameters::the()->observationDirectory;

	patLinkAndPathCost router_link_cost(
			patNBParameters::the()->router_cost_link_scale,
			patNBParameters::the()->router_cost_length_coef, 0.0,
			patNBParameters::the()->router_cost_sb_coef);

	map<ARC_ATTRIBUTES_TYPES, double> link_coef;
	link_coef[ENUM_LENGTH] = patNBParameters::the()->utility_length_coef;
	link_coef[ENUM_TRAFFIC_SIGNAL] = patNBParameters::the()->utility_sb_coef;

	map<const patMultiModalPath, double> ps;
	MHWeightFunction mh_weight(link_coef,
			patNBParameters::the()->utility_link_scale,
			patNBParameters::the()->utility_ps_coef, 0.0);
	mh_weight.setPathProbas(NULL);

	MHPathGenerator path_generator(patNBParameters::the()->randomSeed);
	path_generator.setRouterLinkCost(&router_link_cost);
	path_generator.setNetwork(network_environment.getNetwork(CAR));

	path_generator.setMHWeight(&mh_weight);

	MHObservationWritterWrapper path_writer(folder,patNBParameters::the()->SAMPLEINTERVAL_ELEMENT);
	path_generator.setWritterWrapper(&path_writer);
	path_generator.run(origin, destination);

}
