/*
 * patBTDDR.cc
 *
 *  Created on: Nov 25, 2011
 *      Author: jchen
 */

#include "patBTDDR.h"
#include "patBTMeasurementModel.h"
patBTDDR::~patBTDDR() {
}

patBTDDR::patBTDDR(patBTMeasurement* bt) :
		patMeasurementDDR::patMeasurementDDR(), m_bt(bt) {

}

void patBTDDR::setBT(patBTMeasurement* bt) {
	m_bt = bt;
}
double patBTDDR::measureDDR(const patArc* arc, TransportMode mode) {
	patBTMeasurementModel bt_m(m_bt, arc, mode);
	return bt_m.integral(1.0);
}
