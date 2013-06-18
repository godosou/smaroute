/*
 * patMultiModalPathMatching.cc
 *
 *  Created on: Apr 3, 2012
 *      Author: jchen
 */

#include "patMultiModalPathMatching.h"
#include "patConst.h"
#include "patArc.h"
#include "patGpsPoint.h"
#include "patMeasurement.h"
#include "patDisplay.h"
#include "patError.h"
#include "patErrMiscError.h"
#include "patArcTransition.h"
#include "patNBParameters.h"
#include "patPower.h"
#include "patBTMeasurementModel.h"
#include "patAccelMeasurementModel.h"
#include "patMeasurementDDR.h"

patMultiModalPathMatching::patMultiModalPathMatching() {
	// TODO Auto-generated constructor stub

}

patMultiModalPathMatching::~patMultiModalPathMatching() {
	// TODO Auto-generated destructor stub
}

/**
 * Add a value to the norminator in a given position.
 * If position doens't exist, return false;
 */

/*
 * update the norminator and the denominator
 * nominator, add raw arc ddr
 * denominator, add the downstream length.
 */

bool patMultiModalPathMatching::updateProba(double norminator,
		double denorminator, short position) {

	if (m_norminators.size() != m_denorminators.size()) {
		WARNING("norminator and denominator have different size");
		return false;
	}
	if (m_norminators.size() == position) {
		//DEBUG_MESSAGE("new measurement");
		m_norminators.push_back(0.0);
		m_denorminators.push_back(0.0);
	}
	else if (m_norminators.size() < position) {
		WARNING("WRONG"<<m_norminators.size() <<","<<position);
		return false;
	}
	m_norminators[position] += norminator;
	m_denorminators[position] += denorminator;
	return true;
}

bool patMultiModalPathMatching::calculateLogLikelihood() {

	if (m_norminators.size() != m_denorminators.size() ) {
		WARNING("norminator and denominator have different size");
		return false;
	}
	if(m_measurement_sequence.size()!=m_norminators.size()){
		WARNING("wrong proba size"<<m_norminators.size()<<","<<m_measurement_sequence.size());
	}
	if( m_denorminators[0] - getLength() > 0.00000000001){
		WARNING("first denominator wrong"<<getLength()<<"!="<<m_denorminators[0]);
	}
	m_loglikelihood = 0.0;
	int i=0;
	for (vector<double>::const_iterator iter = m_norminators.begin();
			iter != m_norminators.end(); ++iter) {
		if (*iter == 0.0) {
			m_loglikelihood = -DBL_MAX;
			DEBUG_MESSAGE("zero proba at "<<i);
		}
		++i;
		m_loglikelihood += log(*iter);
	}

	for (vector<double>::const_iterator iter = m_denorminators.begin();
			iter != m_denorminators.end(); ++iter) {
		if (*iter == 0.0) {

			m_loglikelihood = -DBL_MAX;
			WARNING("denominator zero");
			return false;
		}
		m_loglikelihood -= log(*iter);
	}
	return true;
}

int patMultiModalPathMatching::getPreviousGPSIndex(int j) const {
	for (int k = j - 1; k >= 0; --k) {

		if (m_measurement_sequence[k]->isGPS()) {
			return k;
		}
	}
	return -1;
}

int patMultiModalPathMatching::getNextGPSIndex(int j) const {
	for (int k = j+1 ; k < m_measurement_sequence.size(); ++k) {

		if (m_measurement_sequence[k]->isGPS()) {
			return k;
		}
	}
	return -1;
}
bool patMultiModalPathMatching::updateFirstPoint(pair<const patArc*, TransportMode>* first_arc_prev_ddr) {
	double n = m_measurement_sequence[0]->getDDR()->computePathDDRRaw(
			m_downstream);
	double dn = m_downstream.getLength();
	first_arc_prev_ddr->first = front();
	first_arc_prev_ddr->second = frontMode();
	return updateProba(n, dn, 0);
}

int integral_count = 0;
int found_count = 0;
/**
 * Update a point with measurement index,
 * denominator gets the raw ddr of the previous point.
 * @param new_measurement: indicates whether it is new measurement or not.
 */
