/*
 * patRoadBase.h
 *
 *  Created on: Nov 1, 2011
 *      Author: jchen
 */

#ifndef PATROADBASE_H_
#define PATROADBASE_H_
#include <list>
#include "dataStruct.h"
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
	patRoadBase(const patRoadBase& another);
	/**
	 * Set length of the road.
	 */
	void setLength(bool the_length);

	/**
	 * Pure virtual function. Get the length of the road.
	 */
	 double getLength() const;

	/**
	 * Pure virtual function. Calculate the length of the road.
	 */
	virtual double computeLength()  = 0;

	 double getGeneralizedCost() const;
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
	virtual vector<const patArc*> getArcList( ) const=0;
	virtual double getAttribute(ARC_ATTRIBUTES_TYPES attribute) const=0;
	virtual double computeGeneralizedCost(const map<ARC_ATTRIBUTES_TYPES, double>& link_coef);
	 FolderPtr getKML(string mode) const;
	virtual bool isValid() const=0;
	virtual int size() const=0;
	//virtual double getAttribute(string attribute) const;
	virtual ~patRoadBase();
protected:
	double m_length;
	double m_generalized_cost;
};

#endif /* PATROADBASE_H_ */
