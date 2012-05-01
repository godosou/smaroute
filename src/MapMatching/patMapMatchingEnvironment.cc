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
patNetworkEnvironment::patNetworkEnvironment(patGeoBoundingBox& bb, patError*& err) {

	patWay::initiateNetworkTypeRules();//Initiate rules for loading types of network elements.

	m_network_elements.readNetworkFromPostGreSQL(bb, err);
	if(err!=NULL){
		return
	}
	DEBUG_MESSAGE("Network topology has been read;");

	m_networks["car"] =  patNetworkCar();
	m_networks["car"].getFromNetwork(&m_network_elements);
	DEBUG_MESSAGE("Car network size" << m_networks["car"].size());

	m_networks["train"] =  patNetworkTrain();
	m_networks["train"].getFromNetwork(&m_network_elements);
	DEBUG_MESSAGE("Train network size" << m_networks["train"].size());

	m_networks["bus"] =  patNetworkBus();
	m_networks["bus"].getFromNetwork(&m_network_elements);
	DEBUG_MESSAGE("Bus network size" << m_networks["bus"].size());

	m_networks["walk"] =  patNetworkWalk();
	m_networks["walk"].getFromNetwork(&m_network_elements);
	DEBUG_MESSAGE("Walk network size" << m_networks["walk"].size());

	m_networks["bike"] =  patNetworkBike();
	m_networks["bike"].getFromNetwork(&m_network_elements);
	DEBUG_MESSAGE("Bike network size" << m_networks["Bike"].size());

	DEBUG_MESSAGE("finished");

}

patNetworkEnvironment::~patNetworkEnvironment() {
}


const patNetworkBase* patNetworkEnvironment::getNetwork(string network_type) const{
	map<string, patNetworkBase>::const_iterator find_network= m_networks.find(network_type);
	if(find_network==m_networks.end()){
		return NULL;
	}
	else{
		return &find_network->second;
	}
}

const map<string,patNetworkBase>* patNetworkEnvironment::getAllNetworks() const{
	return &m_networks;
}
