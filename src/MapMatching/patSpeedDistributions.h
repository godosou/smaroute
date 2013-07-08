/*
 * patSpeedDistributions.h
 *
 *  Created on: Mar 26, 2012
 *      Author: jchen
 */

#ifndef PATSPEEDDISTRIBUTIONS_H_
#define PATSPEEDDISTRIBUTIONS_H_

#include "patError.h"
#include "patTransportMode.h"
#include "dataStruct.h"
#include <map>
class patSpeedDistributions {
public:
	patSpeedDistributions();
	static patSpeedDistributions* the();
	static patSpeedDistributions* ins;
	static void readParams( patError*& err);
	static double pdf(double v, TransportMode mode);
	virtual ~patSpeedDistributions();
	static map<TransportMode, TrafficModelParam> tm_params;

};

#endif /* PATSPEEDDISTRIBUTIONS_H_ */
