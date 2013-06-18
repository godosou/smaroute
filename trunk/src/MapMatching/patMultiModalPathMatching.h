/*
 * patMultiModalPathMatching.h
 *
 *  Created on: Apr 3, 2012
 *      Author: jchen
 */

#ifndef PATMULTIMODALPATHMATCHING_H_
#define PATMULTIMODALPATHMATCHING_H_

#include <patMultiModalPath.h>

#include <vector>
#include <map>
#include "patGpsPoint.h"
#include "patMeasurement.h"
#include "patType.h"
#include "patTransportMode.h"
#include "patArcTransition.h"
class patMultiModalPathMatching: public patMultiModalPath {
public:
	patMultiModalPathMatching();
	virtual ~patMultiModalPathMatching();

	/**
	 * Add a value to the norminator in a given position.
	 * If position doens't exist, return false;
	 */

	/*
	 * update the norminator and the denominator
	 * nominator, add raw arc ddr
	 * denominator, add the downstream length.
	 */

	bool updateProba(double norminator, double denorminator, short position);

	bool calculateLogLikelihood();

	int getPreviousGPSIndex(int j) const;
	int getNextGPSIndex(int j) const;
	bool updateFirstPoint(pair<const patArc*, TransportMode>* first_arc_prev_ddr);
	bool updatePoint(int measurement_index, bool new_measurement,set<patArcTransition>* arc_tran_set,pair<const patArc*, TransportMode>* first_arc_prev_ddr);
	/**
	 * Add down stream
	 */
	bool addDownStream(patMultiModalPath down_stream);

	/**
	 * Add down stream
	 */
	void addMeasurement( patMeasurement* new_measurements);

	/**
	 * Update the m_norminator, m_denorminator and loglike, m_measurement_sequence and arcs.
	 * New entry in m_norminator and m_denorminator is created for new measurements.
	 * Clear m_new_measurements and m_down_stream at the end.
	 */
	bool update(set<patArcTransition>* arc_tran_set);
	double getLogLikelihood() const;
protected:
	vector<double> m_norminators;
	vector<double> m_denorminators;

	double m_loglikelihood;
	vector< patMeasurement*> m_measurement_sequence;

	vector< patMeasurement*> m_new_measurements;
	int m_downstream_length;
	patMultiModalPath m_downstream;

};

#endif /* patMultiModalPathMatching_H_ */
