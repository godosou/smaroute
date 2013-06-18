/*
 * patNetworkBike.h
 *
 *  Created on: Oct 28, 2011
 *      Author: jchen
 */

#ifndef PATNETWORKBIKE_H_
#define PATNETWORKBIKE_H_

#include "patNetworkElements.h"
#include "patNetworkUnimodal.h"
#include "patGeoBoundingBox.h"
class patNetworkBike  : public patNetworkUnimodal {
public:
	patNetworkBike();
	virtual patNetworkBase* clone() const {
		return new patNetworkBike(*this);
	}
	;
	void getFromNetwork(patNetworkElements* network, patGeoBoundingBox bb);
	virtual ~patNetworkBike();
	 double getMinSpeed() const;
	 double getMaxSpeed() const;
};

#endif /* PATNETWORKBIKE_H_ */
