/*
 * patExperimentBed.cc
 *
 *  Created on: Jul 11, 2012
 *      Author: jchen
 */

#include "patExperimentBed.h"

#include "patError.h"
#include "patNBParameters.h"
#include "patDisplay.h"
#include "patNetworkEnvironment.h"
#include "patWay.h"
#include "patGeoBoundingBox.h"
#include "patLinkAndPathCost.h"
#include "MHWeightFunction.h"
#include "MHPathGenerator.h"
#include "RWPathGenerator.h"
#include "MHWeightFunction.h"
#include "patComputePathSize.h"
#include "patChoiceSet.h"
#include <boost/lexical_cast.hpp>
#include "patReadChoiceSetFromKML.h"
#include "patKMLPathWriter.h"
#include "patUtilityFunction.h"
#include "patSampleChoiceSetForObservations.h"
#include "patGetPathProbasFromObservations.h"
#include "patReadPathsFromKML.h"
#include "patReadObservationFromKML.h"
#include "patWriteBiogemeData.h"
#include "MHObservationWritterWrapper.h"
#include "patPathSizeComputer.h"
#include <dirent.h>
#include "patRouter.h"
#include "patVerifyingSamplingResult.h"
using namespace std;

void patExperimentBed::checkExperimentFolder() const {
	if (!ifstream(m_experiment_folder.c_str())) {
		throw RuntimeException("Experiment folder does not exist");
	}
}
void patExperimentBed::checkChoiceSetFolder() const {
	if (!ifstream(m_choice_set_foler.c_str())) {
		throw RuntimeException("Choiceset folder does not exist");
	}
}
void patExperimentBed::checkObservationFolder() const {
	if (!ifstream(m_observation_folder.c_str())) {
		throw RuntimeException("observation folder does not exist");
	}
}
void patExperimentBed::initiateNetworks() {

	patError* err(NULL);

	string network_file = patNBParameters::the()->OsmNetworkFileName;
	string network_folder = network_file.substr(0, network_file.rfind("/"));
	string network_rule = network_folder + "/network_rules.csv";
	if (ifstream(network_rule.c_str())) {
		patWay::initiateNetworkTypeRules(network_rule);
	}
	patGeoBoundingBox bb;
	if (m_network_real) {
		DEBUG_MESSAGE("Use real network");
		bb = patGeoBoundingBox(
				patNBParameters::the()->boundingBoxLeftUpLongitude,
				patNBParameters::the()->boundingBoxLeftUpLatitude,
				patNBParameters::the()->boundingBoxRightBottumLongitude,
				patNBParameters::the()->boundingBoxRightBottumLatitude);
	} else {
		bb = patGeoBoundingBox(-100, 100, 100, -100);
	}

	m_network_environment = new patNetworkEnvironment(bb, err);
	if (err != NULL) {
		exit(-1);
	}
//	DEBUG_MESSAGE("network with nodes: "<<m_network_environment->getNetwork(CAR)->getNodeSize());
	cout << "network loaded" << endl;

}

void patExperimentBed::exportNetwork() {

	m_network_environment->getNetwork(m_transport_mode)->exportKML(
			patNBParameters::the()->OsmNetworkFileName);
	m_network_environment->getNetwork(m_transport_mode)->exportShpFiles(
			patNBParameters::the()->OsmNetworkFileName);
	cout << "network exported" << endl;
}

