/*
 * patNetworkTrain.h
 *
 *  Created on: Oct 11, 2011
 *      Author: jchen
 */

#ifndef PATNETWORKTRAIN_H_
#define PATNETWORKTRAIN_H_

#include "patNetworkPublicTransport.h"
class patNetworkTrain : public patNetworkPublicTransport{
public:
	patNetworkTrain();
	void getFromNetwork(patNetworkElements* network);
	virtual ~patNetworkTrain();
};

#endif /* PATNETWORKTRAIN_H_ */
