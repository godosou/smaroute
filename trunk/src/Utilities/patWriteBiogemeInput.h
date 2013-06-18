#ifndef patWriteBiogemeInput_h
#define patWriteBiogemeInput_h

#include "patError.h"
#include "patSample.h"
#include <fstream>
#include "patType.h"
#include "patObservation.h"

class patWriteBiogemeInput{
 public:
	patWriteBiogemeInput( patSample* sample,patNetwork* network,patError*& err);
	
	void computeMaxPathNumber(patError*& err);
	
	void computeColumNumber();
	
	void writeSampleHeader(const patString& fileName);
	
	void writeSampleFile(const patString& fileName);
	void writeSpecFile(const patString& fileName);
	void writeObservation(patObservation* theObservation);
	void writeData(const patString& fileName);
	protected:
	unsigned long nbrOfAlternatives;
	 patSample* theSample;
	patNetwork* theNetwork;
		unsigned long maxPathNumber;
};

#endif