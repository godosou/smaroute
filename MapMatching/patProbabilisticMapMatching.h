/*
 * patProbabilisticMapMatching.h
 *
 *  Created on: Nov 10, 2011
 *      Author: jchen
 */

#ifndef PATPROBABILISTICMAPMATCHING_H_
#define PATPROBABILISTICMAPMATCHING_H_
#include "patGpsSequence"
class patProbabilisticMapMatching {
public:
	patProbabilisticMapMatching();
	virtual ~patProbabilisticMapMatching();
protected:
	patGpsSequence m_gps_sequence;
	set<patArcTransition> m_arc_tran_st;
};

#endif /* PATPROBABILISTICMAPMATCHING_H_ */
