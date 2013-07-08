/*
 * patSampleChoiceSetWithRandomWalk.h
 *
 *  Created on: Jun 26, 2012
 *      Author: jchen
 */

#ifndef PATSAMPLECHOICESETWITHRANDOMWALK_H_
#define PATSAMPLECHOICESETWITHRANDOMWALK_H_

#include "RWPathGenerator.h"
#include "patObservation.h"
#include <vector>
using namespace std;
class patSampleChoiceSetWithRandomWalk {
public:
	patSampleChoiceSetWithRandomWalk();

	void addObservation(patObservation& obs);
	void sampleChoiceSet(RWPathGenerator* path_generator, string folder);

	virtual ~patSampleChoiceSetWithRandomWalk();
protected:
	vector<patObservation> m_observations;
};

#endif /* PATSAMPLECHOICESETWITHRANDOMWALK_H_ */
