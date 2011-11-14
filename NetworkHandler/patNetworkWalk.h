/*
 * patNetworkWalk.h
 *
 *  Created on: Oct 28, 2011
 *      Author: jchen
 */

#ifndef PATNETWORKWALK_H_
#define PATNETWORKWALK_H_
#include "patNetworkElements.h"
class patNetworkWalk : public patNetworkUnimodal {
public:
	void getFromNetwork(patNetworkElements* network);
	virtual ~patNetworkWalk();
};

#endif /* PATNETWORKWALK_H_ */

