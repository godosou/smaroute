/*
 *  patMeasurementModel.cc
 *  newbioroute
 *
 *  Created by Jingmin Chen on 5/27/10.
 *  Copyright 2010 EPFL. All rights reserved.
 *
 */

#include "patMeasurementModel.h"

void patMeasurementModel::setParams(struct gps_params tgp, struct network_params tnp){
	gp=tgp;
	np=tnp;
	initiate();
}

patReal patMeasurementModel::getConstant(void){
	return constValue;
}