/*
 * patNetworkWalk.h
 *
 *  Created on: Oct 28, 2011
 *      Author: jchen
 */

#ifndef PATNETWORKWALK_H_
#define PATNETWORKWALK_H_
#include "patNetworkElements.h"
#include "patNetworkUnimodal.h"
#include "patGeoBoundingBox.h"
class patNetworkWalk : public patNetworkUnimodal {
public:
	patNetworkWalk();
	void getFromNetwork(patNetworkElements* network, patGeoBoundingBox bb);
	virtual ~patNetworkWalk();
	 double getMinSpeed() const;
	 double getMaxSpeed() const;
};

#endif /* PATNETWORKWALK_H_ */

