/*
 * patObservations.h
 *
 *  Created on: Nov 25, 2011
 *      Author: jchen
 */

#ifndef PATOBSERVATIONS_H_
#define PATOBSERVATIONS_H_
#include "patMeasurementSequence.h"
#include "patError.h"
#include "patMeasurement.h"
#include "patBTSequence.h"
#include "patAccelSequence.h"
class patGpsSequence;
class patObservations {
public:
	patObservations();
	void insertToDataVector(vector<patMeasurement*>* inserted_vector);
	void readFromFile(string file_name, patError*& err);
	virtual ~patObservations();
	patGpsSequence* m_gps_sequence;
	patBTSequence* m_bt_sequence;
	patAccelSequence* m_accel_sequence;
	vector<patMeasurement*> m_measurement_sequence;
protected:
};

#endif /* PATOBSERVATIONS_H_ */
