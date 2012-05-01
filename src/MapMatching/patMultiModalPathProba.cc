#include "patConst.h"
#include "patMultiModalPath.h"
#include "patArc.h"
#include "patGpsPoint.h"
#include "patMeasurement.h"
#include "patDisplay.h"
#include "patError.h"
#include "patErrMiscError.h"
#include "patArcTransition.h"
#include "patNBParameters.h"
#include "patPower.h"
#include "patMultiModalPathProba.h"
#include "patBTMeasurementModel.h"
#include "patACCELMeasurementModel.h"
#include "patMeasurementDDR.h"

/*
 *
 */

patMultiModalPathProba::patMultiModalPathProba(const patMultiModalPath* path,
		vector<patMeasurement*>* current_gps_sequence,
		set<patArcTransition>* arc_tran_set) :
		m_path(path), m_measurement_sequence(current_gps_sequence), m_proba(
				current_gps_sequence->size(), 0.0), m_arc_tran_set(arc_tran_set) {
	m_arcs = m_path->getArcsWithMode();
	//DEBUG_MESSAGE("Initiate path probability calculation");
}

/**
 *
 * Length until the last arc in ddr.
 */
double patMultiModalPathProba::getLengthWithConstrainedByDDR() const {
	list<const patArc*> arc_list = m_path->getArcList();
	list<TransportMode> mode_list = m_path->getModeList();
	list<const patArc*>::const_iterator arc_iter = arc_list.end();
	list<TransportMode>::const_iterator mode_iter = mode_list.end();
	double effective_length = m_path->getLength();
	while (arc_iter != arc_list.begin()) {
		--arc_iter;
		--mode_iter;
		if ((*mode_iter != TransportMode(BUS)
				&& *mode_iter != TransportMode(TRAIN)
				&& *mode_iter != TransportMode(METRO))
				|| m_measurement_sequence->back()->getDDR()->isArcInDomain(
						*arc_iter, *mode_iter)) {
			break;
		} else {
			effective_length -= (*arc_iter)->getLength();
		}
	}
	list<TransportMode>::const_iterator mode_iter_begin = mode_list.begin();

	for (list<const patArc*>::const_iterator arc_iter_begin = arc_list.begin();
			arc_iter_begin != arc_iter; ++arc_iter_begin) {

		if ((*mode_iter_begin != TransportMode(BUS)
				&& *mode_iter_begin != TransportMode(TRAIN)
				&& *mode_iter_begin != TransportMode(METRO))
				|| m_measurement_sequence->front()->getDDR()->isArcInDomain(
						*arc_iter_begin, *mode_iter_begin)) {
			break;
		} else {
			effective_length -= (*arc_iter_begin)->getLength();
		}
		++mode_iter_begin;
	}
	return effective_length;
}

double patMultiModalPathProba::run(string algo_type) {

	return run_ts(algo_type);

}

double patMultiModalPathProba::firstPointProba(string algo_type) {
	m_proba[0] = m_measurement_sequence->at(0)->getDDR()->computePathDDRRaw(
			*m_path);
	/*
	 if (m_proba[0] / m_path->getLength() > 1.0) {
	 //proba greater than zero, show warning message.
	 WARNING(
	 m_proba[0] / m_path->getLength() << "=" << m_proba[0] << "/"
	 << m_path->getLength());
	 }
	 */
	/*
	double effective_length = getLengthWithConstrainedByDDR();
	if (effective_length < 0.0) {
		WARNING(
				"wrong effective length " << m_path->getLength() << " "
						<< effective_length);
	}

	if (effective_length < m_path->getLength()) {
		WARNING(
				"different effective length "
						<< getTransportMode(m_path->front_road_travel().mode)
						<< getTransportMode(m_path->back_road_travel().mode)
						<< ":" << m_path->getLength() << " "
						<< effective_length);
	}
*/
	m_proba[0] = m_proba[0] / m_path->getLength();
//	DEBUG_MESSAGE(m_proba[0]<<" "<< effective_length);
	return m_proba[0];

}
int integral_count = 0;
int found_count = 0;
double patMultiModalPathProba::run_ts(string algo_type) {
	/*to do*/
	//double probaPath = 1.0 * gpsSequence->front()->getddr_arcsValue(m_path->front(),baseNetwork)/gpsSequence->at(0)->getDDRSum();
	//probaPath = 1.0;
//	DEBUG_MESSAGE("---Calculate a path's likelihood");
	integral_count = 0;
	found_count = 0;

	m_proba[0] = firstPointProba(algo_type);
	if (m_proba[0] == 0.0) {
		return -DBL_MAX;
	}

//	DEBUG_MESSAGE("point "<<0<<*(m_measurement_sequence->at(0)));

	//DEBUG_MESSAGE(m_mm_iteartion->getCurentGpsSequence()->size());
	for (unsigned long j = 1; j < m_measurement_sequence->size(); ++j) {
		//DEBUG_MESSAGE("point "<<j<<": "<< m_measurement_sequence->at(j)->getMeasurementType());
		calPointProba(algo_type, j);
		if (m_proba[j] == 0.0) {

			DEBUG_MESSAGE(
					"---algo:" << algo_type << "zero probability at ["
							<< (j + 1) << "]"
							<< m_measurement_sequence->at(j)->getTimeStamp()
							<< " total measurements:" << m_measurement_sequence->size());

			DEBUG_MESSAGE("---Finish a path's likelihood");
			return -DBL_MAX;
		}
	}
	double log_proba = 0.0;

	for (int j = 0; j < m_proba.size(); j++) {
		if (m_proba[j] == 0.0) {
			return -DBL_MAX;
		} else {
			log_proba += log(m_proba[j]);
		}
	}
	//DEBUG_MESSAGE(
//			"	Calculated Transitions:" << integral_count << "+" << found_count);
//	DEBUG_MESSAGE("path proba" << path_proba);
//	DEBUG_MESSAGE("---Finish a path's likelihood");

	return log_proba;
}

