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
class patObservationWritter {
public:
	patObservationWritter();
	patObservationWritter(std::string folder, const unsigned long sampleInterval);
	virtual ~patObservationWritter();
	virtual void end();
	virtual void processState(const patMultiModalPath& path, const double log_weight);
	virtual void start();

protected:
	std::string m_folder;

	unsigned long m_sampled_path_count;
	unsigned long m_warmup_iterations;
	unsigned long m_sample_interval;
	unsigned long m_path_count;
};

#endif /* PATOBSERVATIONWRITER_H_ */
