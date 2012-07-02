/*
 * patLinkCost.h
 *
 *  Created on: Apr 20, 2012
 *      Author: jchen
 */

#ifndef PATABSTRACTCOST_H_
#define PATABSTRACTCOST_H_
#include "patArc.h"
#include "patRoadBase.h"
#include "patTransportMode.h"
class patMultiModalPath;
class patAbstractCost {
public:
	patAbstractCost();
	virtual double getCost(const patRoadBase* road) const = 0;
	virtual double getCost(const patMultiModalPath& path ) const = 0;
	//virtual double getCost(const patRoadBase* road, const TransportMode& mode) const;
	//virtual double getCost(const patArc* arc, const TransportMode& mode) const=0;
	virtual ~patAbstractCost();

};

#endif /* PATLINKCOST_H_ */
