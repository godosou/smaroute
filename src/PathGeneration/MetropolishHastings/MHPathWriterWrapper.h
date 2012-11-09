/*
 * MHPathWriterWrapper.h
 *
 *  Created on: May 31, 2012
 *      Author: jchen
 */

#ifndef MHPATHWRITERWRAPPER_H_
#define MHPATHWRITERWRAPPER_H_

#include "patChoiceSetWriter.h"
#include "MHStateProcessor.h"
#include "MHPath.h"
#include "patLinkAndPathCost.h"
class MHPathWriterWrapper: public patChoiceSetWriter, public MHStateProcessor<
		MHPath> {
public:
	MHPathWriterWrapper(patPathWriter* pathWriter, const int sampleInterval, const patLinkAndPathCost* link_cost) :
			patChoiceSetWriter::patChoiceSetWriter(pathWriter, sampleInterval ,link_cost) {

	}
	;
	void end(){
		patChoiceSetWriter::end();
	};

	void processState(const MHPath& path, const double log_weight){
		patChoiceSetWriter::processState(path,log_weight);
	}
	;
	void start(){
		patChoiceSetWriter::start();
	};

	virtual ~MHPathWriterWrapper();
};

#endif /* MHPATHWRITERWRAPPER_H_ */
