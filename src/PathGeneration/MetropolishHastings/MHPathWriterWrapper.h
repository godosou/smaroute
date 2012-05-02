/*
 * MHPathWriterWrapper.h
 *
 *  Created on: Apr 27, 2012
 *      Author: jchen
 */

#ifndef MHPATHWRITERWRAPPER_H_
#define MHPATHWRITERWRAPPER_H_

#include "MHPathWriterWrapper.h"
#include "patPathWriter.h"
#include "MHLinkAndPathCost.h"
#include "MHStateProcessor.h"
#include "MHPath.h"
class MHPathWriterWrapper: public MHStateProcessor<MHPath> {
public:
	MHPathWriterWrapper();
	virtual ~MHPathWriterWrapper();

	void end();
	void processState(const MHPath& path);
	void start();
	MHPathWriterWrapper(patPathWriter* pathWriter, const int sampleInterval,
			const MHLinkAndPathCost* pathCost);

protected:
	patPathWriter* m_path_writer;
	int m_sample_interval;
	const MHLinkAndPathCost* m_path_cost;
	int m_path_count;
};

#endif /* MHPATHWRITERWRAPPER_H_ */
