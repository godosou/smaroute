/*
 * patMapMatchingEnvironment.h
 *
 *  Created on: Oct 19, 2011
 *      Author: jchen
 */

#ifndef PATMAPMATCHINGENVIRONMENT_H_
#define PATMAPMATCHINGENVIRONMENT_H_
#include "patNetworkCar.h"
#include "patNetworkBus.h"
#include "patNetworkTrain.h"
#include "patNetworkElements.h"
#include "patNetworkBase.h"
#include <map>

class patNetworkEnvironment {
public:
	/**
	 * Constructor. Load networks from the database.
	 */
	patNetworkEnvironment();
	virtual ~patNetworkEnvironment();

	/**
	 * Get the pointer to the network elements.
	 */
	patNetworkElements* getNetworkElements() const;

	/**
	 * Get a particular type of network.
	 * @param network_type the type of the network.
	 * Availables are:
	 * - "car"
	 * - "bus"
	 * - "train"
	 * - "walk"
	 * - "bike"
	 */
	const patNetworkBase* getNetwork(string network_type) const;

	/**
	 * Get the pointer to all networks.
	 * @return the pointer to networks.
	 */
	const map<string,patNetworkBase>* getAllNetworks() const;
protected:
	patNetworkElements m_network_elements;
	map<string,patNetworkBase> m_networks;
};

#endif /* PATMAPMATCHINGENVIRONMENT_H_ */