double patMultiModalPathProba::calPointProba(string algo_type, int j) {
	m_proba[j] = 0.0;

	int previous_gps_index = getPreviousGPSIndex(j);
	if (previous_gps_index < 0) {
		WARNING("wrong gps index");
		return 0.0;
	}

	const list<double>* distance_to_stop = m_path->getDistanceToStop();
	 list<double>::const_iterator stop_iter = distance_to_stop->begin();
	for (list<pair<const patArc*, TransportMode> >::const_iterator curr_arc_iter =
			m_arcs.begin(); curr_arc_iter != m_arcs.end(); ++curr_arc_iter) {
		if (m_measurement_sequence->at(j)->getDDR()->isArcInDomain(
				curr_arc_iter->first, curr_arc_iter->second)) {
			list<pair<const patArc*, TransportMode> >::const_iterator prev_arc_iter =
					curr_arc_iter;
			list<pair<const patArc*, TransportMode> > inter_arcs;
			while (1) {
				inter_arcs.push_front(*prev_arc_iter);
				if (m_measurement_sequence->at(previous_gps_index)->getDDR()->isArcInDomain(
						prev_arc_iter->first, prev_arc_iter->second)) {

					bool right_connection = true;
					patMultiModalPath inter_path(inter_arcs, right_connection);
					if (right_connection != true) {
						WARNING("Wrong connection");
						return 0.0;
					}
					inter_path.setDistanceToStop(*stop_iter);
					patArcTransition theTran(inter_path,
							m_measurement_sequence->at(previous_gps_index),
							m_measurement_sequence->at(j));
					double tranValue = 0.0;
					set<patArcTransition>::iterator found =
							m_arc_tran_set->find(theTran);

					if (found != m_arc_tran_set->end()) {
						try {
							tranValue =
									const_cast<patArcTransition*>(&(*found))->getValue(
											algo_type);
						} catch (...) {
							DEBUG_MESSAGE("ERROR in calculating arc transition");
							tranValue = 0.0;
						}
						//tranValue = found->second;
						++found_count;
					} else {
						++integral_count;
						try {
							tranValue = theTran.getValue(algo_type);
						} catch (...) {

							DEBUG_MESSAGE("ERROR in calculating arc transition");
							tranValue = 0.0;
						}
						m_arc_tran_set->insert(theTran);
						//(*m_arc_tran_set)[theTran]=tranValue;

					}

					m_proba_cache[curr_arc_iter->second] += tranValue;
					//			DEBUG_MESSAGE(tranValue);
					m_proba[j] += tranValue;

				}
				if (prev_arc_iter == m_arcs.begin()) {
					break;
				} else {
					--prev_arc_iter;
				}
			}
		}
		++stop_iter;
	}
//	DEBUG_MESSAGE(j<<" "<<m_proba[j] << "," << getPreviousGPSIndex(j)<<" "<<getPointSimpleDDR(getPreviousGPSIndex(j)));
	double normalization =
			m_measurement_sequence->at(previous_gps_index)->getDDR()->computePathDDRRaw(
					*m_path);
//double normalization = getPointSimpleDDR(getPreviousGPSIndex(j));
//DEBUG_MESSAGE(	m_proba[j]<< ";"<< normalization<<", "<<getPointSimpleDDR(getPreviousGPSIndex(j)));
	m_proba[j] /= normalization;
	for (map<TransportMode, double>::iterator iter = m_proba_cache.begin();
			iter != m_proba_cache.end(); ++iter) {
		m_proba_cache[iter->first] /= normalization;

	}

//	m_proba[j] /=getPointSimpleDDR(getPreviousGPSIndex(j));
	//DEBUG_MESSAGE(" proba: " << j << ": " << m_proba[j]);

	return m_proba[j];
}

double patMultiModalPathProba::getPointSimpleDDR(int g) const {
	double ddr_sum = 0.0;
	list<const patArc*> arc_list = m_path->getArcList();
	list<TransportMode> mode_list = m_path->getModeList();
	list<const patArc*>::const_iterator arc_iter = arc_list.end();
	list<TransportMode>::const_iterator mode_iter = mode_list.end();
	while (arc_iter != arc_list.begin()) {
		--arc_iter;
		--mode_iter;
		ddr_sum += (*arc_iter)->getLength()
				* m_measurement_sequence->at(g)->getDDR()->getArcDDRValue(
						*arc_iter, *mode_iter);
	}
//DEBUG_MESSAGE(ddr_sum)
	return ddr_sum;
}
int patMultiModalPathProba::getPreviousGPSIndex(int j) const {
	for (int k = j - 1; k >= 0; --k) {

		if (m_measurement_sequence->at(k)->isGPS()) {
			return k;
		}
	}
	return -1;
}
