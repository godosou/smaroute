/*
 * patMapMatchingBed.cc
 *
 *  Created on: Aug 18, 2012
 *      Author: jchen
 */

#include "patMapMatchingBed.h"

#include "patDisplay.h"
#include "patException.h"
#include "patNBParameters.h"
#include "patRouter.h"
#include "patGpsSequence.h"
#include "patKMLPathWriter.h"
#include "patMultiModalPath.h"
#include "patLinkAndPathCost.h"
#include "patGetShortestPathFromDB.h"
#include <map>
#include <fstream>
using namespace std;
patMapMatchingBed::patMapMatchingBed(const std::vector<string>& gps_files) :
		m_gps_files(gps_files), m_network_environment(NULL) {
	// getBoundingBox();
	// initiateNetworks();
	// TODO Auto-generated constructor stub

}

patMapMatchingBed::~patMapMatchingBed() {
	if (m_network_environment != NULL) {
		delete m_network_environment;
		m_network_environment = NULL;
	}
}

void patMapMatchingBed::initiateNetworks() {

	string network_file = patNBParameters::the()->OsmNetworkFileName;
	string network_folder = network_file.substr(0, network_file.rfind("/"));
	string network_rule = network_folder + "/network_rules.csv";
	if (ifstream(network_rule.c_str())) {
		patWay::initiateNetworkTypeRules(network_rule);
	}
//	patGeoBoundingBox bb;
	DEBUG_MESSAGE("Use real network");
//	bb = patGeoBoundingBox(patNBParameters::the()->boundingBoxLeftUpLongitude,
//			patNBParameters::the()->boundingBoxLeftUpLatitude,
//			patNBParameters::the()->boundingBoxRightBottumLongitude,
//			patNBParameters::the()->boundingBoxRightBottumLatitude);
//	m_network_environment = new patNetworkEnvironment(m_bb, err);
//	DEBUG_MESSAGE("network with nodes: "<<m_network_environment->getNetwork(CAR)->getNodeSize());
//	cout << "network loaded" << endl;

}
void patMapMatchingBed::getBoundingBox() {
	for (unsigned i = 0; i < m_gps_files.size(); ++i) {
		string file_path = m_gps_files.at(i);
		try {
			patGpsSequence gps_seqeunce(file_path);
			m_bb.extend(gps_seqeunce.computeBoundingBox(0.00));

		} catch (RuntimeException& e) {
			cout << e.what() << endl;
		}
	}
	cout << m_bb;
}
void patMapMatchingBed::genShortestPath() {
	cout << "gps files: " << m_gps_files.size() << endl;
	for (unsigned i = 0; i < m_gps_files.size(); ++i) {
		string file_path = m_gps_files.at(i);
		try {
			patGpsSequence gps_sequence(file_path);

			patGeoBoundingBox bb = gps_sequence.computeBoundingBox(0.05);
			// patNetworkEnvironment network_environment(bb);
			// cout << "network loaded" << endl;
			// const patNetworkBase* sub_network = network_environment.getNetwork(
			// 		CAR);
			// const patNode* first_node = sub_network->getNearestNode(
			// 		gps_sequence.getFirstGps()->getGeoCoord());
			// const patNode* last_node = sub_network->getNearestNode(
			// 		gps_sequence.getLastGps()->getGeoCoord());

			// map<ARC_ATTRIBUTES_TYPES, double> rw_link_coef;

			// rw_link_coef[ENUM_LENGTH] = 1.0;

			// patLinkAndPathCost cost_func(rw_link_coef, 1.0, 0.0);
			// patRouter router(sub_network, &cost_func);
			// patMultiModalPath sp = router.bestRouteFwd(first_node, last_node);

			patNetworkElements network_elements;
			patGetShortestPathFromDB gen_sp(&network_elements);
			patMultiModalPath sp = gen_sp.getSPFromGeoCoords(
					*gps_sequence.getFirstGps()->getGeoCoord(),
					*gps_sequence.getLastGps()->getGeoCoord(),bb);
			cout<<"sp with arcs"<<sp.size();
			patKMLPathWriter sp_writer(
					file_path.substr(0, file_path.size() - 4) + ".sp.kml");

			vector<double> p_dist = gps_sequence.distanceTo(sp);

			string rp_file = file_path.substr(0, file_path.size() - 4)
					+ ".rp.txt";

			ofstream rp_f(rp_file.c_str());
			for (unsigned j = 0; j < p_dist.size(); ++j) {
				rp_f << p_dist[j] << endl;
			}
			rp_f.close();
			map < string, string > attr;
			sp_writer.writePath(sp, attr);
			sp_writer.close();
		} catch (RuntimeException& e) {
			cout << e.what() << endl;
		}
	}
}
