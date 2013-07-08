/*
 * patMultiModalPathProba.h
 *
 *  Created on: Aug 5, 2011
 *      Author: jchen
 */

#ifndef PATMULTIMODALPATHPROBA_H_
#define PATMULTIMODALPATHPROBA_H_
#include <vector>
#include <map>
#include "patGpsPoint.h"
#include "patMeasurement.h"
#include "patType.h"
#include "patTransportMode.h"
#include<tr1/unordered_map>
using namespace std::tr1;
class patMultiModalPathProba{
public:
	patMultiModalPathProba();
	patMultiModalPathProba(const patMultiModalPath* path,
			vector<patMeasurement*>* current_gps_sequence,
			set<patArcTransition>* arc_tran_set);
	/**
	 * Calculate the proba for the first point.
	 * @return a real value.
	 */
	double firstPointProba(string algo_type);

	double getLengthWithConstrainedByDDR() const;
	/**
	 * Run the algorithm and return a proba;
	 */
	double run(string algo_type);

	/**
	 * Calculate the probability for a point: \f$Pr(y_k|y_{1:k-1},p)\f$
	 * @param k the \f$k^{th}\f$ GPS point.
	 */
	double calPointProba(string algo_type,int k);

	void initArc();
	double run_ts(string algo_type);
	/**
	 *
	 * Decompose point proba to arcs.
	 * @param k the \f$k^{th}\f$ GPS point
	 * @param arcDDR the arc with DDr
	 */

	set<pair<const patArc*, TransportMode> > getArcsInDDR(
			const patMeasurementDDR* ddr);
	double getPointSimpleDDR(int g) const;

	double calPointArcProba(string algo_type, int j,
			const patArc*, TransportMode mode);
	int getPreviousGPSIndex( int j) const;
protected:
    vector<double> m_proba;
    vector<patMeasurement*>* m_measurement_sequence;
    const patMultiModalPath* m_path;
    double m_init_arc_proba;
    set<patArcTransition>* m_arc_tran_set;
    list<pair<const patArc*, TransportMode> > m_arcs;
    map<TransportMode, double> m_proba_cache;


    const patMultiModalPath* m_up_stream;
    const patMultiModalPath* m_down_stream;
    double m_upstream_proba;
};

#endif /* PATMULTIMODALPATHPROBA_H_ */
