/*
 *  patMeasurementModelV1.h
 *  newbioroute
 *
 *  Created by Jingmin Chen on 5/27/10.
 *  Copyright 2010 EPFL. All rights reserved.
 *
 */

#ifndef patMeasurementModelV1_h
#define patMeasurementModelV1_h
#include "patMeasurementModel.h"
#include "dataStruct.h"
class patMeasurementModelV1:public patMeasurementModel {
public:
	void initiate();
	patReal integral(patReal ell_curr, patReal ell_prev);
};


#endif