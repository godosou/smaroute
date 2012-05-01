/*
 * patAccelDDR.h
 *
 *  Created on: Nov 25, 2011
 *      Author: jchen
 */

#ifndef PATACCELDDR_H_
#define PATACCELDDR_H_

#include "patMeasurementDDR.h"
#include "patAccelMeasurement.h"
class patAccelDDR: public patMeasurementDDR {
public:
	patAccelDDR(patAccelMeasurement* accel);
	void setAccel(patAccelMeasurement* accel);
	bool detArcDDR(const patArc* arc, TransportMode mode) {
		return patMeasurementDDR::detArcDDR(arc, mode);
	}
	double measureDDR(const patArc* arc, TransportMode mode);
	virtual ~patAccelDDR();
protected:
	patAccelMeasurement* m_accel;
};

#endif /* PATACCELDDR_H_ */
