/*
 * patMapMatchingEnvironment.h
 *
 *  Created on: Oct 19, 2011
 *      Author: jchen
 */

#ifndef PATMAPMATCHINGENVIRONMENT_H_
#define PATMAPMATCHINGENVIRONMENT_H_
#include "patNetworkElements.h"
#include "patNetworkBase.h"
#include "patTransportMode.h"
#include <map>

class patNetworkEnvironment {
public:
	/**
	 * Constructor. Load networks from the database.
	 */
	patNetworkEnvironment(){
		;
	}
	patNetworkEnvironment(patGeoBoundingBox& bb);
	 ~patNetworkEnvironment();

	/**
	 * Get the pointer to the network elements.
	 */
	const patNetworkElements& getNetworkElements() const;
	void computeGeneralizedCost(const map<ARC_ATTRIBUTES_TYPES, double>& link_coef);

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
	const patNetworkBase* getNetwork(string network_type);

	const patNetworkBase* getNetwork (TransportMode mode) const;
	double getMinimumTravelTime(const patRoadBase* road, TransportMode mode) const;
	/**
	 * Get the pointer to all networks.
	 * @return the pointer to networks.
	 */
	const map<string, patNetworkBase*>* getAllNetworks() const;
	void exportNetwork();

	bool isPTStop(const patNode* stop) const;
protected:
	patNetworkElements m_network_elements;//The network elements

	map<string, patNetworkBase*> m_networks;//Tranposrtation networks. Key: the network name; value: the network.
};

#endif /* PATMAPMATCHINGENVIRONMENT_H_ */
