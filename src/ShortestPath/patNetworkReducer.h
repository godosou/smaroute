/*
 * patNetworkReducer.h
 *
 *  Created on: Apr 28, 2012
 *      Author: jchen
 */

#ifndef PATNETWORKREDUCER_H_
#define PATNETWORKREDUCER_H_
#include "patNode.h"
#include "patLinkCost.h"
#include "patNetworkBase.h"
class patNetworkReducer {
public:
	patNetworkReducer(const patNode* origin,
			const patNode* destination, const patLinkCost* linkCost,
			double expansion);

	patNetworkReducer(const patNode* origin,
			const patNode* destination, const patLinkCost* linkCost);
	void reduce(patNetworkBase* network);
	virtual ~patNetworkReducer();
protected:
	const patNode* m_origin;
	const patNode* m_destination;
	const patLinkCost* m_linkCost;
	double m_expansion;
};

#endif /* PATNETWORKREDUCER_H_ */
