/*
 * patObservationWriter.cc
 *
 *  Created on: Jul 5, 2012
 *      Author: jchen
 */

#include "patObservationWritter.h"
#include "patMultiModalPath.h"
#include "patNBParameters.h"
#include "patException.h"
#include <boost/lexical_cast.hpp>
#include "patKMLPathWriter.h"
using namespace std;
patObservationWritter::patObservationWritter() {
	// TODO Auto-generated constructor stub

}

patObservationWritter::~patObservationWritter() {
	// TODO Auto-generated destructor stub
}

patObservationWritter::patObservationWritter(string folder,
		const  unsigned long  sampleInterval) :
		m_sample_interval(sampleInterval), m_folder(folder), m_path_count(
				0) ,m_sampled_path_count(0){

	m_warmup_iterations = patNBParameters::the()->WARMUP_ITERATIONS;
	if (sampleInterval <= 0) {
		throw IllegalArgumentException(
				"sample interval is not strictly positive");
	}
	DEBUG_MESSAGE(m_warmup_iterations);

}

void patObservationWritter::start() {
	m_path_count = 0;
	m_sampled_path_count=0;
}

void patObservationWritter::processState(const patMultiModalPath& path,
		const double log_weight) {
	/*
	 * (1) check if this path should be written
	 */
	m_path_count++;
	int sampled_count = m_path_count-m_warmup_iterations;
	if (sampled_count<=0 || sampled_count% m_sample_interval != 0) {

		return;
	}

	/*
	 * (2) write out the path
	 */

//	cout << m_path_count<<"-"<<m_warmup_iterations<<endl;

//	cout <<"export a path"<<endl;
	m_sampled_path_count++;
	string i_str = boost::lexical_cast<string>(m_sampled_path_count);
	patKMLPathWriter kml_writer(m_folder + "observations/" + i_str + ".kml");
//	cout <<m_folder + "observations/" + i_str + ".kml"<<endl;
	map<string, string> attrs_true;
	attrs_true["true"] = boost::lexical_cast<string>(log_weight);
	attrs_true["id"] = i_str;
	attrs_true["proba"] = boost::lexical_cast<string>(
			1.0 - patNBParameters::the()->errorInSimulatedObservations);
	kml_writer.writePath(path, attrs_true);
	kml_writer.close();
}

void patObservationWritter::end() {
	if(m_sampled_path_count!=patNBParameters::the()->SAMPLE_COUNT){
		WARNING("sampled paths do not match: expected "<<patNBParameters::the()->SAMPLE_COUNT<<", return "<<m_sampled_path_count);
	}
}
