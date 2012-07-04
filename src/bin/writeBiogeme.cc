/*
 * writeBiogeme.cc
 *
 *  Created on: May 4, 2012
 *      Author: jchen
 */

#include "patException.h"
#include "patError.h"
#include "patNBParameters.h"
#include "patDisplay.h"
#include "patReadObservationFromKML.h"
#include "patObservation.h"
#include <dirent.h>
#include "patWay.h"
#include "patWriteBiogemeData.h"
#include "patObservation.h"
#include "MHPathGenerator.h"
#include "patReadChoiceSetFromKML.h"
#include "MHWeightFunction.h"
#include "patUtilityFunction.h"
#include "patRandomNumber.h"
#include "patLinkAndPathCost.h"
#include "patSampleChoiceSetWithObservations.h"
#include <boost/lexical_cast.hpp>
#include "patArc.h"
#include "RWPathGenerator.h"
int main(int argc, char *argv[]) {

	DEBUG_MESSAGE("STARTED..")

	/*
	 * (1) Extract single command line parameter.
	 */
	patError* err(NULL);

	//Read parameters
	if (argc == 1) {
		patNBParameters::the()->readFile(
				"/Users/jchen/Documents/Project/newbioroute/src/params/config.xml",
				err);

	} else {
		patNBParameters::the()->readFile(argv[1], err);

	}
	patNBParameters::the()->init(err);
	if (err != NULL) {
		exit(-1);
	}
	string observation_folder = patNBParameters::the()->observationDirectory
			+ "observations/";
	if (argc == 3) {
		patNBParameters::the()->choiceSetInBiogemeData = atoi(argv[2]);
	}
	DIR * dip;
	struct dirent *dit;
	const char* dir_name_char = observation_folder.c_str();

	DEBUG_MESSAGE("Try to open directory " << dir_name_char);
	if ((dip = opendir(dir_name_char)) == NULL) {
		stringstream str;
		str << "Directory " << dir_name_char
				<< " doesn't exist or no permission to read.";
		WARNING(str.str());
		exit(-1);
	}DEBUG_MESSAGE("Direcotry " << dir_name_char << " is now open");
	vector<string> observation_files;
	unsigned char isFile = 0x8;
	string esp("~");
	string kml("kml");

	while ((dit = readdir(dip)) != NULL) {
		if (dit->d_type == isFile) {
			//DEBUG_MESSAGE(dit->d_name);
			string fileName(dit->d_name);
			if (fileName.find(esp) == string::npos
					&& fileName.find(kml) != string::npos) {
				DEBUG_MESSAGE("Found file:" << fileName);
				observation_files.push_back(fileName);
			}

		}
	}

	closedir(dip);

	DEBUG_MESSAGE("nbr of observations" << observation_files.size());
	if (observation_files.empty()) {
		exit(0);
	}

	string network_file = patNBParameters::the()->OsmNetworkFileName;
	string network_folder = network_file.substr(0, network_file.rfind("/"));
	string network_rule = network_folder + "/network_rules.csv";
	DEBUG_MESSAGE("network rules file: "<<network_rule);
	patWay::initiateNetworkTypeRules(network_rule);
	patGeoBoundingBox bb = patGeoBoundingBox(
			patNBParameters::the()->boundingBoxLeftUpLongitude,
			patNBParameters::the()->boundingBoxLeftUpLatitude,
			patNBParameters::the()->boundingBoxRightBottumLongitude,
			patNBParameters::the()->boundingBoxRightBottumLatitude);

	//		patGeoBoundingBox bb = patGeoBoundingBox(6.49909428385,
	//				46.550856996900002, 6.5700872475999997, 46.505016372300001);

	patNetworkEnvironment network_environment(bb, err);
	//Load networks
	if (err != NULL) {
		exit(-1);
	}

	DEBUG_MESSAGE("network loaded");
	//MHLinkAndPathCost::configure();

	patRandomNumber rnd(patNBParameters::the()->randomSeed);

	patLinkAndPathCost router_link_cost(
			patNBParameters::the()->router_cost_link_scale,
			patNBParameters::the()->router_cost_length_coef, 0.0,
			patNBParameters::the()->router_cost_sb_coef);

	MHWeightFunction mh_weight(patNBParameters::the()->mh_link_scale,
			patNBParameters::the()->mh_length_coef, 0.0,
			patNBParameters::the()->mh_obs_scale);

	patUtilityFunction utility_function(
			patNBParameters::the()->utility_link_scale,
			patNBParameters::the()->utility_length_coef,
			patNBParameters::the()->utility_ps_coef);
	utility_function.deleteLinkCoefficient(ENUM_SPEED_BUMP);
	utility_function.setLinkCoefficient(ENUM_TRAFFIC_SIGNAL, -0.1);
	patSampleChoiceSetWithObservations sco;
	string sample_folder = patNBParameters::the()->observationDirectory
			+ patNBParameters::the()->choiceSetFolder;
	vector<patObservation> obs;
	int sampled = 0;
	int not_sampled = 0;
	vector<int> uppod;
	vector<pair<int, int> > od_paths;

	string choice_set_string = patNBParameters::the()->choiceSetInBiogemeData;
	for (unsigned int i = 0; i < observation_files.size(); ++i) {
		string obsrevation_file = observation_folder + observation_files[i];

		DEBUG_MESSAGE("===BEGIN " << observation_files[i] << " ===");
		patObservation new_observation;

		patReadObservationFromKML ro(&network_environment.getNetworkElements());
		ro.parseFile(obsrevation_file, &new_observation);
		string sample_file = sample_folder + "/" + new_observation.getId()
				+ "_sample.kml";

		map<patOd, patChoiceSet> css;
		patReadChoiceSetFromKML rc(&network_environment.getNetworkElements());

		if (ifstream(sample_file.c_str())) {
			css = rc.read(sample_file, rnd);
			DEBUG_MESSAGE(
					"\t read sample file "<<new_observation.getId() + "_sample.kml");

		} else {

			unsigned file_index = 0;

			while (true) {
				++file_index;
				sample_file = sample_folder + "/" + new_observation.getId()
						+ "_" + boost::lexical_cast<string>(file_index)
						+ "_sample.kml";
				if (!ifstream(sample_file.c_str())) {
					break;
				}
				map<patOd, patChoiceSet> new_css = rc.read(sample_file,rnd);
				css.insert(new_css.begin(), new_css.end());
//				DEBUG_MESSAGE(
//						"\t read sample file "<<new_observation.getId() + "_" + boost::lexical_cast < string > (file_index) + "_sample.kml");

			}
		}DEBUG_MESSAGE("\t chocie set read with ods: "<<css.size());
		//			DEBUG_MESSAGE(css.size());
		if (css.size() == 0) {
			continue;
		}
		new_observation.setChoiceSet(css);

//		string vis_folder = patNBParameters::the()->observationDirectory
//				+ patNBParameters::the()->choiceSetFolder + "/"
//				+ new_observation.getId();
//		DEBUG_MESSAGE(vis_folder);
//			system(("mkdir -p \""+vis_folder+"\"").c_str());
//			network_environment.getNetwork(CAR)->exportCadytsOSM(vis_folder+"/network_cadyts.xml");
//			new_observation.exportCadytsVisData();
		sco.addObservation(new_observation, false);

		pair<int, int> ccps = new_observation.countChosenPathsSampled();
		DEBUG_MESSAGE(
				new_observation.getId()<<":"<<ccps.first<<","<<ccps.second<<" ("<<(double) ccps.first/(ccps.first+ccps.second)<<")")
		sampled += ccps.first;
		not_sampled += ccps.second;

		vector<int> new_uppod = new_observation.getUniquePathsPerOD();
		uppod.insert(uppod.end(), new_uppod.begin(), new_uppod.end());
		od_paths.push_back(
				pair<int, int>(new_observation.getNbOfOds(),
						new_observation.getNbrOfCandidates()));
		DEBUG_MESSAGE("===End " << sample_file << " ===");
//		break;
		//boost::thread workerThread(workerFunc,file_path,&path_generator,&network_environment.getNetworkElements());
//		    boost::thread workerThread = testThread();
		//	    workerThread.join();
	}

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
		MHPathGenerator path_generator(patNBParameters::the()->randomSeed);
		path_generator.setRouterLinkCost(&router_link_cost);
		path_generator.setMHWeight(&mh_weight);

		path_generator.setNetwork(network_environment.getNetwork(CAR));
		if (patNBParameters::the()->samplingWithObs == 1) {
			sco.averagePathProbas();
			mh_weight.setPathProbas(&sco.getPathProbas());
		}
		patWriteBiogemeData wbd(sco.getObs(), &utility_function,
				&path_generator, NULL,rnd);

		wbd.writeSampleFile(sample_folder);
	} else if (patNBParameters::the()->pathSampleAlgorithm == "RW") {
		patLinkAndPathCost router_link_cost(1.0, 1.0, 0.0, 0.0);
		RWPathGenerator path_generator(patNBParameters::the()->randomSeed,
				patNBParameters::the()->kumaA, patNBParameters::the()->kumaB,
				&router_link_cost);
		path_generator.setNetwork(network_environment.getNetwork(CAR));

		patWriteBiogemeData wbd(sco.getObs(), &utility_function,
				&path_generator, NULL,rnd);

		wbd.writeSampleFile(sample_folder);
	}
	string scp_txt_fn = sample_folder + string("/sampledChosenPath_")
			+ boost::lexical_cast<string>(
					patNBParameters::the()->SAMPLE_COUNT)
			+ string(".txt");
	ofstream sampleFile(scp_txt_fn.c_str());
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
