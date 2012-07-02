/*
 * patNetworkReducer.h
 *
 *  Created on: Apr 28, 2012
 *      Author: jchen
 */

#ifndef PATNETWORKREDUCER_H_
#define PATNETWORKREDUCER_H_
#include "patNode.h"
#include "patLinkAndPathCost.h"
#include "patNetworkBase.h"
class patNetworkReducer {
public:
	patNetworkReducer(const patNode* origin,
			const patNode* destination, const patLinkAndPathCost* linkCost,
			double expansion);

	patNetworkReducer(const patNode* origin,
			const patNode* destination, const patLinkAndPathCost* linkCost);
	void reduce(patNetworkBase* network);
	virtual ~patNetworkReducer();
protected:
	const patNode* m_origin;
	const patNode* m_destination;
	const patLinkAndPathCost* m_linkCost;
	double m_expansion;
};

#endif /* PATNETWORKREDUCER_H_ */
