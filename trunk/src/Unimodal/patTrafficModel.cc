/*
 *  patTrafficModel.cc
 *  newbioroute
 *
 *  Created by Jingmin Chen on 5/25/10.
 *  Copyright 2010 EPFL. All rights reserved.
 *
 */

#include "patTrafficModel.h"
void patTrafficModel::setParams(struct gps_params tgp, struct network_params tnp){
	gp=tgp;
	np=tnp;
	initiate();
	
}


patReal patTrafficModel::getConstant(void){
	return constValue;
}