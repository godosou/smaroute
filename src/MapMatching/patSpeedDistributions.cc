/*
 * patSpeedDistributions.cc
 *
 *  Created on: Mar 26, 2012
 *      Author: jchen
 */

#include "patSpeedDistributions.h"
#include "patDisplay.h"
#include "patNBParameters.h"
#include "patErrMiscError.h"

#include <boost/math/distributions/normal.hpp>
#include <boost/math/distributions/lognormal.hpp> // for normal_distribution
#include <boost/math/distributions/exponential.hpp> // for exponential distribution
#include "patDisplay.h"
#include "patError.h"
#include <sstream>
#include <vector>
#include <cstdlib>
using boost::math::normal;
using boost::math::lognormal;
using boost::math::exponential_distribution;
 map<TransportMode, TrafficModelParam> patSpeedDistributions::tm_params;
patSpeedDistributions::patSpeedDistributions() {

}

patSpeedDistributions::~patSpeedDistributions() {
}

patSpeedDistributions* patSpeedDistributions::ins = NULL;

patSpeedDistributions* patSpeedDistributions::the() {
	if (patSpeedDistributions::ins == NULL) {
		patSpeedDistributions::ins = new patSpeedDistributions;
	}
	return patSpeedDistributions::ins;
}
double patSpeedDistributions::pdf(double v, TransportMode mode) {
	TrafficModelParam tm_param = patSpeedDistributions::tm_params[mode];
	double rtn = 0.0;
	if (v < 0.0) {
//		WARNING("wrong speed" << v);
		rtn = 0.0;
	} else if (v == 0.0) {
		rtn = tm_param.w * tm_param.lambda;
	} else {

		if (tm_param.mode == TransportMode(WALK)) {
			boost::math::exponential e(tm_param.lambda);	
			boost::math::normal n(tm_param.mu,tm_param.sigma);
	 		
			rtn = tm_param.w * boost::math::pdf(e,v )  + (1.0-tm_param.w) *boost::math::pdf(n,v ) ;
			// rtn = tm_param.w * tm_param.lambda * exp(-tm_param.lambda * v)
			// 		+ (1.0 - tm_param.w)
			// 				* exp(-(v - tm_param.mu) * (v - tm_param.mu) /(2.0 * tm_param.sigma * tm_param.sigma))
			// 				/ (tm_param.sigma * sqrt(2.0 * 	M_PI) );
		} else {
			boost::math::exponential e(tm_param.lambda );
			boost::math::lognormal n(tm_param.mu,tm_param.sigma);
			 rtn = tm_param.w * boost::math::pdf(e,v )  + (1.0-tm_param.w) *boost::math::pdf(n,v ) ;
			// rtn = tm_param.w * tm_param.lambda * exp(-tm_param.lambda * v)
			// 		+ (1.0 - tm_param.w)
			// 				* exp( -(log(v) - tm_param.mu) * (log(v) - tm_param.mu) /(2.0 * tm_param.sigma * tm_param.sigma))
			// 				/ (v * tm_param.sigma * sqrt(2.0 * M_PI));
		}
	}
	if (rtn >= 1.0 || rtn < 0.0) {
		WARNING("WRONG");
	}
	return rtn;
}
void patSpeedDistributions::readParams(patError*& err) {

	vector<TransportMode> modes;
	modes.push_back(TransportMode(CAR));
	modes.push_back(TransportMode(BUS));
	modes.push_back(TransportMode(METRO));
	modes.push_back(TransportMode(TRAIN));
	modes.push_back(TransportMode(WALK));
	modes.push_back(TransportMode(BIKE));
	for (int i = 0; i < modes.size(); ++i) {
		TrafficModelParam params;
		params.mode = modes[i];
		string file_name = patNBParameters::the()->paramFolder + "speed/"
				+ getTransportModeString(modes[i]);
		//patAccelMeasurementModel::m_params[modes[i]];
		ifstream file_stream_handler;
		file_stream_handler.open(file_name.c_str(), ios::in);
		if (!file_stream_handler) {
			stringstream str;
			str << "Error while parsing " << file_name;
			err = new patErrMiscError(str.str());
			WARNING(err->describe());
			return;
		}
//		DEBUG_MESSAGE("Read file:" << file_name);
		int components = 0;
		string line;
		if (getline(file_stream_handler, line)) {

			istringstream linestream(line);
			string item;

			getline(linestream, item, ',');
			//	DEBUG_MESSAGE(item);
			params.w = atof(item.c_str());

			getline(linestream, item, ',');
			//DEBUG_MESSAGE(item);
			params.lambda = atof(item.c_str());
			//		DEBUG_MESSAGE(item);

			getline(linestream, item, ',');
			params.mu = atof(item.c_str());
//			DEBUG_MESSAGE(item);

			getline(linestream, item, ',');
			params.sigma = atof(item.c_str());

//			DEBUG_MESSAGE(
//					params.w << "," << params.lambda << "," << params.mu << ","
//							<< params.sigma)
			patSpeedDistributions::tm_params[modes[i]] = params;
		} else {
			stringstream str;
			str << "Speed distribution for  " << modes[i] << "is wrong";
			err = new patErrMiscError(str.str());
			WARNING(err->describe());
			return;

		}

	}
}
