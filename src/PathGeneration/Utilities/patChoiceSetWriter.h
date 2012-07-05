/*
 * MHPathWriterWrapper.h
 *
 *  Created on: Apr 27, 2012
 *      Author: jchen
 */

#ifndef PATCHOICESETWRITER_H_
#define PATCHOICESETWRITER_H_

#include "patPathWriter.h"

#include "patMultiModalPath.h"
#include <set>
class patChoiceSetWriter {
public:
	patChoiceSetWriter();
	virtual ~patChoiceSetWriter();

	virtual void end();
	virtual void processState(const patMultiModalPath& path, const double log_weight);
	virtual void start();
	patChoiceSetWriter(patPathWriter* pathWriter, const int sampleInterval);

protected:
	patPathWriter* m_path_writer;
	unsigned long m_warmup_iterations;
	int m_sample_interval;
//	const MHLinkAndPathCost* m_path_cost;
	map<patMultiModalPath,pair<int, double> > m_sampled_set;
	int m_path_count;
};

#endif /* PATCHOICESETWRITER_H_ */
