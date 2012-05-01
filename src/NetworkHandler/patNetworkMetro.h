/*
 * patNetworkMetro.h
 *
 *  Created on: Mar 14, 2012
 *      Author: jchen
 */

#ifndef PATNETWORKMETRO_H_
#define PATNETWORKMETRO_H_

#include "patGeoBoundingBox.h"
#include "patNetworkPublicTransport.h"
class patNetworkMetro: public patNetworkPublicTransport {
public:
	patNetworkMetro();

	void getFromNetwork(patNetworkElements* network,patGeoBoundingBox bounding_box);
	virtual ~patNetworkMetro();
	 double getMinSpeed() const;
	 double getMaxSpeed() const;
};
#endif /* PATNETWORKMETRO_H_ */
