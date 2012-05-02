/*
 * MHPathWriterWrapper.cc
 *
 *  Created on: Apr 27, 2012
 *      Author: jchen
 */

#include "patException.h"
#include "MHPathWriterWrapper.h"
#include <boost/lexical_cast.hpp>
using namespace boost;

MHPathWriterWrapper::MHPathWriterWrapper() {
	// TODO Auto-generated destructor stub
}
MHPathWriterWrapper::~MHPathWriterWrapper() {
	// TODO Auto-generated destructor stub
}
MHPathWriterWrapper::MHPathWriterWrapper( patPathWriter* pathWriter,
		const int sampleInterval, const MHLinkAndPathCost* pathCost) :m_sample_interval(sampleInterval),
		m_path_writer(pathWriter),
		m_path_cost(pathCost),
		m_path_count(0){
	if (pathWriter == NULL) {
		throw IllegalArgumentException("path writer is null");
	}
	if (sampleInterval <= 0) {
		throw IllegalArgumentException(
				"sample interval is not strictly positive");
	}
	if (pathCost == NULL) {
		throw new IllegalArgumentException("path cost is null");
	}
}

void MHPathWriterWrapper::start() {
	m_path_count = 0;
}

void MHPathWriterWrapper::processState(const MHPath& path) {
	/*
	 * (1) check if this path should be written
	 */
	m_path_count++;
	if (m_path_count % m_sample_interval != 0) {
		return;
	}
	/*
	 * (2) write out the link IDs that correspond to the inverted nodes
	 */
	map < string, string > attrs;
	attrs["logweight"]=lexical_cast<std::string>(m_path_cost->logWeightWithoutCorrection(path));
	m_path_writer->writePath(path,attrs);
}

void MHPathWriterWrapper::end() {
}

