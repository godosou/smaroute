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

class patMeasurementModel {
protected:
	patReal constValue;
	struct gps_params gp;
	struct network_params np;
public:
	void setParams(struct gps_params tgp, struct network_params tnp);
	patReal getConstant(void);
	virtual patReal integral(patReal ell_curr, patReal ell_prev)=0;
	virtual void initiate()=0;
};

#endif

