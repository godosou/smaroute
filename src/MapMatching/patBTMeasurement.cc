/*
 * patBTMeasurement.cc
 *
 *  Created on: Nov 25, 2011
 *      Author: jchen
 */

#include "patBTMeasurement.h"
#include "patBTDDR.h"
#include "patTransportMode.h"
#include "patArc.h"
#include "dataStruct.h"
#include "patDisplay.h"
patBTMeasurement::patBTMeasurement(double bt_time, int bt_amount) {
	m_measurement_type = MeasurementType(BT);
	m_timestamp=bt_time;
	m_nearby_bt_amount = bt_amount;
	//DEBUG_MESSAGE(m_timestamp<<","<<m_nearby_bt_amount);
	m_ddr = new patBTDDR(this);

}

int patBTMeasurement::getNearbyBTAmount() const{
	return m_nearby_bt_amount;
}
patBTMeasurement::~patBTMeasurement() {
}


void patBTMeasurement::setMeasurementType(){
	m_measurement_type=MeasurementType(BT);
}

MeasurementModelParam patBTMeasurement::getMeasurementModelParam(const patArc* arc,
		TransportMode mode) const {
	MeasurementModelParam param;
	param.mode = mode;
	param.length_of_arc = arc->getLength();
	param.foot_on_arc = 0.5;
	param.distance_to_arc = 0.0;
	return param;
}
double patBTMeasurement::getSimpleValue() const{
	return m_nearby_bt_amount;
}

bool patBTMeasurement::isGPS() const{
	return false;
}
 ostream& operator<<(ostream& str, const patBTMeasurement& x){
 		str.precision(15);
	str << "BT Time:" << x.m_timestamp << ", " << x.m_nearby_bt_amount<<endl;

 }