bool patMultiModalPathMatching::updatePoint(int measurement_index,
		bool new_measurement, set<patArcTransition>* arc_tran_set,
		pair<const patArc*, TransportMode>* first_arc_prev_ddr) {
	/**
	 * Update m_norminator and m_denorminator for new point on entire path
	 */
	if (measurement_index == 0) {
		return updateFirstPoint(first_arc_prev_ddr);
	}

	//denominator.

	double denorminator = 0.0;

	double norminator = 0.0;
	int step_limit = 0;

	int previous_gps_index = measurement_index -1;
	if(patNBParameters::the()->usePreviousGPS==1){
		previous_gps_index = getPreviousGPSIndex(measurement_index);
	}
	//

	int next_gps_index = getNextGPSIndex(measurement_index);
	if(next_gps_index<0){
		//WARNING("do not find next GPS");
	}
	if (m_measurement_sequence[measurement_index]->getTimeStamp() == m_measurement_sequence[previous_gps_index]->getTimeStamp() ){
		//WARNING("SAME TIME STAMP")
		--previous_gps_index;
	}

	if (previous_gps_index < 0) {
		WARNING("wrong gps index");
		return false;
	}

	list<pair<const patArc*, TransportMode> > arcs_with_modes =
			getArcsWithMode();
	if (new_measurement == true) {
		step_limit = arcs_with_modes.size(); //all arcs are considered
		denorminator =
				m_measurement_sequence[previous_gps_index]->getDDR()->computePathDDRRaw(
						*this);
	} else {
		step_limit = m_downstream.size(); //only the down stream is considered.

		denorminator =
				m_measurement_sequence[previous_gps_index]->getDDR()->computePathDDRRaw(
						m_downstream);
	}


	list<double>::const_iterator stop_iter = m_distance_to_stop.begin();
	list<pair<const patArc*, TransportMode> >::const_iterator curr_arc_iter =
			arcs_with_modes.end();

	int curr_gap_in_ddr = -10000000;

	bool found_next_gps_ddr = false;
	list<pair<const patArc*, TransportMode> >::const_iterator first_arc_prev_ddr_tmp = arcs_with_modes.begin();
	for(list<pair<const patArc*, TransportMode> >::const_iterator iter_1 = arcs_with_modes.begin();iter_1!=arcs_with_modes.end();++iter_1){
		if(m_measurement_sequence[previous_gps_index]->getDDR()->isArcInDomain(iter_1->first,iter_1->second)){
			first_arc_prev_ddr_tmp = iter_1;
			break;
		}
	}

	for (int i = 0; i < step_limit; ++i) {
		--curr_arc_iter;
		if (curr_gap_in_ddr >= 5) {
			break;
		}
		if (found_next_gps_ddr==false && !m_measurement_sequence[measurement_index]->isGPS()&&
				!m_measurement_sequence[next_gps_index]->getDDR()->isArcInDomain(
				curr_arc_iter->first, curr_arc_iter->second)
			){
			//DEBUG_MESSAGE("SKIP");
			continue;
		}
		else {
			found_next_gps_ddr = true;
			if (m_measurement_sequence[measurement_index]->getDDR()->isArcInDomain(curr_arc_iter->first, curr_arc_iter->second)) {
				if(m_measurement_sequence[measurement_index]->isGPS()){
					first_arc_prev_ddr->first = curr_arc_iter->first;
					first_arc_prev_ddr->second  =  curr_arc_iter->second;
				}
				curr_gap_in_ddr = 0;
				list<pair<const patArc*, TransportMode> >::const_iterator prev_arc_iter =
						curr_arc_iter;
				list<pair<const patArc*, TransportMode> > inter_arcs;
				int gap_in_ddr = -10000000;
				while (1) {
					inter_arcs.push_front(*prev_arc_iter);
					if (gap_in_ddr >= 5	) {
						break;
					}
					if (m_measurement_sequence[previous_gps_index]->getDDR()->isArcInDomain(prev_arc_iter->first, prev_arc_iter->second)) {
						gap_in_ddr = 0;
						bool right_connection = true;
						patMultiModalPath inter_path(inter_arcs, right_connection);
						if (right_connection != true) {
							WARNING("Wrong connection");
							return false;
						}
						inter_path.setDistanceToStop(0.0);
						patArcTransition theTran(inter_path,
								m_measurement_sequence[previous_gps_index],
								m_measurement_sequence[measurement_index]);
						double tranValue = 0.0;

						set<patArcTransition>::iterator found = arc_tran_set->find(
								theTran);

						if (found != arc_tran_set->end()) {
							try {
								tranValue =
										const_cast<patArcTransition*>(&(*found))->getValue(
												patNBParameters::the()->algoInSelection,m_measurement_sequence[previous_gps_index],m_measurement_sequence[measurement_index]);
							} catch (...) {
								DEBUG_MESSAGE(
										"ERROR in calculating arc transition");
								tranValue = 0.0;
							}
							//tranValue = found->second;
							++found_count;
						} else {
							++integral_count;
							try {
								tranValue = theTran.getValue(
										patNBParameters::the()->algoInSelection,m_measurement_sequence[previous_gps_index],m_measurement_sequence[measurement_index]);
								arc_tran_set->insert(theTran);
							} catch (...) {

								DEBUG_MESSAGE(
										"ERROR in calculating arc transition");
								tranValue = 0.0;
							}
							//(*m_arc_tran_set)[theTran]=tranValue;

						}

						norminator += tranValue;

					} else {
						++gap_in_ddr;
					}
					if (prev_arc_iter == arcs_with_modes.begin()) {
						break;
					}
					else if(!m_measurement_sequence[measurement_index]->isGPS() && prev_arc_iter==first_arc_prev_ddr_tmp){
						break;
					}
					else {
						--prev_arc_iter;
					}
				}
			}
			else {
				++curr_gap_in_ddr;
			}
		}

	}
	if(norminator==0.0 && new_measurement){
		WARNING(m_measurement_sequence[measurement_index]->isGPS()<<","<<curr_gap_in_ddr);
	}
	//DEBUG_MESSAGE("FOUND COUNT"<<found_count<<", integral_count"<<integral_count);
	return updateProba(norminator, denorminator, measurement_index);
}

