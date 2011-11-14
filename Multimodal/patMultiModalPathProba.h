/*
 * patMultiModalPathProba.h
 *
 *  Created on: Aug 5, 2011
 *      Author: jchen
 */

#ifndef PATMULTIMODALPATHPROBA_H_
#define PATMULTIMODALPATHPROBA_H_
#include "patPathProbaAlgoV4.h"
#include <vector>
#include <map>
#include "patGpsPoint.h"
#include "patNetworks.h"
#include "patType.h"
class patMultiModalPathProba{
public:
	patMultiModalPathProba();
	virtual ~patMultiModalPathProba();
	/**
	 * Calculate the proba for the first point.
	 * @return a real value.
	 */
	patReal firstPointProba();

	/**
	 * Run the algorithm and return a proba;
	 */
	patReal run();

	/**
	 * Calculate the probability for a point: \f$Pr(y_k|y_{1:k-1},p)\f$
	 * @param k the \f$k^{th}\f$ GPS point.
	 */
	patReal calPointProba(short k);

	/**
	 * Get the value for normalizing.
	 * \f[
	 *	\int_{x_{k}\in p} Pr(y_{k}|x_{k},p)
	 * \f]
	 */
	patReal getPointSimpleDDR(short k);

	/**
	 *
	 * Decompose point proba to arcs.
	 * @param k the \f$k^{th}\f$ GPS point
	 * @param arcDDR the arc with DDr
	 */
	patReal calPointArcProba(short k,pair<patArc*,patReal> arc_DDR);

protected:
    vector<patReal> m_proba;
    patMapMatchingIteration* m_mm_iteration;
    patPathJ* m_path;
    patReal m_init_arc_proba;
    vector<map<patArc*, double> > m_point_arc_probas;
    set<patArcTransition>* m_arc_tran_set;

};

#endif /* PATMULTIMODALPATHPROBA_H_ */
