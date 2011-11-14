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
	void setSpeedType();

	/**
	 * get the size of the gps sequence;
	 */
	int size() const;
	patGpsPoint* operator[](int i) {
		return	&(m_gps_vector[i]);
	}
protected:
vector<patGpsPoint> m_gps_vector;
};

#endif /* PATGPSSEQUENCE_H_ */
