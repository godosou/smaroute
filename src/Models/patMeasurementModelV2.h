/*
 *  patMeasurementModelV2.h
 *  newbioroute
 *
 *  Created by Jingmin Chen on 5/27/10.
 *  Copyright 2010 EPFL. All rights reserved.
 *
 */

#ifndef patMeasurementModelV2_h
#define patMeasurementModelV2_h
#include "patMeasurementModel.h"
#include "dataStruct.h"
#include "patType.h"
class patMeasurementModelV2:public patMeasurementModel{
public:
	
	void initiate();
	patReal integral(patReal ell_curr, patReal ell_prev);
};


#endif
