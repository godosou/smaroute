/*
 * patAccelMeasurementModel.h
 *
 *  Created on: Nov 26, 2011
 *      Author: jchen
 */

#ifndef PATACCELMEASUREMENTMODEL_H_
#define PATACCELMEASUREMENTMODEL_H_
#include "patMeasurementModel.h"
#include "patError.h"
#include "patTransportMode.h"
#include "dataStruct.h"
class patAccelMeasurementModel :public patMeasurementModel {
public:
	patAccelMeasurementModel(patMeasurement* measurement, const patArc* arc,
			TransportMode mode,double d);
	double integral(double ell);
	void initiate();	
	static map<TransportMode, AccelModelParam> m_params;
	static void readParams( patError*& err);
private:
	TransportMode m_mode;
	double m_stationary_proba;
	AccelModelParam m_param;
	double m_distance_to_stop;
	double m_arc_length;
};

#endif /* PATACCELMEASUREMENTMODEL_H_ */
