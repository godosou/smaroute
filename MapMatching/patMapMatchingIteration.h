/*
 * patMapMatchingIteration.h
 *
 *  Created on: Oct 12, 2011
 *      Author: jchen
 */

#ifndef PATMAPMATCHINGITERATION_H_
#define PATMAPMATCHINGITERATION_H_
#include "patNode.h"
#include "patGpsPoint.h"
#include "patGpsDDR.h"

class patMapMatchingIteration {
public:
	patMapMatchingIteration(patGpsPoint* a_gps);

	/**
	 * Extend from a node
	 */
	void extendFromNode(patNode* node);
	virtual ~patMapMatchingIteration();
protected:

	vector<patGpsPoint*>* m_gps_sequence ;//The gps sequence up to previous one.
	const vector<patGpsPoint>* m_original_gps_sequence;//The original gps sequence.

	patGpsPoint* m_gps;
	patGpsDDR m_gps_ddr;
	map<patPathJ,double>  m_paths;
	double m_total_proba;
	set<patArcTransition*>* m_arc_tran_set;
	const patNetworkEnvironment* m_environment;
};

#endif /* PATMAPMATCHINGITERATION_H_ */
