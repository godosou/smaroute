/*
 *  patMeasurementModelV2.cc
 *  newbioroute
 *
 *  Created by Jingmin Chen on 5/27/10.
 *  Copyright 2010 EPFL. All rights reserved.
 *
 */

#include "patMeasurementModelV2.h"


patReal patMeasurementModelV2::integral(patReal ell_curr, patReal ell_prev){
	patReal d_curr_2 = 	pow(np.d_curr,2)+pow((ell_curr- np.e_d_curr) * np.l_curr,2);	
	patReal d_prev_2 = pow(np.d_prev,2)+pow((ell_prev - np.e_d_prev) * np.l_prev,2);
	
	return exp(	-((d_curr_2 )/ (2 * pow(gp.std_x_curr,2)))-((d_prev_2)/(2 * pow(gp.std_x_prev,2))));
	
}

void patMeasurementModelV2::initiate(){
	constValue=np.l_curr * np.l_prev;
}