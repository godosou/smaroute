/*
 * patSampleChoiceSetForObservations.h
 *
 *  Created on: Jul 11, 2012
 *      Author: jchen
 */

#ifndef PATSAMPLECHOICESETFOROBSERVATIONS_H_
#define PATSAMPLECHOICESETFOROBSERVATIONS_H_
#include <vector>
#include "patObservation.h"

class patSampleChoiceSetForObservations {
public:
	patSampleChoiceSetForObservations();
	virtual ~patSampleChoiceSetForObservations();
	void sampleChoiceSet(
			const vector<patObservation>& observations,
			patPathGenerator* path_generator, string folder);
	void addObservation(const patObservation& obs);
protected:

	std::vector<patObservation> m_observations;
};

#endif /* PATSAMPLECHOICESETFOROBSERVATIONS_H_ */
