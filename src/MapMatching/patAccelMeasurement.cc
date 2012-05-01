/*
 * patAccelMeasurement.cc
 *
 *  Created on: Nov 25, 2011
 *      Author: jchen
 */

#include "patAccelMeasurement.h"
#include "patAccelDDR.h"
patAccelMeasurement::patAccelMeasurement(double accel_time,
		double accel) {


	m_measurement_type = ACCEL;
	m_timestamp = accel_time;
	m_accel = accel;

	m_ddr = new patAccelDDR(this);
}


double patAccelMeasurement::getAccel() const {
	return m_accel;
}
void patAccelMeasurement::setMeasurementType() {
	m_measurement_type = MeasurementType(ACCEL);
}

MeasurementModelParam patAccelMeasurement::getMeasurementModelParam(const patArc* arc,
		TransportMode mode) const {
	MeasurementModelParam param;
	param.mode = mode;
	param.length_of_arc = arc->getLength();
	param.foot_on_arc = 0.5;
	param.distance_to_arc = 0.0;
	return param;
}

double patAccelMeasurement::getSimpleValue() const{
	return m_accel;
}

bool patAccelMeasurement::isGPS() const{
	return false;
}

 ostream& operator<<(ostream& str, const patAccelMeasurement& x){
 		str.precision(15);
	str << "ACCEL Time:" << x.m_timestamp << ", " << x.m_accel<<endl;

 }