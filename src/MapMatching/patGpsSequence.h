/*
 * patGpsSequence.h
 *
 *  Created on: Nov 14, 2011
 *      Author: jchen
 */

#ifndef PATGPSSEQUENCE_H_
#define PATGPSSEQUENCE_H_
#include "patGpsPoint.h"
#include <vector>
#include "patMeasurementSequence.h"
class patGpsSequence {
public:
	patGpsSequence(string gps_file_name, patError*& err);
	virtual ~patGpsSequence();
	/**
	 * Recalculate speed and heading from coordinates.
	 */
	void recalculateSpeedHeading();

	/**
	 * Set the speed type.
	 */
	void setGpsSpeedType();


	int size() const;
	patGpsPoint* at(int i);
	void readFromFile( string file_name,
			patError*& err);
	patGpsPoint* operator[](int i) {
		if (i >= m_gps_vector.size()) {
			return NULL;
		} else {
			return (m_gps_vector[i]);
		}
	}
	const patGpsPoint* operator[](int i) const {
		if (i >= m_gps_vector.size()) {
			return NULL;
		} else {
			return (m_gps_vector[i]);
		}
	}


protected:
	vector<patGpsPoint*> m_gps_vector;

};

#endif /* PATGPSSEQUENCE_H_ */
