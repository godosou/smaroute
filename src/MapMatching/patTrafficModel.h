/*
 *  patTrafficModel.h
 *  newbioroute
 *
 *  Created by Jingmin Chen on 5/25/10.
 *  Copyright 2010 EPFL. All rights reserved.
 *
 */

#ifndef patTrafficModel_h
#define patTrafficModel_h

#include "patType.h"
#include "dataStruct.h"
#include "patMeasurement.h"
#include "patMultiModalPath.h"
class patTrafficModel {
protected:
	double m_const;
	ArcTranParam m_param;
public:
	patTrafficModel(const patMeasurement* measurement_prev,
			const patMeasurement* measurement_curr,
			const patMultiModalPath* path);
	double getConstant(void) const;

	void setParams(const patMeasurement* measurement_prev,
			const patMeasurement* measurement_curr,
			const patMultiModalPath* path);


	virtual double integral(double a, double b, double c)=0;
	virtual double integral(double ell_prev, double ell_curr)=0;
	virtual void initiate()=0;
};
#endif