void patExperimentBed::enumerateMHPaths() {
	checkExperimentFolder();
	const patNode* origin = m_network_environment->getNetworkElements().getNode(
			patNBParameters::the()->OriginId);
	const patNode* destination =
			m_network_environment->getNetworkElements().getNode(
					patNBParameters::the()->DestinationId);
	if (origin == NULL || destination == NULL) {
		WARNING("od not specified");
		exit(-1);
	}

	patPathGenerator* generator_clone = m_mh_path_generator->clone();
//	patNetworkBase* cloned_network = m_mh_path_generator->getNetwork()->clone();
//	generator_clone->setNetwork(cloned_network);
	cout << "Start enumeration" << endl;
	string file_name = m_experiment_folder + "universal_choice_set.kml";
	patKMLPathWriter path_writer(file_name);
	generator_clone->setPathWriter(&path_writer);
	generator_clone->run(origin, destination);
	cout << "Finish enumeration" << endl;
	path_writer.close();
	delete generator_clone;
	generator_clone = NULL;
//	delete cloned_network;
//	cloned_network = NULL;

}
void patExperimentBed::sampleChoiceSetWithOd(const unsigned count) {
	checkChoiceSetFolder();
	const patNode* origin = m_network_environment->getNetworkElements().getNode(
			patNBParameters::the()->OriginId);
	const patNode* destination =
			m_network_environment->getNetworkElements().getNode(
					patNBParameters::the()->DestinationId);
	if (origin == NULL || destination == NULL) {
		WARNING("od not specified");
		exit(-1);
	}

	const patPathGenerator* sampling_pg(NULL);

	if (m_algorithm == "MH" && m_mh_path_generator != NULL) {
		sampling_pg = m_mh_path_generator;

		DEBUG_MESSAGE(m_mh_path_generator->getNetwork()->getNodeSize());
		DEBUG_MESSAGE(m_mh_path_generator->getNetwork()->getMinimumLabel());

	} else if (m_algorithm == "RW" && m_rw_path_generator != NULL) {
		DEBUG_MESSAGE(m_rw_path_generator->getNetwork()->getNodeSize());

		sampling_pg = m_rw_path_generator;

	} else {
		WARNING("Wrong sampling algorithm: "<<m_algorithm);
		throw RuntimeException("Wrong sampling algorithm");
	}

	DEBUG_MESSAGE(
			"Sample choice set with od: "<<origin->getUserId()<<"-"<<destination->getUserId());
#pragma omp parallel num_threads(patNBParameters::the()->nbrOfThreads)
	{
#pragma omp for
		for (unsigned i = 0; i < count; ++i) {

			patPathGenerator* generator_clone = sampling_pg->clone();
//			patNetworkBase* cloned_network = sampling_pg->getNetwork()->clone();
//
//			generator_clone->setNetwork(cloned_network);

			cout << "Start an od" << endl;
			string file_name = m_choice_set_foler
					+ boost::lexical_cast<string>(i + 1) + "_sample.kml";
			patKMLPathWriter path_writer(file_name);
			generator_clone->setPathWriter(&path_writer);
			generator_clone->run(origin, destination);
			cout << "An od is dealt with" << endl;
			path_writer.close();
			delete generator_clone;
			generator_clone = NULL;
//			delete cloned_network;
//			cloned_network = NULL;
		}
	}
}

void patExperimentBed::sampleChoiceSet() {
	checkChoiceSetFolder();
	checkObservationFolder();
	readObservations();
	patSampleChoiceSetForObservations sample_choice_set;

	DEBUG_MESSAGE("Sample choice set");

	if (m_algorithm == "MH" && m_mh_path_generator != NULL) {
		sample_choice_set.sampleChoiceSet(m_observations, m_mh_path_generator,
				m_choice_set_foler);
	} else if (m_algorithm == "RW" && m_rw_path_generator != NULL) {

		sample_choice_set.sampleChoiceSet(m_observations, m_rw_path_generator,
				m_choice_set_foler);
	}

	else {
		WARNING("Wrong sampling algorithm: "<<m_algorithm);
		throw RuntimeException("Wrong sampling algorithm");
	}
}

