/*
 * convex combination of negative exponential distribution and lognormal distribution
 *  patTrafficModelComplex.h
 *  newbioroute
 *
 *  Created by Jingmin Chen on 8/5/10.
 *  Copyright 2010 EPFL. All rights reserved.
 *
 */

#ifndef patTrafficModelComplex_h
#define patTrafficModelComplex_h
#include "patTrafficModel.h"
#include "dataStruct.h"
#include "patType.h"

class patTrafficModelComplex:public patTrafficModel {
public:
	void initiate();
	patReal integral(patReal ell_curr, patReal ell_prev);
};


#endif


