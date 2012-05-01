/*
 * patLinkCost.h
 *
 *  Created on: Apr 20, 2012
 *      Author: jchen
 */

#ifndef PATLINKCOST_H_
#define PATLINKCOST_H_
#include "patArc.h"
#include "patRoadBase.h"
#include "patTransportMode.h"
class patLinkCost {
public:
	patLinkCost();
	virtual double getCost(const patRoadBase* road) const;
	virtual double getCost(const patArc* arc) const=0;

	//virtual double getCost(const patRoadBase* road, const TransportMode& mode) const;
	//virtual double getCost(const patArc* arc, const TransportMode& mode) const=0;
	virtual ~patLinkCost();

};

#endif /* PATLINKCOST_H_ */