void patExperimentBed::readUniversalChoiceSet() {

	if (m_universal_choice_set.empty()) {
		string choiceset_file = m_experiment_folder
				+ "universal_choice_set.kml";
		if (!ifstream(choiceset_file.c_str())) {
			return;
//		throw RuntimeException("universal choice set not found");
//		exit(-1);
		}
		patReadChoiceSetFromKML rc(
				&m_network_environment->getNetworkElements());
		//update the way to calculate path size. the universal choice set may not contain a path.

		map<patOd, patChoiceSet> od_choice_set = rc.read(choiceset_file, m_rnd);
		patOd od = od_choice_set.begin()->first;
		m_universal_choice_set = od_choice_set.begin()->second;
	}
}
void patExperimentBed::initCostFunctions() {

	if (m_algorithm == "MH") {
		cout << "Use MH algorithm" << endl;

		map<ARC_ATTRIBUTES_TYPES, double> link_coef;

		link_coef[ENUM_LENGTH] = patNBParameters::the()->mh_length_coef;

		if (m_network_real) {
			cout << "\tReal network" << endl;
			link_coef[ENUM_TRAFFIC_SIGNAL] = patNBParameters::the()->mh_sb_coef;
		} else {
			cout << "\tSynthetic network" << endl;
			link_coef[ENUM_SPEED_BUMP] = patNBParameters::the()->mh_sb_coef;
		}

		m_network_environment->computeGeneralizedCost(link_coef);

		double router_ps_coef = 0.0;
		double router_link_scale =
				patNBParameters::the()->router_cost_link_scale;
		if (router_link_scale > patNBParameters::the()->mh_link_scale) {
			router_link_scale = patNBParameters::the()->mh_link_scale;
		}
		m_mh_router_link_cost = new patLinkAndPathCost(link_coef,router_link_scale,
				router_ps_coef);

		m_mh_weight_function = new MHWeightFunction(
				link_coef,
				patNBParameters::the()->mh_link_scale,
				patNBParameters::the()->mh_ps_coef,
				patNBParameters::the()->mh_obs_scale);
		cout << "MH params:" << endl;
		cout << "\tmu\t " << patNBParameters::the()->mh_link_scale << endl;
		cout << "\tlength\t " << patNBParameters::the()->mh_length_coef << endl;
		cout << "\tsb\t " << patNBParameters::the()->mh_sb_coef << endl;
		cout << "\tps\t " << patNBParameters::the()->mh_ps_coef << endl;
		cout << "\tobs\t " << patNBParameters::the()->mh_obs_scale << endl;
		if (patNBParameters::the()->mh_ps_coef > 0.0) {

			cout << "\tUse path size for sampling algorithm" << endl;
			readUniversalChoiceSet();

			if (m_universal_choice_set.empty()) {
				throw RuntimeException(
						"ps coef is specified but universal choice set not found");
			}
			patPathSizeComputer ps_computer(
					m_universal_choice_set.getChoiceSet());
			m_mh_weight_function->setPathSizeComputer(&ps_computer);
		}
		int sample_with_obs = patNBParameters::the()->samplingWithObs;
		if (patNBParameters::the()->mh_obs_scale > 0.0) {
			cout << "\tUse mh observations for sampling algorithm" << endl;
			readObservations();
			patGetPathProbasFromObservations ppfo;
			if (m_observations.empty()) {
				throw RuntimeException("No observation is read");
			}
			m_obs_path_probas = ppfo.getPathProbas(m_observations);
			m_mh_weight_function->setPathProbas(&m_obs_path_probas);
		}

		else {
			m_mh_weight_function->setPathProbas(NULL);

		}
		m_mh_path_generator = new MHPathGenerator(m_rnd);
		m_mh_path_generator->setRouterLinkCost(m_mh_router_link_cost);
		m_mh_path_generator->setNetwork(
				m_network_environment->getNetwork(m_transport_mode));

		m_mh_path_generator->setMHWeight(m_mh_weight_function);
	} else if (m_algorithm == "RW") {
		cout << "Use RW algorithm" << endl;
		map<ARC_ATTRIBUTES_TYPES, double> rw_link_coef;

		rw_link_coef[ENUM_LENGTH] = 1.0;

		m_rw_router_link_cost = new patLinkAndPathCost(rw_link_coef, 1.0, 0.0);

		m_rw_path_generator = new RWPathGenerator(m_rnd,
				patNBParameters::the()->kumaA, patNBParameters::the()->kumaB,
				m_rw_router_link_cost);
		m_rw_path_generator->setNetwork(
				m_network_environment->getNetwork(m_transport_mode));

	}

	map<ARC_ATTRIBUTES_TYPES, double> utility_link_coef;

	utility_link_coef[ENUM_LENGTH] = patNBParameters::the()->mh_link_scale;

	if (m_network_real) {
		utility_link_coef[ENUM_TRAFFIC_SIGNAL] =
				patNBParameters::the()->mh_sb_coef;
	} else {
		utility_link_coef[ENUM_SPEED_BUMP] = patNBParameters::the()->mh_sb_coef;
	}

	m_utility_function = new patUtilityFunction(utility_link_coef,
			patNBParameters::the()->mh_link_scale,
			patNBParameters::the()->mh_ps_coef);

}

