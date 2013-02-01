/*
 * patAnalyzeChoiceSet.h
 *
 *  Created on: Sep 28, 2012
 *      Author: jchen
 */

#ifndef PATANALYZECHOICESET_H_
#define PATANALYZECHOICESET_H_

#include "patObservation.h"
#include <vector>
class patAnalyzeChoiceSet {
public:
	patAnalyzeChoiceSet(std::vector<patObservation>& m_observations);
	virtual ~patAnalyzeChoiceSet();
protected:
	const std::vector<patObservation>& m_observations;

};

#endif /* PATANALYZECHOICESET_H_ */
