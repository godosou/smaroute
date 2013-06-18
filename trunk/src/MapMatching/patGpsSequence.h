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
#include "patGeoBoundingBox.h"
class patGpsSequence {
public:
	patGpsSequence(string gps_file_name);
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
	void readFromFile(string file_name);
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

	/**
	 * Compute the length of the track according to the cumulative distance between them.
	 */
	double computeLength() const;

	/**
	 * Compute the distance of each point a road (arc or path)
	 */
	vector<double> distanceTo(const patRoadBase& road) const;

	/**
	 * Compute the bounding box of the gps sequence with margin
	 * @param margin: the margin of the box
	 */
	patGeoBoundingBox computeBoundingBox(const double& margin) const;
	const patGpsPoint* getFirstGps() const;
	const patGpsPoint* getLastGps() const;
protected:
	vector<patGpsPoint*> m_gps_vector;

};

#endif /* PATGPSSEQUENCE_H_ */
