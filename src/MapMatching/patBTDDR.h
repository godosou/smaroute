/*
 * patBTDDR.h
 *
 *  Created on: Nov 25, 2011
 *      Author: jchen
 */

#ifndef PATBTDDR_H_
#define PATBTDDR_H_
#include "patBTMeasurement.h"
#include "patMeasurementDDR.h"
class patBTDDR: public patMeasurementDDR {
public:
	patBTDDR(patBTMeasurement* bt);
	void setBT(patBTMeasurement* bt);
	 bool detArcDDR(const patArc* arc, TransportMode mode) {
		return patMeasurementDDR::detArcDDR(arc, mode);
	}

	double measureDDR(const patArc* arc, TransportMode mode);
	virtual ~patBTDDR();
protected:
	patBTMeasurement* m_bt;
};

#endif /* PATBTDDR_H_ */
