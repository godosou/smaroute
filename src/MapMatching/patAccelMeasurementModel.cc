/*
 * patAccelMeasurementModel.cc
 *
 *  Created on: Nov 26, 2011
 *      Author: jchen
 */

#include "patAccelMeasurementModel.h"
#include "patNBParameters.h"
#include "patDisplay.h"
#include "patErrMiscError.h"
#include "patMixtureNormal.h"
#include<iostream>
#include<sstream>
#include<fstream>
#include "patError.h"
#include <boost/math/distributions/normal.hpp> // for normal_distribution
using boost::math::normal;
// typedef provides default type is double.
map<TransportMode, AccelModelParam> patAccelMeasurementModel::m_params;

void patAccelMeasurementModel::readParams(patError*& err) {

	vector<TransportMode> modes;
	modes.push_back(TransportMode(CAR));
	modes.push_back(TransportMode(BUS));
	modes.push_back(TransportMode(METRO));
	modes.push_back(TransportMode(TRAIN));
	modes.push_back(TransportMode(WALK));
	modes.push_back(TransportMode(BIKE));
	for (int i = 0; i < modes.size(); ++i) {
		string file_name = patNBParameters::the()->paramFolder + "accel/"
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
		}DEBUG_MESSAGE("Read file:" << file_name);
		int components = 0;
		vector<double> w, mu, sigma;
		double w1, mu1, sigma1;
		string line;
		while (getline(file_stream_handler, line)) {

			components++;
			istringstream linestream(line);

			string item;

			getline(linestream, item, ',');
			w1 = atof(item.c_str());

			getline(linestream, item, ',');
			mu1 = atof(item.c_str());

			getline(linestream, item, '\n');
			sigma1 = atof(item.c_str());

			DEBUG_MESSAGE(w1<<","<<mu1<<","<<sigma1)
			w.push_back(w1);
			mu.push_back(mu1);
			sigma.push_back(sigma1);
		}
		file_stream_handler.close();
		patAccelMeasurementModel::m_params[modes[i]].components = components;
		patAccelMeasurementModel::m_params[modes[i]].w = w;
		patAccelMeasurementModel::m_params[modes[i]].mu = mu;
		patAccelMeasurementModel::m_params[modes[i]].sigma = sigma;

	}
}

patAccelMeasurementModel::patAccelMeasurementModel(patMeasurement* measurement,
		const patArc* arc, TransportMode mode, double d) :
		patMeasurementModel::patMeasurementModel(measurement, arc, mode) {
	initiate();
	m_distance_to_stop = d;

	m_mode = mode;
	m_const = arc->getLength();
	m_arc_length = arc->getLength();

	patError* err;
	patMixtureNormal mn(patAccelMeasurementModel::m_params[m_mode].components,
		patAccelMeasurementModel::m_params[m_mode].w,
		patAccelMeasurementModel::m_params[m_mode].mu,
		patAccelMeasurementModel::m_params[m_mode].sigma,
		err);

	m_proba = mn.pdf(measurement->getSimpleValue());
	// m_param = patAccelMeasurementModel::m_params[m_mode];
	// normal s;
	// m_proba = 0.0;
	// for (int i = 0; i < patAccelMeasurementModel::m_params[m_mode].components;
	// 		++i) {

	// 	m_proba += m_param.w[i]
	// 			* pdf(
	// 					s,
	// 					(measurement->getSimpleValue() - m_param.mu[i])
	// 							/ m_param.sigma[i]) ;
	// }

	m_stationary_proba = -1.0;
	/*
	if (patAccelMeasurementModel::m_params.find(TransportMode(STATIONARY))
			!= patAccelMeasurementModel::m_params.end())

			{
		m_stationary_proba = 0.0;
		for (int i = 0;
				i
						< patAccelMeasurementModel::m_params[TransportMode(
								STATIONARY)].components; ++i) {

			m_stationary_proba += m_param.w[i]
					* pdf(
							s,
							(measurement->getSimpleValue() - m_param.mu[i])
									/ m_param.sigma[i]) / m_param.sigma[i];
		}
	}
	*/
	//DEBUG_MESSAGE(getTransportMode(m_mode)<<","<<m_proba);
	//DEBUG_MESSAGE(m_proba);
}
/*
 double patAccelMeasurementModel::integral(double ell) {
 //DEBUG_MESSAGE(ell << ", "<< std_normal << ", " << rtn);
 return m_proba;

 //return m_proba;
 }
 */
double patAccelMeasurementModel::integral(double ell) {
	if (m_stationary_proba < 0.0) {
		return m_proba;
	} else {
		double distance = ell * (m_arc_length) + m_distance_to_stop;
		double stationary_prior_proba = patNBParameters::the()->stationaryPriorMu
				* exp(
						-patNBParameters::the()->stationaryPriorMu * distance
								/ 100.0);

		return stationary_prior_proba * m_stationary_proba
				+ (1.0 - m_stationary_proba) * m_proba;
	}
}
void patAccelMeasurementModel::initiate() {
}
