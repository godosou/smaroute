/*
 * File:   patBTMeasurementModel.cc
 * Author: jchen
 *
 * Created on May 4, 2011, 4:34 PM
 */

#include "patBTMeasurementModel.h"
#include <vector>
#include "patType.h"
#include "patTransportMode.h"
#include "patDisplay.h"
patBTMeasurementModel::patBTMeasurementModel(patMeasurement* measurement,
		const patArc* arc, TransportMode mode) :
		patMeasurementModel::patMeasurementModel(measurement, arc, mode) {
	initiate();
	double simple_value = measurement->getSimpleValue();
	m_const = arc->getLength();

	double low_proba;
	double high_proba;
	if (isPublicTransport(mode)) {
		//TODO
		low_proba = 0.18;// 0.18
		high_proba = 1.0 - low_proba;
	} else {

		low_proba = 0.60;// 0.60
		high_proba = 1.0 - low_proba;
	}

	if (simple_value < 0) {
		WARNING("WRONG BT VALUE"<<simple_value);
		m_proba = 0.0;
	}
	else if (simple_value < 1) {
		m_proba = low_proba;
	} else {
		m_proba = high_proba;
	}
}
double patBTMeasurementModel::integral(double ell) {
	return m_proba;
}
void patBTMeasurementModel::initiate() {

}
