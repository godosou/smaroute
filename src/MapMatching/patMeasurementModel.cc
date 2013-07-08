/*
 *  patMeasurementModel.cc
 *  newbioroute
 *
 *  Created by Jingmin Chen on 5/27/10.
 *  Copyright 2010 EPFL. All rights reserved.
 *
 */

#include "patMeasurementModel.h"
#include "patDisplay.h"
double patMeasurementModel::getConstant(void) {

	return m_const;
}

void patMeasurementModel::setParams(patMeasurement* measurement,
		const patArc* arc, TransportMode mode) {
	m_param = measurement->getMeasurementModelParam(arc, mode);

}

patMeasurementModel::patMeasurementModel(patMeasurement* measurement,
		const patArc* arc, TransportMode mode) {
	//DEBUG_MESSAGE("abstract measuremenet model");
	setParams(measurement, arc, mode);
}
