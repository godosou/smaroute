/*
 * patRoadBase.h
 *
 *  Created on: Nov 1, 2011
 *      Author: jchen
 */

#ifndef PATROADBASE_H_
#define PATROADBASE_H_
#include <list>

#include "kml/dom.h"
using kmldom::FolderPtr;
using namespace std;
class patNode;
class patArc;

class patRoadBase {

	/**
	 * Base class for any road type.
	 * Derive class: patArc, patPublicTransportSegment, patWay
	 */
public:
	patRoadBase();
	/**
	 * Set length of the road.
	 */
	void setLength(bool the_length);

	/**
	 * Pure virtual function. Get the length of the road.
	 */
	virtual double getLength() const= 0;

	/**
	 * Pure virtual function. Calculate the length of the road.
	 */
	virtual double computeLength()  = 0;

	/**
	 * Virtual function get the up node.
	 */
	virtual const patNode *getUpNode() const=0;
	/**
	 * Virtual function get the up node.
	 */
	virtual const patNode* getDownNode() const=0;
	/**
	 * Virtual function get the list of arcs.
	 */
	virtual list<const patArc*> getArcList( ) const=0;

	 FolderPtr getKML(string mode) const;
	virtual bool isValid() const=0;
	virtual int size() const=0;
	virtual ~patRoadBase();
protected:
	double m_length;
};

#endif /* PATROADBASE_H_ */
