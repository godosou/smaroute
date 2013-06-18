/*
 *  patTrafficModelTriangular.h
 *  newbioroute
 *
 *  Created by Jingmin Chen on 5/27/10.
 *  Copyright 2010 EPFL. All rights reserved.
 *
 */

#ifndef patTrafficModelTriangular_h
#define patTrafficModelTriangular_h
#include "patTrafficModel.h"
#include "dataStruct.h"
#include "patType.h"

class patTrafficModelTriangular:public patTrafficModel {
public:
	void initiate();
	patReal integral(patReal ell_curr, patReal ell_prev);
};


#endif


