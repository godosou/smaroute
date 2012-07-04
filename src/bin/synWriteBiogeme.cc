/*
 * synWriteBiogeme.cc
 *
 *  Created on: May 26, 2012
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
#include "MHWeightFunction.h"
#include "MHPathGenerator.h"
#include "patSimulation.h"
#include "patSampleChoiceSetWithObservations.h"
#include "patLinkAndPathCost.h"
#include "patReadPathsFromKML.h"
#include "patWriteBiogemeData.h"
#include "patUtilityFunction.h"
#include <boost/lexical_cast.hpp>
#include "patVerifyingSamplingResult.h"
#include "patReadChoiceSetFromKML.h"
#include <boost/lexical_cast.hpp>
#include "patRandomNumber.h"
#include "MHPathGenerator.h"
#include "RWPathGenerator.h"
#include "patComputePathSize.h"
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
	string sample_folder = folder + patNBParameters::the()->choiceSetFolder;

	patReadChoiceSetFromKML rc(&network_environment.getNetworkElements());

	string choiceset_file = folder + "universal_choice_set.kml";
	if (!ifstream(choiceset_file.c_str())) {
		exit(-1);
	}

	patRandomNumber rnd(patNBParameters::the()->randomSeed);
	map<patOd, patChoiceSet> od_choice_set = rc.read(choiceset_file, rnd);
//	DEBUG_MESSAGE(od_choice_set.size());
	patOd od = od_choice_set.begin()->first;
	patChoiceSet choice_set = od_choice_set.begin()->second;

	vector<patObservation> obs;
//	vsr.verifyProbability(choice_set, &mh_weight);
	patSampleChoiceSetWithObservations sco;
	int sampled = 0;
	int not_sampled = 0;
	vector<int> uppod;
	vector<pair<int, int> > od_paths;
	DEBUG_MESSAGE("read choice set: "<<choice_set.size());
	vector<patObservation> observations;
	for (unsigned i = 0; i < 1000; ++i) { //FIXME
		string obs_file_name = patNBParameters::the()->observationDirectory
				+ "observations/" + boost::lexical_cast<string>(i) + ".kml";

		patObservation new_observation;
		new_observation.setId(boost::lexical_cast<string>(i));
		if (!ifstream(obs_file_name.c_str())) {
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
	DEBUG_MESSAGE("Read all observations"<<observations.size());
	for (int i = 0; i < observations.size(); ++i) {

		if (argc == 3 && string(argv[2]) == "full") {
//			DEBUG_MESSAGE("universal choice set");
			observations[i].setChoiceSet(od_choice_set);
		} else {
			map<patOd, patChoiceSet> css;
			patReadChoiceSetFromKML rc(
					&network_environment.getNetworkElements());

			string sample_file = sample_folder + "/" + observations[i].getId()
					+ "_sample.kml";
			if (ifstream(sample_file.c_str())) {
				css = rc.read(sample_file, rnd);
			} else {
				unsigned file_index = 0;

				while (true) {
					++file_index;
					sample_file = sample_folder + "/" + observations[i].getId()
							+ "_" + boost::lexical_cast<string>(file_index)
							+ "_sample.kml";

					if (!ifstream(sample_file.c_str())) {
						break;
					}
					map<patOd, patChoiceSet> new_css = rc.read(sample_file,
							rnd);
					css.insert(new_css.begin(), new_css.end());
				}
			}
//			DEBUG_MESSAGE(css.size());
			if (css.size() == 0) {
				continue;
			}
			observations[i].setChoiceSet(css);
		}
		sco.addObservation(observations[i], false);
		pair<int, int> ccps = observations[i].countChosenPathsSampled();
		sampled += ccps.first;
		not_sampled += ccps.second;

		vector<int> new_uppod = observations[i].getUniquePathsPerOD();
		uppod.insert(uppod.end(), new_uppod.begin(), new_uppod.end());
		od_paths.push_back(
				pair<int, int>(observations[i].getNbOfOds(),
						observations[i].getNbrOfCandidates()));

	}
	patVerifyingSamplingResult vsr(sample_folder,
			&network_environment.getNetworkElements(), rnd);
	patUtilityFunction utility_function(
			patNBParameters::the()->utility_link_scale,
			patNBParameters::the()->utility_length_coef,
			patNBParameters::the()->utility_ps_coef);
	double vsr_chi2 = 0;
	if (patNBParameters::the()->pathSampleAlgorithm == "MH") {

		patLinkAndPathCost router_link_cost(
				patNBParameters::the()->router_cost_link_scale,
				patNBParameters::the()->router_cost_length_coef, 0.0,
				patNBParameters::the()->router_cost_sb_coef);

		map<ARC_ATTRIBUTES_TYPES, double> link_coef;
		link_coef[ENUM_LENGTH] = patNBParameters::the()->mh_length_coef;
		link_coef[ENUM_SPEED_BUMP] = patNBParameters::the()->mh_sb_coef;

		map<const patMultiModalPath, double> ps;
		MHWeightFunction mh_weight(link_coef,
				patNBParameters::the()->mh_link_scale,
				patNBParameters::the()->mh_ps_coef,
				patNBParameters::the()->mh_obs_scale);
		if (patNBParameters::the()->mh_ps_coef > 0.0) {
			DEBUG_MESSAGE("with path size specification");
			patComputePathSize ps_computer;
			ps = ps_computer.computePS(choice_set);
			mh_weight.setPathSize(&ps);
		}

		MHPathGenerator path_generator(patNBParameters::the()->randomSeed);
		path_generator.setRouterLinkCost(&router_link_cost);
		path_generator.setMHWeight(&mh_weight);

		path_generator.setNetwork(network_environment.getNetwork(CAR));
		if (patNBParameters::the()->samplingWithObs == 1) {
			sco.averagePathProbas();
			mh_weight.setPathProbas(&sco.getPathProbas());
		}
		vsr_chi2 = vsr.verifyProbability(choice_set, &path_generator);
		patWriteBiogemeData wbd(sco.getObs(), &utility_function,
				&path_generator, &choice_set, rnd);

		wbd.writeSampleFile(sample_folder);
	} else if (patNBParameters::the()->pathSampleAlgorithm == "RW") {
		patLinkAndPathCost router_link_cost(1.0, 1.0, 0.0, 0.0);
		RWPathGenerator path_generator(patNBParameters::the()->randomSeed,
				patNBParameters::the()->kumaA, patNBParameters::the()->kumaB,
				&router_link_cost);
		path_generator.setNetwork(network_environment.getNetwork(CAR));

		vsr_chi2 = vsr.verifyProbability(choice_set, &path_generator);
		patWriteBiogemeData wbd(sco.getObs(), &utility_function,
				&path_generator, &choice_set, rnd);

		wbd.writeSampleFile(sample_folder);
	}

	else {
		throw RuntimeException("Invalid sampling algorithm");
	}

	string scp_txt_fn = sample_folder + string("/sampledChosenPath_")
			+ boost::lexical_cast<string>(
					patNBParameters::the()->choiceSetInBiogemeData)
			+ string(".txt");
	ofstream sampleFile(scp_txt_fn.c_str());
	sampleFile << "chi2" << "," << vsr_chi2;
	sampleFile << "sampled" << "," << "not_sampled" << endl;
	sampleFile << sampled << "," << not_sampled << endl;

	sampleFile << "ods" << "," << "candidates" << endl;
	for (vector<pair<int, int> >::const_iterator u_iter = od_paths.begin();
			u_iter != od_paths.end(); ++u_iter) {
		sampleFile << u_iter->first << "," << u_iter->second << endl;
	}
	sampleFile << "unique paths" << endl;
	for (vector<int>::const_iterator u_iter = uppod.begin();
			u_iter != uppod.end(); ++u_iter) {
		sampleFile << *u_iter << endl;
	}
	sampleFile.close();

}
