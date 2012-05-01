/*
 * patMeasurementSequence.h
 *
 *  Created on: Nov 25, 2011
 *      Author: jchen
 */

#ifndef PATMEASUREMENTSEQUENCE_H_
#define PATMEASUREMENTSEQUENCE_H_
using namespace std;

#include "patMeasurement.h"
#include "patError.h"
#include <vector>
class patMeasurementSequence {
public:
	patMeasurementSequence();

	virtual void readFromFile(string file_name, patError*& err) = 0;
	vector<patMeasurement*>* getPointer();
	virtual ~patMeasurementSequence();

	int size() const;
	patMeasurement* operator[](int i) {
		if (i >= m_measurement_vector.size()) {
			return NULL;
		} else {
			return (m_measurement_vector[i]);
		}
	}
	const patMeasurement* operator[](int i) const {
		if (i >= m_measurement_vector.size()) {
			return NULL;
		} else {
			return (m_measurement_vector[i]);
		}
	}

protected:
	vector<patMeasurement*> m_measurement_vector;

};

#endif /* PATMEASUREMENTSEQUENCE_H_ */
