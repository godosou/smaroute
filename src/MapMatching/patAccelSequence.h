/*
 * patAccelSequence.h
 *
 *  Created on: Nov 25, 2011
 *      Author: jchen
 */

#ifndef PATACCELSEQUENCE_H_
#define PATACCELSEQUENCE_H_
#include "patMeasurementSequence.h"

class patAccelSequence : public patMeasurementSequence {
public:
	patAccelSequence(string file_name, patError*& err);
	void readFromFile(string file_name, patError*& err);
	virtual ~patAccelSequence();
};

#endif
