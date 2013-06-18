/*
 * File:   patBTMeasurementModel.h
 * Author: jchen
 *
 * Created on May 4, 2011, 4:34 PM
 */

#ifndef PATBTMEASUREMENTMODEL_H
#define	PATBTMEASUREMENTMODEL_H
#include <vector>
#include "patType.h"
#include "patMeasurementModel.h"
#include "patMeasurement.h"
#include "patArc.h"
#include "patTransportMode.h"
class patBTMeasurementModel: public patMeasurementModel {
public:
	patBTMeasurementModel(patMeasurement* measurement, const patArc* arc,
			TransportMode mode);
	double calProba(unsigned long mode, unsigned long number);
	double integral(double ell);
	void initiate();
private:
	vector<unsigned long> threshold;
	vector<vector<double> > proba_matrix;
};

#endif	/* PATBTMEASUREMENTMODEL_H */

