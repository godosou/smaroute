/*
 * patMapMatchingEnvironment.cc
 *
 *  Created on: Oct 19, 2011
 *      Author: jchen
 */

#include "patNetworkEnvironment.h"
#include "patGeoBoundingBox.h"
#include "patWay.h"
#include "patError.h"
#include "patPostGreSQLConnector.h"
#include "patDisplay.h"
#include "patNetworkCar.h"
#include "patNetworkBus.h"
#include "patNetworkTrain.h"
#include "patNetworkWalk.h"
#include "patNetworkMetro.h"
#include "patNetworkBike.h"
#include "patNBParameters.h"
patNetworkEnvironment::patNetworkEnvironment(patGeoBoundingBox& bb,
		patError*& err) {

	patWay::initiateNetworkTypeRules(); //Initiate rules for loading types of network elements.
	DEBUG_MESSAGE(patNBParameters::the()->enableTrainNetwork
			<<patNBParameters::the()->enableBikeNetwork
			<<patNBParameters::the()->enableWalkNetwork
			<<patNBParameters::the()->enableBusNetwork
			<<patNBParameters::the()->enableCarNetwork
			<<patNBParameters::the()->enableMetroNetwork
			);

	m_network_elements.readNetworkFromPostGreSQL(bb, err);
	if (err != NULL) {
		WARNING("Fail to setup network environment");
	} else {

		DEBUG_MESSAGE("Network topology has been read.");
		DEBUG_MESSAGE("Start to read transport network structures.");
		if (patNBParameters::the()->enableTrainNetwork == 1) {
			m_networks["train"] = new patNetworkTrain();
			m_networks["train"]->getFromNetwork(&m_network_elements, bb);
		}
		if (patNBParameters::the()->enableBikeNetwork == 1) {
			m_networks["bike"] = new patNetworkBike();
			m_networks["bike"]->getFromNetwork(&m_network_elements, bb);
		}
		if (patNBParameters::the()->enableWalkNetwork == 1) {
			m_networks["walk"] = new patNetworkWalk();
			m_networks["walk"]->getFromNetwork(&m_network_elements, bb);
		}
		if (patNBParameters::the()->enableBusNetwork == 1) {
			m_networks["bus"] = new patNetworkBus();
			m_networks["bus"]->getFromNetwork(&m_network_elements, bb);
		}

		if (patNBParameters::the()->enableCarNetwork == 1) {
			m_networks["car"] = new patNetworkCar();
			m_networks["car"]->getFromNetwork(&m_network_elements, bb);
		}

		if (patNBParameters::the()->enableMetroNetwork == 1) {
			m_networks["metro"] = new patNetworkMetro();
			m_networks["metro"]->getFromNetwork(&m_network_elements, bb);
		}
		if (m_networks.find("walk")!=m_networks.end()){
			DEBUG_MESSAGE("Deal with walk connection for networks");

			for (map<string, patNetworkBase*>::iterator network_iter =
					m_networks.begin(); network_iter != m_networks.end();
					++network_iter) {
				/*
				if(isPublicTransport(network_iter->second->getTransportMode())){
					network_iter->second->walkFromToStops(&m_network_elements,m_networks["walk"]);
				}
				*/
				if(network_iter->second->getTransportMode()==METRO){
					if(patNBParameters::the()->walkOnTrack){
						network_iter->second->walkOnTrack(&m_network_elements,m_networks["walk"]);
					}
					network_iter->second->walkFromToStops(&m_network_elements,m_networks["walk"]);
				}
			}
		}
		m_network_elements.computeLength(); //It has to be here. after reading all network, before finalize them.

		for (map<string, patNetworkBase*>::iterator network_iter =
				m_networks.begin(); network_iter != m_networks.end();
				++network_iter) {
			network_iter->second->finalizeNetwork();
			DEBUG_MESSAGE(
					network_iter->first << " network node size"
							<< network_iter->second->getNodeSize());

		}
		DEBUG_MESSAGE("finished");
		exportNetwork();
	}
}

void patNetworkEnvironment::exportNetwork() {
	for (map<string, patNetworkBase*>::iterator network_iter =
			m_networks.begin(); network_iter != m_networks.end();
			++network_iter) {
		network_iter->second->exportKML(network_iter->first);
		network_iter->second->exportShpFiles(network_iter->first);
	}
}
patNetworkEnvironment::~patNetworkEnvironment() {
	for (map<string, patNetworkBase*>::iterator network_iter =
			m_networks.begin(); network_iter != m_networks.end();
			++network_iter) {
		delete network_iter->second;
		network_iter->second = NULL;
	}
}

const patNetworkBase* patNetworkEnvironment::getNetwork(string network_type) {
	map<string, patNetworkBase*>::const_iterator find_network = m_networks.find(
			network_type);
	if (find_network == m_networks.end()) {
		return NULL;
	} else {
		return find_network->second;
	}
}

const map<string, patNetworkBase*>* patNetworkEnvironment::getAllNetworks() const {
	return &m_networks;
}
const patNetworkElements& patNetworkEnvironment::getNetworkElements() const {
	return m_network_elements;
}

const patNetworkBase* patNetworkEnvironment::getNetwork(
		TransportMode mode) const {
	string network_string = getTransportMode(mode);
	const map<string, patNetworkBase*>::const_iterator find_network =
			m_networks.find(network_string);

	if (find_network == m_networks.end()) {
		DEBUG_MESSAGE("NETWORK"<<mode<<"NOT FOUND");
		return NULL;
	} else {
		DEBUG_MESSAGE("NETWORK"<<mode<<" FOUND");
		return find_network->second;
	}
}

double patNetworkEnvironment::getMinimumTravelTime(const patRoadBase* road,
		TransportMode mode) const {
	return 3.6 * (road->getLength()) / (getNetwork(mode)->getMaxSpeed());
}

	bool patNetworkEnvironment::isPTStop(const patNode* stop) const{

	for (map<string, patNetworkBase*>::const_iterator network_iter =
			m_networks.begin(); network_iter != m_networks.end();
			++network_iter) {
		if (network_iter->second->isPT()){
			if( network_iter->second->isStop(stop)){
				return true;
			}
		}
	}
	return false;
	}
