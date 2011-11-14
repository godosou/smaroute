/*
 * patNetworkBike.h
 *
 *  Created on: Oct 28, 2011
 *      Author: jchen
 */

#ifndef PATNETWORKBIKE_H_
#define PATNETWORKBIKE_H_

#include "patNetworkElements.h"
class patNetworkBike  : public patNetworkUnimodal {
public:
	void getFromNetwork(patNetworkElements* network);
	virtual ~patNetworkBike();
};

#endif /* PATNETWORKBIKE_H_ */