bool compareTwoObservationFiles(string file1, string file2) {

	try {
		unsigned dot_position_1 = file1.rfind(".");
		string file1_id = file1.substr(0, dot_position_1);

		unsigned dot_position_2 = file2.rfind(".");
		string file2_id = file2.substr(0, dot_position_2);

		if (atoi(file1_id.c_str()) < atoi(file2_id.c_str())) {
			return true;
		} else {
			return false;
		}

	} catch (...) {
		return file1 < file2;
	}
}
vector<string> patExperimentBed::getObservationFiles() {

	DIR * dip;
	struct dirent *dit;
	const char* dir_name_char = m_observation_folder.c_str();

	DEBUG_MESSAGE("Try to open directory " << dir_name_char);
	if ((dip = opendir(dir_name_char)) == NULL) {
		stringstream str;
		str << "Directory " << dir_name_char
				<< " doesn't exist or no permission to read.";
		WARNING(str.str());
		exit(-1);
	}
//	DEBUG_MESSAGE("Direcotry " << dir_name_char << " is now open");
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
//				DEBUG_MESSAGE("Found file:" << fileName);
				observation_files.push_back(fileName);
			}

		}
	}

	closedir(dip);

	if (observation_files.empty()) {
		exit(0);
	}

	sort(observation_files.begin(), observation_files.end(),
			compareTwoObservationFiles);
	DEBUG_MESSAGE(
			observation_files.size()<<" kml files are read. First one: "<<observation_files.front());
	return observation_files;
}

void patExperimentBed::readObservations() {

	if (m_observations.empty()) {
		DEBUG_MESSAGE("read observation files");
		if (m_observation_real) {
			readRealObservations();
		} else {
			readSyntheticObservations();
		}
	}
}
void patExperimentBed::readSyntheticObservations() {

	vector<string> observation_files = getObservationFiles();
	for (unsigned i = 0; i < observation_files.size() && i < m_nbr_observations;
			++i) {
		string file_name = m_observation_folder + observation_files[i];
		patObservation new_observation;
		new_observation.setId(boost::lexical_cast<string>(i));
		if (!ifstream(file_name.c_str())) {
			WARNING(file_name);
			throw RuntimeException("no valid observation file");
		}
		patReadPathsFromKML rp;

		vector<patMultiModalPath> obs_paths = rp.read(
				&m_network_environment->getNetworkElements(), file_name);
		if (obs_paths.size() != 1) {
			WARNING("WRONG PATH NUMBER"<<obs_paths.size());
		}
		new_observation.addPath(obs_paths.front(), 1.0);

		unsigned slash_position = observation_files[i].rfind(".");
		new_observation.setId(observation_files[i].substr(0, slash_position));
//	    cout<<new_observation.getId();
//	    break;
		m_observations.push_back(new_observation);

	}
	DEBUG_MESSAGE(m_observations.size()<<" synthetic observations are read.");
}

void patExperimentBed::readRealObservations() {

	vector<string> observation_files = getObservationFiles();
	for (unsigned i = 0; i < observation_files.size() && i < m_nbr_observations;
			++i) {
		string file_name = m_observation_folder + observation_files[i];
		patObservation new_observation;
		patReadObservationFromKML ro(
				&m_network_environment->getNetworkElements());
		ro.parseFile(file_name, &new_observation);
		m_observations.push_back(new_observation);
	}
	DEBUG_MESSAGE(m_observations.size()<<" real observations are read.");
}
patExperimentBed::patExperimentBed(bool network_real, bool observation_real,
		TransportMode transport_mode, unsigned nbr_observations) :
		m_nbr_observations(nbr_observations), m_network_real(network_real), m_observation_real(
				observation_real), m_transport_mode(transport_mode), m_mh_router_link_cost(
				NULL), m_mh_weight_function(NULL), m_utility_function(NULL), m_rw_router_link_cost(
				NULL), m_mh_path_generator(NULL), m_rw_path_generator(NULL), m_algorithm(
				patNBParameters::the()->pathSampleAlgorithm), m_rnd(
				patNBParameters::the()->randomSeed), m_experiment_folder(
				patNBParameters::the()->observationDirectory), m_observation_folder(
				patNBParameters::the()->observationDirectory + "observations/"), m_choice_set_foler(
				patNBParameters::the()->observationDirectory
						+ patNBParameters::the()->choiceSetFolder + "/") {

	initiateNetworks();
	initCostFunctions();
}

