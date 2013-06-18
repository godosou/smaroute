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
	map<TransportMode, TrafficModelParam> patTrafficModelComplex::tm_params;

patReal patTrafficModelComplex::computeSpeedUntilChangePoint(patReal ell_prev,
		patReal t_c) {
	patReal v= 3.6 * ((1.0 - ell_prev) * np.l_prev * np.intermediate_lengths.front())
			/ (t_c - gp.time_prev);

	if (v < 0) {
		WARNING("wrong speed" << ell_prev<<","<<np.l_prev<<","<<np.intermediate_lengths.front()<<","<<t_c<<","<<gp.time_prev);
	}
	return v;

}
patReal patTrafficModelComplex::computeSpeedFromChangePoint(patReal ell_curr,
		patReal t_c) {
	return 3.6 * (ell_curr * np.l_curr * np.intermediate_lengths.back())
			/ (gp.time_curr - t_c);
}
patReal patTrafficModelComplex::computeSpeedBetweenTwoChangePoints(
		patReal t_c_1, patReal t_c_2, int segment) {
	return 3.6 * (np.intermediate_lengths[segment]) / (t_c_2 - t_c_1);
}

patReal patTrafficModelComplex::computeSpeedBetweenTwoLocations(
		patReal ell_prev, patReal ell_curr) {
	patReal v;
	if (np.a_total == 1) {
		if (ell_curr <= ell_prev) {
			return 0.0;
		} else {

			v = 3.6 * (ell_curr - ell_prev) * np.l_curr / gp.time_diff;
			return v;
		}
	} else {
		v = 3.6 * (ell_curr * np.l_curr + (1.0 - ell_prev) * np.l_prev
						+ np.l_inter) / gp.time_diff;
	}
	if (v < 0.0) {
		return 0.0;
	}
	return v;
}
patReal patTrafficModelComplex::integral(patReal ell_curr,patReal ell_prev, 
		patReal t_c) {
	return integral(computeSpeedUntilChangePoint(ell_prev, t_c),
			tm_params[np.transport_modes.front()])
			* integral(computeSpeedFromChangePoint(ell_curr, t_c),
					tm_params[np.transport_modes.back()]);
}

patReal patTrafficModelComplex::integral(patReal ell_curr, patReal  ell_prev){
return integral(computeSpeedBetweenTwoLocations(ell_prev,ell_curr), tm_params[np.transport_modes.front()]);
}
patReal patTrafficModelComplex::integral(patReal v,
		TrafficModelParam tm_param) {

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
	//DEBUG_MESSAGE(tm_param.w<<","<<tm_param.lambda<<","<<tm_param.mu<<","<<tm_param.sigma);
	if (v < 0.0) {
		WARNING("wrong speed" << v);
		return 0.0;
	}
	if (v==0.0){
		return tm_param.w * tm_param.lambda;
	}
	return tm_param.w * tm_param.lambda * exp(-tm_param.lambda * v)
			+ (1 - tm_param.w) * exp(-pow((log(v) - tm_param.mu), 2)/(2* pow(tm_param.sigma, 2))) / (v * tm_param.sigma * sqrt(2 * M_PI));
}

void patTrafficModelComplex::initiate() {
	constValue = 1.0;
}

