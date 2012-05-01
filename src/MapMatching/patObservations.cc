/*
 * patObservations.cc
 *
 *  Created on: Nov 25, 2011
 *      Author: jchen
 */

#include "patObservations.h"
#include "patErrMiscError.h"
#include "patGpsSequence.h"
#include "patAccelSequence.h"
#include "patBTSequence.h"
#include "patNBParameters.h"
#include "patErrMiscError.h"
#include "patDisplay.h"
patObservations::patObservations() {

}

void patObservations::insertToDataVector(
		vector<patMeasurement*>* inserted_vector) {
	vector<patMeasurement*>::iterator inclusive_iter =
			m_measurement_sequence.begin();
	vector<patMeasurement*>::iterator inserted_iter = inserted_vector->begin();

	while (inserted_iter != inserted_vector->end()) {
		while ((*inserted_iter)->getTimeStamp()
				> (*inclusive_iter)->getTimeStamp()) {
			++inclusive_iter;
			if (inclusive_iter == m_measurement_sequence.end()) {
				break;
			}
		}

		inclusive_iter = m_measurement_sequence.insert(inclusive_iter,
				*inserted_iter);
		++inserted_iter;
	}
}
void patObservations::readFromFile(string file_name, patError*& err) {

	vector<patMeasurement*> ordered_all_data;
	string gps_file = patNBParameters::the()->dataDirectory + "/" + file_name
			+ ".csv";
	string bt_file = patNBParameters::the()->dataDirectory + "/" + file_name
			+ "_bt.csv";
	string accel_file = patNBParameters::the()->dataDirectory + "/" + file_name
			+ "_accel.csv";
	m_gps_sequence = new patGpsSequence(gps_file, err);
	if (err != NULL) {
		return;
	}DEBUG_MESSAGE("gps data read.");

	patError* read_file_err = NULL;

	if (patNBParameters::the()->enableBT == 1) {
		m_bt_sequence = new patBTSequence(bt_file, read_file_err);
		if (read_file_err == NULL) {
			//DEBUG_MESSAGE(m_bt_sequence->getPointer()->size());
			DEBUG_MESSAGE("BT is enabled.");
			if (m_measurement_sequence.empty()) {

				m_measurement_sequence = *(m_bt_sequence->getPointer());
			} else {
				if (read_file_err == NULL) {
					insertToDataVector(m_bt_sequence->getPointer());
				}
				//DEBUG_MESSAGE(m_measurement_sequence[0]->getTimeStamp());
				//insertToDataVector(bt_sequence.getPointer());
			}
		}
	}
	if (patNBParameters::the()->enableACCEL == 1) {
		DEBUG_MESSAGE("ACCEL is enabled.");
		read_file_err = NULL;
		m_accel_sequence = new patAccelSequence(accel_file, read_file_err);

		if (m_measurement_sequence.empty()) {

			m_measurement_sequence = *(m_accel_sequence->getPointer());
		} else {
			if (read_file_err == NULL) {
				insertToDataVector(m_accel_sequence->getPointer());
			}
		}
	}
	if (m_measurement_sequence.empty()) {
		DEBUG_MESSAGE("no measurements data read except GPS");
	} else {
		DEBUG_MESSAGE(
				"total number of measurements: " << m_measurement_sequence.size());
	}

}

patObservations::~patObservations() {
	delete m_gps_sequence;
	m_gps_sequence = NULL;
	delete m_bt_sequence;
	m_bt_sequence = NULL;
	;
	delete m_accel_sequence;
	m_bt_sequence = NULL;
}
