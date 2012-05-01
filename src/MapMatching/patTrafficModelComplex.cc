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
#include "patErrMiscError.h"
#include "patSpeedDistributions.h"


patTrafficModelComplex::patTrafficModelComplex(
		const patMeasurement* measurement_prev,
		const patMeasurement* measurement_curr, const patMultiModalPath* path) :
		patTrafficModel::patTrafficModel(measurement_prev, measurement_curr,
				path) {
	initiate();

}
double patTrafficModelComplex::computeSpeedUntilChangePoint(double ell_prev,
		double t_c) {
//	DEBUG_MESSAGE(m_param.inter_seg_lengths.size());
	if (t_c - m_param.time_prev <= 0.0) {
		//WARNING("Wrong time");
		return -1.0;
	}
	double v = 3.6
			* ((1.0 - ell_prev) * m_param.length_prev
					+ m_param.inter_seg_lengths.front())
			/ (t_c - m_param.time_prev);

	if (v < 0.0) {
		WARNING(
				"wrong speed" << ell_prev << "," << m_param.length_prev << ","
						<< m_param.inter_seg_lengths.front() << "," << t_c
						<< "," << m_param.time_prev);
	}
	return v;

}
double patTrafficModelComplex::computeSpeedFromChangePoint(double ell_curr,
		double t_c) {
	double t = m_param.time_curr - t_c - m_param.change_penalty
			- m_param.inter_stop_time;
	if (t < 0.0) {
		//WARNING("Wrong time");
		return -1.0;
	}
	return 3.6
			* (ell_curr * m_param.length_curr + m_param.inter_seg_lengths.back())
			/ t;
}
double patTrafficModelComplex::computeSpeedBetweenTwoChangePoints(double t_c_1,
		double t_c_2, int segment) {
	return 3.6 * (m_param.inter_seg_lengths[segment]) / (t_c_2 - t_c_1);
}

double patTrafficModelComplex::computeSpeedBetweenTwoLocations(double ell_prev,
		double ell_curr) {
	double v;
	/*
	 DEBUG_MESSAGE(m_param.length_curr<<" "<<m_param.time_diff<<" "<<m_param.arc_amount<<" "<<m_param.length_prev
	 <<" "<<m_param.inter_length<<" "<<m_param.inter_stop_time);
	 */
	if (m_param.arc_amount == 1) {
		if (ell_curr < ell_prev) {
			return -1.0;
		} else {

			v = 3.6 * (ell_curr - ell_prev) * m_param.length_curr
					/ m_param.time_diff;
			return v;
		}
	} else {
		v = 3.6
				* (ell_curr * m_param.length_curr
						+ (1.0 - ell_prev) * m_param.length_prev
						+ m_param.inter_length)
				/ (m_param.time_diff - m_param.inter_stop_time);
	}
	return v;
}
double patTrafficModelComplex::integral(double ell_prev, double ell_curr,
		double t_c) {
	double rtn =
			patSpeedDistributions::the()->pdf(
					computeSpeedUntilChangePoint(ell_prev, t_c),
					m_param.transport_modes.front())
					* patSpeedDistributions::the()->pdf(
							computeSpeedFromChangePoint(ell_curr, t_c),
							m_param.transport_modes.back());
	if (rtn >= 1.0) {
		WARNING("WRONG");
	}
	return rtn;
}

double patTrafficModelComplex::integral(double ell_prev, double ell_curr) {
	return patSpeedDistributions::the()->pdf(computeSpeedBetweenTwoLocations(ell_prev, ell_curr),
			m_param.transport_modes.front());
}
void patTrafficModelComplex::initiate() {
	m_const = 1.0;
}
