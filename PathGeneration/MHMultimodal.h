/*
 * MHMultimodal.h
 *
 *  Created on: Oct 28, 2011
 *      Author: jchen
 */

#ifndef MHMULTIMODAL_H_
#define MHMULTIMODAL_H_
#include "patNode.h"
#include "patMapMatchingEnvironment.h"
#include "patPathJ"
class MHMultimodal {
public:
	MHMultimodal(patNode* the_origin, patNode* the_destination, patNetworkEnvironment* the_environment);
	void initiate();
	void shuffle();
	void splice();

	virtual ~MHMultimodal();
protected:
	patNode* origin;
	patNode* destination;
	patNetworkEnvironment* environment;
};

#endif /* MHMULTIMODAL_H_ */
