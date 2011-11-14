/*
 * patPublicTransportSegment.h
 *
 *  Created on: Jul 22, 2011
 *      Author: jchen
 */

#ifndef PATPUBLICTRANSPORTSEGMENT_H_
#define PATPUBLICTRANSPORTSEGMENT_H_

#include "patWay.h"
#include "patNode.h"
#include "patNetworkElements.h"
#include <list>

/**
 * Public transport segment between two stops. It is essentially an ordered list of ways;
 */
class patPublicTransportSegment {
public:
	patPublicTransportSegment();

	/**
	 * Append way_id way to the end;
	 * @param way_id the id of the way
	 */
	bool pushBack(patNetworkElements* network,patULong way_id);

	/**
	 * Append way_id way to the end;
	 * @param way_id the id of the way
	 */
	bool pushFront(patNetworkElements* network,patULong way_id);

	bool isError();
	/**
	 * Append new way to the end;
	 * @param new_way Pointer to the new way
	 */
	void pushBack(patWay* new_way);
	/**
	 * Add new way to the front
	 * @param new_way Pointer to the new way
	 */
	void pushFront(patWay* new_way);

	/**
	 * return the pointer to the up node
	 */
	patNode* getUpNode();

	/**
	 * return the pointer to the down node
	 */
	patNode* getDownNode();
	virtual ~patPublicTransportSegment();
protected:
	list<patWay*> ways;
	bool error;
};

#endif /* PATPUBLICTRANSPORTSEGMENT_H_ */
