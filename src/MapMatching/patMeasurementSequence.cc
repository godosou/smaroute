/*
 * patMeasurementSequence.cc
 *
 *  Created on: Nov 25, 2011
 *      Author: jchen
 */

#include "patMeasurementSequence.h"

patMeasurementSequence::patMeasurementSequence() {
}
vector<patMeasurement*>* patMeasurementSequence::getPointer(){
	 return &m_measurement_vector;
}
patMeasurementSequence::~patMeasurementSequence() {

	for(vector<patMeasurement*>::iterator iter= m_measurement_vector.begin();
			iter!=m_measurement_vector.end();
			++iter){
		delete *iter;
		*iter =NULL;
	}
}

int patMeasurementSequence::size() const{
	return m_measurement_vector.size();
}
