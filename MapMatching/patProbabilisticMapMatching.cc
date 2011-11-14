/*
 * patProbabilisticMapMatching.cc
 *
 *  Created on: Nov 10, 2011
 *      Author: jchen
 */

#include "patProbabilisticMapMatching.h"
#include "patReadGpsFromCsv.h"

patProbabilisticMapMatching::patProbabilisticMapMatching(string gps_file_name,patError*& err):
m_gps_sequence(gps_file_name,err){

	m_gps_sequence.recalculateSpeedHeading();
	m_gps_sequence.setSpeedType();
}

patProbabilisticMapMatching::~patProbabilisticMapMatching() {
}


int patProbabilisticMapMatching::findFirstValidGps() {

	return i;
}

void patProbabilisticMapMatching::run(){
	/*
	 * Algorithm:
	 * Loop over all GPS point, ignore empty GPS point.
	 */

	vector<patGpsPoint*> valid_gps_history;
	DEBUG_MESSAGE("Prepare to generate DDR. ");
	DEBUG_MESSAGE("Raw points:" << m_gps_sequence.size());
	DEBUG_MESSAGE("first point. " << m_gps_sequence[0]->getTimeStamp());
	patGpsPoint* prevGpsPoint;
	patGpsPoint* prevNormalGpsPoint;

	int first_valid = 0;
	for (; first_valid < m_gps_sequence.size(); ++i) {
		int next = i + 1;
		if (first_valid == (m_gps_sequence.size() - 1)) {
			next = first_valid;
		}

		patMapMatchingIteration init_iteration(&valid_gps_history, m_gps_sequence[i],
				&m_arc_tran_st);
		init_iteration.firstIteration(m_gps_sequence[i]);

		if (m_gps_sequence[first_valid]->getDDR()->empty()) {
			DEBUG_MESSAGE("No domain at:" << m_gps_sequence[first_valid]->getTimeStamp());
		} else {
			break;
		}
	}

	DEBUG_MESSAGE(
			"First valid point:" << first_valid << ","
					<< m_gps_sequence[first_valid]->getTimeStamp());
	if (first_valid == m_gps_sequence.size()) {
		return;
	}
	prevGpsPoint = m_gps_sequence[first_valid];
	prevNormalGpsPoint = m_gps_sequence[first_valid];

	vector<patGpsPoint*> lowSpeedGpsPoints;
	for (int i = first_valid + 1; i < m_gps_sequence.size(); ++i) {
		DEBUG_MESSAGE(i + 1 << "," << m_gps_sequence[i]);
		if (m_gps_sequence[i]->getType() != patString("normal_speed")
				&& i != m_gps_sequence.size() - 1) {

			lowSpeedGpsPoints.push_back(m_gps_sequence[i]);
		} else {
			patMapMatchingIteration init_iteration(&gps_iterating, m_gps_sequence[i],
							&m_arc_tran_st);

			m_gps_sequence[i]->genSegmentDDRV3(prevNormalGpsPoint, theNetwork,
					adjList, pathDevelop, &m_gps_sequence, &lowSpeedGpsPoints);
			DEBUG_MESSAGE(
					"Link DDR number:" << m_gps_sequence[i]->getLinkDDR()->size());
			if (&(m_gps_sequence[i]) == pathDevelop->lastGpsPoint()) {
				DEBUG_MESSAGE(
						"Found Domain at (" << i + 1 << ") "
								<< m_gps_sequence[i]->getTimeStamp());
				lowSpeedGpsPoints = vector<patGpsPoint*>();
				prevNormalGpsPoint = &(m_gps_sequence[i]);
			} else {
				DEBUG_MESSAGE("No domain at:" << m_gps_sequence[i]->getTimeStamp());
			}

		}
	}
	if (!lowSpeedGpsPoints.empty()) {
		pathDevelop->lowSpeedPoints(&lowSpeedGpsPoints,
				&(prevNormalGpsPoint->getDomainSet()), theNetwork);
	}

}

