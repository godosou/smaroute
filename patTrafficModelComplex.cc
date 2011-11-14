/*
 *  patTrafficModelComplex.cpp
 *  newbioroute
 *
 *  Created by Jingmin Chen on 8/5/10.
 *  Copyright 2010 EPFL. All rights reserved.
 *
 */
#include "patTrafficModelComplex.h"
#include "patDisplay.h"
#include "patNBParameters.h"
patReal patTrafficModelComplex::integral(patReal ell_curr, patReal ell_prev){
	patReal v;
	if (np.a_total==1){
		if (ell_curr<=ell_prev){
			return 0.0;
		}
		else{
			
			v=3.6*(ell_curr-ell_prev)*np.l_curr/gp.time_diff;
		}
	}
 	v=3.6*(ell_curr*np.l_curr+(1.0-ell_prev)*np.l_prev+np.l_inter)/gp.time_diff;
	if (v<0.0) {
		return 0.0;
	}
	patReal w=patNBParameters::the()->tmcw;
	patReal lambda=patNBParameters::the()->tmclambda;
	patReal mu=patNBParameters::the()->tmcmu;
	patReal sigma=patNBParameters::the()->tmcsigma;
//DEBUG_MESSAGE(v<<","<<w*lambda*exp(-lambda*v)+(1-w)*exp(-pow((log(v)-mu),2)/(2*pow(sigma, 2)))/(v*sigma*sqrt(2*M_PI)));
	/*
	if (v<0.001) {
		return w*lambda*exp(-lambda*v);
	}
	 */
	/*
	if(w*lambda*exp(-lambda*v)+(1-w)*exp(-pow((log(v)-mu),2)/(2*pow(sigma, 2)))/(v*sigma*sqrt(2*M_PI))>1.0){
		
		DEBUG_MESSAGE("error");
	}
	*/
	return w*lambda*exp(-lambda*v)+(1-w)*exp(-pow((log(v)-mu),2)/(2*pow(sigma, 2)))/(v*sigma*sqrt(2*M_PI));
}

void patTrafficModelComplex::initiate(){
	constValue=1.0;
}

