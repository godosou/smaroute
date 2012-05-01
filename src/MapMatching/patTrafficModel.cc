/*
 *  patTrafficModel.cc
 *  newbioroute
 *
 *  Created by Jingmin Chen on 5/25/10.
 *  Copyright 2010 EPFL. All rights reserved.
 *
 */

#include "patTrafficModel.h"
#include "patTransportMode.h"
#include "patArc.h"
#include "patDisplay.h"
#include "patNBParameters.h"
double patTrafficModel::getConstant(void) const {
	return m_const;
}

void patTrafficModel::setParams(const patMeasurement* measurement_prev,
		const patMeasurement* measurement_curr, const patMultiModalPath* path) {
//TODO need to elaborate.
	m_param.time_prev = measurement_prev->getTimeStamp();
	m_param.time_curr = measurement_curr->getTimeStamp();

	m_param.time_diff = m_param.time_curr - m_param.time_prev;

	m_param.length_prev = path->front()->getLength();
	m_param.length_curr = path->back()->getLength();

	m_param.arc_amount = path->size();
	if (m_param.arc_amount <= 2) {
		//DEBUG_MESSAGE("only one arc");
		m_param.inter_length = 0.0;
	} else {
		m_param.inter_length = path->getLength() - m_param.length_prev
				- m_param.length_curr;
	}

	m_param.transport_modes = path->getUnimodalModes();

	if (m_param.transport_modes.size() == 2) {
		m_param.inter_seg_lengths = path->getIntermediateUnimodalLengths();

		if (m_param.inter_seg_lengths.size() > 2) {
			WARNING(
					"More than one change" << m_param.inter_seg_lengths.size()
							<< "," << m_param.transport_modes.size());
		}
		double total_length = m_param.inter_seg_lengths.front()
				+ m_param.inter_seg_lengths.back() + m_param.length_prev
				+ m_param.length_curr;
		if (total_length - path->getLength() > 0.000001 || total_length - path->getLength() < -0.000001 ) {
			WARNING(
					path->getLength() << "!=" << total_length << "="
							<< m_param.inter_seg_lengths.front() << "+"
							<< m_param.inter_seg_lengths.back() << "+"
							<< m_param.length_prev << "+"
							<< m_param.length_curr);
		}

	}

	m_param.change_penalty = 0.0;
	if (m_param.transport_modes.front() != m_param.transport_modes.back()) {
		m_param.change_penalty = patNBParameters::the()->modeChangePenalty;
	}
	/*
	 if (m_param.transport_modes.front() == TransportMode(CAR)
	 && m_param.transport_modes.back() == TransportMode(BUS)) {
	 m_param.change_penalty = 0.0;
	 }
	 if (m_param.transport_modes.back() == TransportMode(CAR)
	 && m_param.transport_modes.front() == TransportMode(BUS)) {
	 m_param.change_penalty = 0.0;
	 }
	 */
	m_param.inter_stop_time = path->getTotalStopTime()
			- path->back_road_travel().stop_time;
	if (m_param.inter_stop_time > 0.0 || m_param.inter_stop_time < 0.0) {
		DEBUG_MESSAGE("inter stop time" << m_param.inter_stop_time);
	}

}
patTrafficModel::patTrafficModel(const patMeasurement* measurement_prev,
		const patMeasurement* measurement_curr, const patMultiModalPath* path) {
	setParams(measurement_prev, measurement_curr, path);
}