/**
 * Add down stream
 */
bool patMultiModalPathMatching::addDownStream(patMultiModalPath down_stream) {
	if(m_downstream.size()!=0){
		WARNING("original downstream not yet treated: "<<m_downstream.size());
		if (m_downstream.append(down_stream) == false) {
			WARNING("fail to update downstream segment ");
			return false;
		}
	}
	else{
		m_downstream = down_stream;

		if (append(m_downstream) == false) {
			WARNING("fail to add downstream segment");
			return false;
		}
	}
	return true;
}

/**
 * Add down stream
 */
void patMultiModalPathMatching::addMeasurement(
		patMeasurement* new_measurements) {
	m_new_measurements.push_back(new_measurements);
}

/**
 * Update the m_norminator, m_denorminator and loglike, m_measurement_sequence and arcs.
 * New entry in m_norminator and m_denorminator is created for new measurements.
 * Clear m_new_measurements and m_downstream at the end.
 */
bool patMultiModalPathMatching::update(set<patArcTransition>* arc_tran_set) {
	short i = 0;
	/*
	if (m_downstream.size()==0){
		DEBUG_MESSAGE("no downstream appended");
	}
	*/
	pair<const patArc*, TransportMode> first_arc_prev_ddr;
	for (vector<patMeasurement*>::const_iterator iter =
			m_measurement_sequence.begin();
			iter != m_measurement_sequence.end(); ++iter) {
		if (i >= 1) {
			if ((*iter)->getTimeStamp()
					< m_measurement_sequence[i - 1]->getTimeStamp()) {

				DEBUG_MESSAGE("wrong time stamp");
			}
		}
		//DEBUG_MESSAGE(i);
		if (updatePoint(i, false, arc_tran_set,&first_arc_prev_ddr) == false) {
			return false;
		}
		++i;
	}

	for (vector<patMeasurement*>::const_iterator iter =
			m_new_measurements.begin(); iter != m_new_measurements.end();
			++iter) {

		if (i >= 1) {
			if ((*iter)->getTimeStamp()
					< m_measurement_sequence[i - 1]->getTimeStamp()) {

				DEBUG_MESSAGE("wrong time stamp");
			return false;
			}
		}

		m_measurement_sequence.push_back(*iter);
	}
	for (vector<patMeasurement*>::const_iterator iter =
			m_new_measurements.begin(); iter != m_new_measurements.end();
			++iter) {
		//DEBUG_MESSAGE(i);
		if(m_measurement_sequence[i ]!=*iter){
			WARNING("wrong data sequence");
		}
		//DEBUG_MESSAGE(i<<m_measurement_sequence[i - 1]->getTimeStamp()<<(*iter)->getTimeStamp);
		if (updatePoint(i, true, arc_tran_set,&first_arc_prev_ddr) == false) {
			WARNING("wrong update "<<i);
			return false;
		}
		++i;
	}
/*
	DEBUG_MESSAGE(
			"	Calculated Transitions:" << integral_count << "+" << found_count);
*/
	calculateLogLikelihood();
	m_new_measurements.clear();
	patMultiModalPath tmp;
	m_downstream = tmp;
	return true;
}
double patMultiModalPathMatching::getLogLikelihood() const {
	return m_loglikelihood;
}
