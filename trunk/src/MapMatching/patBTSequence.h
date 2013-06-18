/*
 * patBTSequence.h
 *
 *  Created on: Nov 25, 2011
 *      Author: jchen
 */

#ifndef PATBTSEQUENCE_H_
#define PATBTSEQUENCE_H_

#include "patBTMeasurement.h"
#include "patMeasurementSequence.h"
class patBTSequence: public patMeasurementSequence {
public:
	patBTSequence(string gps_file_name, patError*& err);
	void readFromFile(string gps_file_name, patError*& err);
	virtual ~patBTSequence();

protected:
};

#endif /* PATBTSEQUENCE_H_ */
