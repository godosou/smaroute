/*
 * patNetworkCar.h
 *
 *  Created on: Jul 21, 2011
 *      Author: jchen
 */

#ifndef PATNETWORKCAR_H_
#define PATNETWORKCAR_H_

#include "patNetworkUnimodal.h"

class patNetworkCar: public patNetworkUnimodal {
public:
	void getFromNetwork(patNetworkElements* network);
	virtual ~patNetworkCar();
};

#endif /* PATNETWORKCAR_H_ */
