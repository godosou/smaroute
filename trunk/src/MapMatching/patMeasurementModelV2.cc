/*
 *  patMeasurementModelV2.cc
 *  newbioroute
 *
 *  Created by Jingmin Chen on 5/27/10.
 *  Copyright 2010 EPFL. All rights reserved.
 *
 */

#include "patMeasurementModelV2.h"
#include "patDisplay.h"
patMeasurementModelV2::patMeasurementModelV2(patMeasurement* measurement,
		const patArc* arc, TransportMode mode) :
		patMeasurementModel::patMeasurementModel(measurement, arc, mode) {

	initiate();
}
double patMeasurementModelV2::integral(double ell_curr) {
	//DEBUG_MESSAGE(m_param.distance_to_arc<<" "<<m_param.foot_on_arc<<" "<<m_param.length_of_arc<<" "<<m_param.std_measurement);
	double d_curr_2 = pow(m_param.distance_to_arc, 2.0)
			+ pow((ell_curr - m_param.foot_on_arc) * m_param.length_of_arc, 2.0);

	double rtn= exp(-(d_curr_2) / (2.0 * pow(m_param.std_measurement, 2.0)));
	if (rtn>=1.0){
		WARNING("WRONG");
	}
	return rtn;
}

void patMeasurementModelV2::initiate() {
	m_const = m_param.length_of_arc;
}
