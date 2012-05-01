/*
 * patProbabilisticMapMatching.cc
 *
 *  Created on: Nov 10, 2011
 *      Author: jchen
 */

#include "patProbabilisticMapMatching.h"
#include "patReadGpsFromCsv.h"
#include "patDisplay.h"
#include<iostream>
#include<sstream>
#include<fstream>
#include "patMapMatchingIteration.h"
#include "patNBParameters.h"
#include "patMultiModalPathProba.h"
patProbabilisticMapMatching::patProbabilisticMapMatching(
		const patNetworkEnvironment* environment, string file_name,
		patError*& err) :
		m_environment(environment), m_file_name(file_name) {
}

patProbabilisticMapMatching::~patProbabilisticMapMatching() {
}
void patProbabilisticMapMatching::setNetworkEnvironment(
		const patNetworkEnvironment* environment) {
	m_environment = environment;
}
void patProbabilisticMapMatching::run(patError*& err) {
	/*
	 * Algorithm:
	 * Loop over all GPS point, ignore empty GPS point.
	 */

	m_observations.readFromFile(m_file_name, err);
	if (err != NULL) {
		return;
	}DEBUG_MESSAGE("Prepare to generate DDR. ");
	DEBUG_MESSAGE("Raw points:" << m_observations.m_gps_sequence->size());
	DEBUG_MESSAGE(
			"first point. " << m_observations.m_gps_sequence->at(0)->getTimeStamp());
	patGpsPoint* prevGpsPoint;
	int first_valid = 0;

	for (; first_valid < m_observations.m_gps_sequence->size(); ++first_valid) {
		DEBUG_MESSAGE("====seek first valid gps point: " << first_valid);
		int next = first_valid + 1;
		if (next == (m_observations.m_gps_sequence->size())) {
			next = first_valid;
		}
		m_running_measurement_sequence.push_back(
				m_observations.m_gps_sequence->at(first_valid));
		patMapMatchingIteration init_iteration(m_environment,
				m_observations.m_gps_sequence->at(first_valid),
				&m_running_measurement_sequence, m_paths);
		init_iteration.firstIteration();

		if (m_observations.m_gps_sequence->at(first_valid)->getDDR()->empty()) {
			DEBUG_MESSAGE(
					"No domain at:" << m_observations.m_gps_sequence->at(first_valid)->getTimeStamp());
			return; //FIXME
		} else {
			m_paths = init_iteration.getPaths();
			init_iteration.writeKML(m_file_name, first_valid);
			break;
		}
	}

	DEBUG_MESSAGE(
			"=======First valid gps point:" << first_valid << "," << m_observations.m_gps_sequence->at(first_valid)->getTimeStamp());
	if (first_valid == m_observations.m_gps_sequence->size()) {
		return;
	}
	prevGpsPoint = m_observations.m_gps_sequence->at(first_valid);

	vector<patMeasurement*> inter_data;
	int measurements_id = 0;
	int fail_iter= 0;
	DEBUG_MESSAGE(
			"observations: " << m_observations.m_measurement_sequence.size());
	for (int i = first_valid + 1; i < m_observations.m_gps_sequence->size();
			++i) {
		DEBUG_MESSAGE(
				"==========Start " << i + 1 << "th iteration ============");
		//DEBUG_MESSAGE("1 "<<measurements_id<<" "<<m_observations.m_measurement_sequence[measurements_id]->getTimeStamp());
		while (measurements_id < m_observations.m_measurement_sequence.size()
				&& m_observations.m_measurement_sequence[measurements_id]->getTimeStamp()
						<= m_observations.m_gps_sequence->at(i - 1)->getTimeStamp()) {
			++measurements_id;
		}
		//DEBUG_MESSAGE("2 "<<measurements_id<<" "<<m_observations.m_measurement_sequence[measurements_id]->getTimeStamp());
		while (measurements_id < m_observations.m_measurement_sequence.size()
				&& m_observations.m_measurement_sequence[measurements_id]->getTimeStamp()
						<= m_observations.m_gps_sequence->at(i)->getTimeStamp()) {
			//DEBUG_MESSAGE("3 "<<measurements_id<<" "<<m_observations.m_measurement_sequence[measurements_id]->getTimeStamp());
			if (m_observations.m_gps_sequence->at(i)->getTimeStamp() - m_observations.m_gps_sequence->at(i-1)->getTimeStamp()<=
				patNBParameters::the()->discardGPSGap){
				inter_data.push_back(
						m_observations.m_measurement_sequence[measurements_id]);
			}
			else{
				DEBUG_MESSAGE("additional data doesn't have enough prior.");
			}
			++measurements_id;

		}
		//DEBUG_MESSAGE("inserted data: " << measurements_id);
		/*if (m_observations[i]->getType() != patString("normal_speed")
		 && i != m_observations.size() - 1) {

		 //lowSpeedGpsPoints.push_back(m_observations[i]);
		 } else {
		 */
		DEBUG_MESSAGE("inter data:" << inter_data.size());
		DEBUG_MESSAGE(*m_observations.m_gps_sequence->at(i));
		m_running_measurement_sequence.push_back(
				m_observations.m_gps_sequence->at(i));
		patMapMatchingIteration mm_iteration(m_environment,
				m_observations.m_gps_sequence->at(i),
				&m_running_measurement_sequence, m_paths);
		bool extend_new_seg = mm_iteration.normalIteration(prevGpsPoint,
				inter_data, err);

		m_paths = mm_iteration.getPaths();
		if (extend_new_seg == false) {
			fail_iter+=1;
			if (fail_iter>20){
				break;
			}
			//If the path is not extended;
			patMeasurement* last_measurement =
					m_running_measurement_sequence.back();
			inter_data.push_back(last_measurement);
			m_running_measurement_sequence.pop_back();
			if (i == m_observations.m_gps_sequence->size() - 1) {
				inter_data.pop_back();
				m_running_measurement_sequence.push_back(
						m_observations.m_gps_sequence->at(i));
				patMapMatchingIteration mm_iteration(m_environment,
						m_observations.m_gps_sequence->at(i),
						&m_running_measurement_sequence,
						m_paths);
				mm_iteration.lastIterationForLowSpeed(inter_data, prevGpsPoint);

				mm_iteration.writeKML(m_file_name, 1000000);
			}

		} else {
			fail_iter=0;
			inter_data.clear();
			if (patNBParameters::the()->printKMLEveryIteration == 1) {
				mm_iteration.writeKML(m_file_name, i);
			}
			if (i == m_observations.m_gps_sequence->size() - 1) {
				mm_iteration.writeKML(m_file_name, 1000000);
			}

			if (err != NULL) {
				//implement better error handling
				m_running_measurement_sequence.pop_back();
				return;
			}DEBUG_MESSAGE(
					"Link DDR number:" << m_observations.m_gps_sequence->at(i)->getDDR()->size());

			if (!m_observations.m_gps_sequence->at(i)->getDDR()->empty()) {

				DEBUG_MESSAGE("path number:" << m_paths.size());
				DEBUG_MESSAGE(
						"Found Domain at (" << i + 1 << ") " << m_observations.m_gps_sequence->at(i)->getTimeStamp());
				inter_data.clear();
				prevGpsPoint = m_observations.m_gps_sequence->at(i);
			} else {
				DEBUG_MESSAGE(
						"No domain at:" << m_observations.m_gps_sequence->at(i)->getTimeStamp());
			}
		}
		//}
		DEBUG_MESSAGE(
				"==========Finish " << i + 1 << "th iteration ============");

	}DEBUG_MESSAGE("Finish map matching");

}

