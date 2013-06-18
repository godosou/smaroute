/*
 *  patMeasurementModel.h
 *  newbioroute
 *
 *  Created by Jingmin Chen on 5/27/10.
 *  Copyright 2010 EPFL. All rights reserved.
 *
 */

#ifndef patMeasurementModel_h
#define patMeasurementModel_h
#include "patType.h"
#include "dataStruct.h"
#include "patArc.h"
#include "patMeasurement.h"
#include "patTransportMode.h"
class patMeasurementModel {
protected:
	double m_const;
	MeasurementModelParam m_param;
	double m_proba;

public:

	patMeasurementModel(patMeasurement* measurement, const patArc* arc, TransportMode mode);

	void setParams(patMeasurement* measurement, const patArc* arc, TransportMode mode);
	double getConstant(void);
	double getProba(){
		return m_proba;
	}
	virtual double getModeSimpleProba(TransportMode mode,double simple_value){
		return 0.0;
	}
	virtual double integral(double ell_curr)=0;
	virtual void initiate()=0;
};

#endif