patExperimentBed::~patExperimentBed() {

	if (m_mh_router_link_cost != NULL) {
		delete m_mh_router_link_cost;
		m_mh_router_link_cost = NULL;
	}

	if (m_mh_weight_function != NULL) {
		delete m_mh_weight_function;
		m_mh_weight_function = NULL;
	}

	if (m_rw_router_link_cost != NULL) {
		delete m_rw_router_link_cost;
		m_rw_router_link_cost = NULL;
	}

	if (m_utility_function != NULL) {
		delete m_utility_function;
		m_utility_function = NULL;
	}

	if (m_mh_path_generator != NULL) {
		delete m_mh_path_generator;
		m_mh_path_generator = NULL;
	}

	if (m_rw_path_generator != NULL) {
		delete m_rw_path_generator;
		m_rw_path_generator = NULL;
	}
	if (m_network_environment != NULL) {
		delete m_network_environment;
		m_network_environment = NULL;
	}

}
void patExperimentBed::writeBiogeme() {
	checkObservationFolder();
	checkChoiceSetFolder();
	patPathGenerator* sampling_pg(NULL);

	if (m_algorithm == "MH") {
		sampling_pg = m_mh_path_generator;
	} else if (m_algorithm == "RW") {
		sampling_pg = m_rw_path_generator;

	} else {
		WARNING("Wrong sampling algorithm: "<<m_algorithm);
		throw RuntimeException("Wrong sampling algorithm");
	}

	readObservations();
	readChoiceSetForObservations();
	readUniversalChoiceSet();
	patWriteBiogemeData wbd(m_observations, m_utility_function, sampling_pg,
			&m_universal_choice_set, m_rnd);

	wbd.writeSampleFile(m_choice_set_foler);

}
void patExperimentBed::readChoiceSetForObservations() {
	DEBUG_MESSAGE("Read choice set for observations: "<<m_observations.size());
	int sampled = 0;
	int not_sampled = 0;
	vector<int> uppod;
	vector<pair<int, int> > od_paths;

#pragma omp parallel num_threads( patNBParameters::the()->nbrOfThreads)

	{
#pragma omp for
		for (unsigned int i = 0; i < m_observations.size(); ++i) {
			string sample_file = m_choice_set_foler + "/"
					+ m_observations[i].getId() + "_sample.kml";

			std::map<patOd, patChoiceSet> css;
			patReadChoiceSetFromKML rc(
					&m_network_environment->getNetworkElements());

			if (ifstream(sample_file.c_str())) {
				css = rc.read(sample_file, m_rnd);
				cout << "\t read sample file "
						<< m_observations[i].getId() + "_sample.kml" << endl;

			} else {

				unsigned file_index = 0;
				unsigned biggest_file_index = file_index;
				while (true) {
					++file_index;

					sample_file = m_choice_set_foler + "/"
							+ m_observations[i].getId() + "_"
							+ boost::lexical_cast<string>(file_index)
							+ "_sample.kml";
					if (!ifstream(sample_file.c_str())) {

						break;
					}

					biggest_file_index = file_index;

				}

				if (biggest_file_index == 0) {
					WARNING(
							" The choice set for"<< m_observations[i].getId()<<" does not exist");
					throw RuntimeException(
							" The choice set for does not exist");
				}
				for (unsigned file_index = 1; file_index <= biggest_file_index;
						++file_index) {

					const string ind_sample_file = m_choice_set_foler + "/"
							+ m_observations[i].getId() + "_"
							+ boost::lexical_cast<string>(file_index)
							+ "_sample.kml";
					if (!ifstream(ind_sample_file.c_str())) {
						cout << "file " << ind_sample_file << " does not exist"
								<< endl;
						throw RuntimeException("File does not exist");
					}
					std::map<patOd, patChoiceSet> new_css = rc.read(
							ind_sample_file, m_rnd);

					css.insert(new_css.begin(), new_css.end());
//					cout << "\t read sample file " << ind_sample_file << endl;
				}

			}

//			cout << "\t chocie set read with ods: " << css.size() << endl;
			if (css.size() == 0) {
				throw RuntimeException("no choice set for an observation.");
			}
			m_observations[i].setChoiceSet(css);
#pragma omp critical
			{

				pair<int, int> ccps =
						m_observations[i].countChosenPathsSampled();
//				cout << "\t" << m_observations[i].getId() << "sampled: "
//						<< ccps.first << ", not sampled: " << ccps.second
//						<< " ("
//						<< (double) ccps.first / (ccps.first + ccps.second)
//						<< ")" << endl;
				sampled += ccps.first;
				not_sampled += ccps.second;

				vector<int> new_uppod = m_observations[i].getUniquePathsPerOD();
				uppod.insert(uppod.end(), new_uppod.begin(), new_uppod.end());
				od_paths.push_back(
						pair<int, int>(m_observations[i].getNbOfOds(),
								m_observations[i].getNbrOfCandidates()));
			}

		}
	}

	string scp_txt_fn = m_choice_set_foler + string("/sampledChosenPath_")
			+ boost::lexical_cast<string>(patNBParameters::the()->SAMPLE_COUNT)
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

void patExperimentBed::simulateObservations() {
	checkObservationFolder();
	const patNode* origin = m_network_environment->getNetworkElements().getNode(
			patNBParameters::the()->OriginId);
	const patNode* destination =
			m_network_environment->getNetworkElements().getNode(
					patNBParameters::the()->DestinationId);
	if (origin == NULL || destination == NULL) {
		WARNING("od not specified");
		exit(-1);
	}
//	patNetworkBase* cloned_network = m_mh_path_generator->getNetwork()->clone();

	MHPathGenerator* generator_clone = m_mh_path_generator->clone();

//	generator_clone->setNetwork(cloned_network);
	cout << "Start simulation" << endl;
	MHObservationWritterWrapper path_writer(m_observation_folder,
			patNBParameters::the()->SAMPLEINTERVAL_ELEMENT,
			m_mh_weight_function);
	generator_clone->setWritterWrapper(&path_writer);
	generator_clone->run(origin, destination);
	cout << "Finish simulation" << endl;
	delete generator_clone;
	generator_clone = NULL;
//	delete cloned_network;
//	cloned_network = NULL;
}

void patExperimentBed::verifySamplingResult() {
	patPathGenerator* sampling_pg(NULL);

	if (m_algorithm == "MH") {
		sampling_pg = m_mh_path_generator;
	} else if (m_algorithm == "RW") {
		sampling_pg = m_rw_path_generator;

	} else {
		WARNING("Wrong sampling algorithm: "<<m_algorithm);
		throw RuntimeException("Wrong sampling algorithm");
	}

	readObservations();
	readChoiceSetForObservations();
	readUniversalChoiceSet();
	patVerifyingSamplingResult vsr(m_observations);
	vsr.verifyProbability(m_universal_choice_set, sampling_pg);

}
void patExperimentBed::testNetwork() const {
	patRouter start_router(m_network_environment->getNetwork(m_transport_mode),
			m_mh_router_link_cost);

	patShortestPathTreeGeneral sp_tree(FWD);
	const patNode* origin = m_network_environment->getNetworkElements().getNode(
			patNBParameters::the()->OriginId);
	const patNode* destination =
			m_network_environment->getNetworkElements().getNode(
					patNBParameters::the()->DestinationId);
	if (origin == NULL || destination == NULL) {
		WARNING("od not specified");
		exit(-1);
	}
	if (m_mh_router_link_cost == NULL) {
		WARNING("WRONG ROUTER");
	}

	start_router.fwdCost(sp_tree, origin, destination);
	double linkCostSP = sp_tree.getLabel(destination);

	DEBUG_MESSAGE(linkCostSP);
	list<const patRoadBase*> list_of_roads;
	sp_tree.getShortestPathTo(list_of_roads, destination);
	patMultiModalPath sp_path(list_of_roads);

	patMultiModalPath new_path_fwd = start_router.bestRouteFwd(origin,
			destination);
	patMultiModalPath new_path_bwd = start_router.bestRouteBwd(origin,
			destination);
	patKMLPathWriter sp_writer("sp.kml");
	map<string, string> attr;
	sp_writer.writePath(sp_path, attr);
	sp_writer.writePath(new_path_fwd, attr);
	sp_writer.writePath(new_path_bwd, attr);
	DEBUG_MESSAGE(m_mh_router_link_cost->getCost(sp_path));
	DEBUG_MESSAGE(m_mh_weight_function->getCost(sp_path));
	DEBUG_MESSAGE(m_mh_weight_function->logWeigthOriginal(sp_path));
	unordered_map<const patNode*, double> proposal_probabilities;
	m_mh_path_generator->getNodeProbability(start_router,
			proposal_probabilities, origin, destination);
	for (unordered_map<const patNode*, double>::iterator node_iter =
			proposal_probabilities.begin();
			node_iter != proposal_probabilities.end(); ++node_iter) {
		DEBUG_MESSAGE(node_iter->first->getUserId()<<":"<<node_iter->second);
	}
	sp_writer.close();
}
