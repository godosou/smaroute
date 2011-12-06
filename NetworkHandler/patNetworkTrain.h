/*
 * patNetworkTrain.h
 *
 *  Created on: Oct 11, 2011
 *      Author: jchen
 */

#ifndef PATNETWORKTRAIN_H_
#define PATNETWORKTRAIN_H_

#include "patGeoBoundingBox.h"
#include "patNetworkPublicTransport.h"
class patNetworkTrain : public patNetworkPublicTransport{
public:
	patNetworkTrain();
	void getFromNetwork(patNetworkElements* network,patGeoBoundingBox bounding_box);
	virtual ~patNetworkTrain();
	 double getMinSpeed() const;
	 double getMaxSpeed() const;
};

#endif /* PATNETWORKTRAIN_H_ */
