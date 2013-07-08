/*
 * patNetworkCar.h
 *
 *  Created on: Jul 21, 2011
 *      Author: jchen
 */

#ifndef PATNETWORKCAR_H_
#define PATNETWORKCAR_H_

#include "patNetworkElements.h"
#include "patNetworkUnimodal.h"

#include "patGeoBoundingBox.h"
class patNetworkCar: public patNetworkUnimodal {
public:
	patNetworkCar();

	virtual patNetworkBase* clone() const {
		return new patNetworkCar(*this);
	}
	;
	void getFromNetwork(patNetworkElements* network, patGeoBoundingBox bb);
	double getMinSpeed() const;
	double getMaxSpeed() const;
	virtual ~patNetworkCar();
};

#endif /* PATNETWORKCAR_H_ */
