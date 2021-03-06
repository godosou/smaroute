/*
 * patObservationWritter
 *
 *  Created on: Jul 5, 2012
 *      Author: jchen
 */

#ifndef PATOBSERVATIONWRITTER_H_
#define PATOBSERVATIONWRITTER_H_
#include <string>
class patMultiModalPath;
#include "patLinkAndPathCost.h"
class patObservationWritter {
public:
	patObservationWritter();
	patObservationWritter(std::string folder,
			const unsigned long sampleInterval,
			const patLinkAndPathCost* cost_function,
			const unsigned long& writer_indexer=0);
	virtual ~patObservationWritter();
	virtual void end();
	virtual void processState(const patMultiModalPath& path,
			const double log_weight=0.0, const double proba=1.);
	virtual void start();

protected:
	std::string m_folder;
	const patLinkAndPathCost* m_cost_function;
	unsigned long m_writer_indexer;
	unsigned long m_sampled_path_count;
	unsigned long m_warmup_iterations;
	unsigned long m_sample_interval;
	unsigned long m_path_count;
};

#endif /* PATOBSERVATIONWRITER_H_ */
