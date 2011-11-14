/*
 * patRoadBase.h
 *
 *  Created on: Nov 1, 2011
 *      Author: jchen
 */

#ifndef PATROADBASE_H_
#define PATROADBASE_H_

class patNode;
class patRoadBase {

	/**
	 * Base class for any road type.
	 * Derive class: patArc, patPublicTransportSegment, patWay
	 */
public:
	patRoadBase();
	void setLength(bool the_length);
	virtual double getLength(void) = 0;
	virtual double computeLength(void) = 0;
	virtual const patNode* getUpNode(void) =0;
	virtual const patNode* getDownNode(void) =0;

	virtual ~patRoadBase();
protected:
	double length;
};

#endif /* PATROADBASE_H_ */
