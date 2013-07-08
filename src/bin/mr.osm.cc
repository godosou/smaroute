/*
 * mr.osm.cc
 *
 *  Created on: Feb 25, 2013
 *      Author: jchen
 *  This software package intends to transform osm data from xml format to directed graph in postgregis database.
 */

#include "patWriteNetworkToDB.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "initParams.h"
#include "patWay.h"
#include "patNetworkEnvironment.h"
#include <libconfig.h++>

#include <iostream>
#include <iomanip>
#include <cstdlib>
using namespace libconfig;
using namespace pqxx;
int main(int argc, char *argv[]) {

	string param_file;
	string config_file;
	int c;
	while ((c = getopt(argc, argv, "c:f:n:o:srh")) != -1) {
		switch (c) {
		case 'c':
			config_file = optarg;
			break;
		case 'f':
			param_file = optarg;
			break;
		default:
			abort();
		}
	}

	initParameters(param_file);
	libconfig::Config cfg;
	string conn_string;
	try {
		cfg.readFile(config_file.c_str());
	} catch (const FileIOException &fioex) {
		std::cerr << "I/O error while reading file." << std::endl;
		return (EXIT_FAILURE);
	} catch (const ParseException &pex) {
		std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
				<< " - " << pex.getError() << std::endl;
		return (EXIT_FAILURE);
	}
	string table_name;
	try {
		const Setting& MrOsm_Settings = cfg.getRoot()["MrOSM"];
		MrOsm_Settings.lookupValue("StoreEdgeTablename", table_name);
		cout << "Edge table: " << table_name << endl;
	} catch (const SettingNotFoundException &nfex) {
		cerr << "No 'MrOSM.StoreEdgeTablename' setting in configuration file."
				<< endl;
		abort();
	}

	try {

		const Setting& Network__Settings = cfg.getRoot()["NetworkLoader"];

		Network__Settings.lookupValue("DatabaseHost", conn_string);
		cout << "connection " << conn_string << endl << endl;
	} catch (const SettingNotFoundException &nfex) {
		cerr << "No 'NetworkLoader.DatabaseHost' setting in configuration file."
				<< endl;
		abort();
	}


	connection Conn(conn_string);
	string osm_network_file;
	try {

		const Setting& Network__Settings = cfg.getRoot()["NetworkLoader"];

		Network__Settings.lookupValue("OsmNetworkFile", osm_network_file);
		cout << "OSM fie: " << osm_network_file << endl << endl;
	} catch (const SettingNotFoundException &nfex) {
		cerr
				<< "No 'NetworkLoader.OsmNetworkFile' setting in configuration file."
				<< endl;
		abort();
	}

	string network_folder = osm_network_file.substr(0,
			osm_network_file.rfind("/"));
	string network_rule = network_folder + "/network_rules.csv";
	cout << "look up network rules file" << network_rule << endl;
	if (ifstream(network_rule.c_str())) {
		patWay::initiateNetworkTypeRules(network_rule);
	} else {

		throw RuntimeException(
				"Network rules are not found;Please put network_rules.csv in the same folder as the osm file");
	}

	patGeoBoundingBox bb(patNBParameters::the()->boundingBoxLeftUpLongitude,
			patNBParameters::the()->boundingBoxLeftUpLatitude,
			patNBParameters::the()->boundingBoxRightBottumLongitude,
			patNBParameters::the()->boundingBoxRightBottumLatitude);
	;
	cout <<bb<<endl;
	patNetworkEnvironment network_environment(bb);
	cout << "network with nodes: "
			<< network_environment.getNetwork(CAR)->getNodeSize();
	cout << "network loaded" << endl;

	patWriteNetworkToDB::write(network_environment.getNetwork(CAR), table_name,
			conn_string);
}
