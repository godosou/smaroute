/*
 * synTestPathGeneration.cc
 *
 *  Created on: Jun 7, 2012
 *      Author: jchen
 */



/*
 * synPathGeneration.cc
 *
 *  Created on: May 26, 2012
 *      Author: jchen
 */

#include "patException.h"
#include "patError.h"
#include "patNBParameters.h"
#include "patEnumeratePaths.h"
#include "patKMLPathWriter.h"
#include "patDisplay.h"
#include "patNetworkCar.h"
#include "patLinkAndPathCost.h"
#include "MHWeightFunction.h"
#include "MHPathGenerator.h"
#include "patSimulation.h"
#include "patSampleChoiceSetWithObservations.h"
#include "MHWeightFunction.h"
#include "patReadPathsFromKML.h"
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

	string folder = patNBParameters::the()->observationDirectory;

	string obsrvation_file = folder + "observations.kml";
	if (!ifstream(obsrvation_file.c_str())) {
		throw RuntimeException("no observation file found");
	}
	patLinkAndPathCost router_link_cost(
			patNBParameters::the()->router_cost_link_scale,
			patNBParameters::the()->router_cost_length_coef, 0.0,patNBParameters::the()->router_cost_sb_coef);

	MHWeightFunction mh_weight(patNBParameters::the()->mh_link_scale,
			patNBParameters::the()->mh_length_coef, 0.0,
			patNBParameters::the()->mh_obs_scale);

	vector<patMultiModalPath> paths = patReadPathsFromKML::read(
			&(network_environment.getNetworkElements()), obsrvation_file);
	MHPathGenerator path_generator(patNBParameters::the()->randomSeed);
	path_generator.setRouterLinkCost(&router_link_cost);
	path_generator.setMHWeight(&mh_weight);

	path_generator.setNetwork(network_environment.getNetwork(CAR));

	patSampleChoiceSetWithObservations sco;
	DEBUG_MESSAGE(paths.size());
	for (vector<patMultiModalPath>::const_iterator path_iter = paths.begin();
			path_iter != paths.end(); ++path_iter) {
		patObservation new_obs;
		new_obs.addPath(*path_iter, 1.0);
		sco.addObservation(new_obs);
		break;
	}
	sco.sampleChoiceSet(&path_generator, folder);
}

