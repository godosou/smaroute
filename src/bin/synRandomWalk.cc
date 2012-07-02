/*
 * synRandomWalk.cc
 *
 *  Created on: Jun 26, 2012
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
#include "RWPathGenerator.h"
#include "patSimulation.h"
#include "patSampleChoiceSetWithObservations.h"
#include "MHWeightFunction.h"
#include "patReadPathsFromKML.h"
#include <boost/lexical_cast.hpp>
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
	patLinkAndPathCost router_link_cost(patNBParameters::the()->1.0,
			-patNBParameters::the()->1.0, 0.0,
			patNBParameters::the()->1.0);

	vector<patObservation> observations;
	for (unsigned i = 0; i < 1000; ++i) {
		string obs_file_name = patNBParameters::the()->observationDirectory
				+ "observations/" + boost::lexical_cast < string > (i) + ".kml";

		patObservation new_observation;
		new_observation.setId(boost::lexical_cast < string > (i));
		if (!ifstream(obs_file_name.c_str())) {
			WARNING(obs_file_name);
			throw RuntimeException("no valid observation file");
		}
		patReadPathsFromKML rp;

		vector<patMultiModalPath> obs_paths = rp.read(
				&network_environment.getNetworkElements(), obs_file_name);
		if (obs_paths.size() != 1) {
			WARNING("WRONG PATH NUMBER"<<obs_paths.size());
		}
		new_observation.addPath(obs_paths.front(), 1.0);
		observations.push_back(new_observation);

	}

	RWPathGenerator path_generator(patNBParameters::the()->randomSeed);
	DEBUG_MESSAGE("OK");
	path_generator.setRouterLinkCost(&router_link_cost);
	path_generator.setMHWeight(&mh_weight);

	path_generator.setNetwork(network_environment.getNetwork(CAR));

	DEBUG_MESSAGE("START TO DEAL WITH OBSERVATIONS"<<observations.size());
	patSampleChoiceSetWithObservations sco;
	for (int i = 0; i < observations.size(); ++i) {
		sco.addObservation(observations[i]);
//		break;
	}DEBUG_MESSAGE("START TO SAMPLE CHOICE SET");
	sco.sampleChoiceSet(&path_generator, folder);
}
