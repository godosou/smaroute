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
#include "patRoadBase.h"
#include "patArcSequence.h"
class patNetworkElements;
#include <list>

/**
 * Public transport segment between two stops. It is essentially an ordered list of arcs;
 */
class patPublicTransportSegment: public patArcSequence {
	friend ostream& operator<<(ostream& str, const patPublicTransportSegment& x) ;

	friend bool operator<(const patPublicTransportSegment& seg_a, const patPublicTransportSegment& seg_b);
	friend bool operator==(const patPublicTransportSegment& seg_a, const patPublicTransportSegment& seg_b);
public:
	patPublicTransportSegment();

	virtual bool addArcToBack(const patArc* arc, int direction);
	virtual const patArc* deleteArcToBack();
	/**
	 * Append way_id way to the end;
	 * @param way_id the id of the way
	 */
	bool pushBack(patNetworkElements* network, unsigned long way_id);

	/**
	 * Append way_id way to the end;
	 * @param way_id the id of the way
	 */
	bool pushFront(patNetworkElements* network, unsigned long way_id);
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
	virtual ~patPublicTransportSegment();
	map<string, string> attributes;
protected:
	list<int> m_directions;
	string m_name;
	string m_route_type;
	string m_ref;
};

#endif /* PATPUBLICTRANSPORTSEGMENT_H_ */
