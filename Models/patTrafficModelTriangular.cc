/*
 *  patTrafficModel_Triangular.cpp
 *  newbioroute
 *
 *  Created by Jingmin Chen on 5/27/10.
 *  Copyright 2010 EPFL. All rights reserved.
 *
 */

#include "patTrafficModelTriangular.h"
#include "patDisplay.h"
patReal patTrafficModelTriangular::integral(patReal ell_curr, patReal ell_prev){
	patReal v=(ell_curr*np.l_curr+(1.0-ell_prev)*np.l_prev+np.l_inter)/gp.time_diff;
	patReal s=1.0;
	if (v<0 || v>=gp.max_v_inter){
		return 0.0;
	}
	else if (v==0.0){
		s = gp.p_0;
	}
	else if (v>0 && v<= gp.mu_v_inter) {
		s=(1-gp.p_0)*v/gp.mu_v_inter+gp.p_0;
	}
	else if (v>gp.mu_v_inter && v<gp.max_v_inter){
		s=(gp.max_v_inter-v)/(gp.max_v_inter-gp.mu_v_inter);
	}
	return s;
}

void patTrafficModelTriangular::initiate(){
	constValue= 2.0/(gp.p_0*gp.mu_v_inter+gp.max_v_inter);
	//if (isnan(constValue)){
		//DEBUG_MESSAGE(gp.p_0<<","<<gp.mu_v_inter<<","<<gp.max_v_inter);
		
	//}
}