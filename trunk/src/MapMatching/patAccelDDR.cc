/*
 * patAccelDDR.cc
 *
 *  Created on: Nov 25, 2011
 *      Author: jchen
 */

#include "patAccelDDR.h"
#include "patAccelMeasurementModel.h"
patAccelDDR::patAccelDDR(patAccelMeasurement* accel) :
		patMeasurementDDR::patMeasurementDDR(), m_accel(accel) {

}

patAccelDDR::~patAccelDDR() {
}

void patAccelDDR::setAccel(patAccelMeasurement* accel) {
	m_accel = accel;
}

double patAccelDDR::measureDDR(const patArc* arc, TransportMode mode) {
	patAccelMeasurementModel accel_m(m_accel, arc, mode,0.0);//FIX ME
	return accel_m.integral(1.0);
}
