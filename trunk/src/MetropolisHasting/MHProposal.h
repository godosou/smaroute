/*
 * MHProposal.h
 *
 *  Created on: Feb 29, 2012
 *      Author: jchen
 */

#ifndef MHPROPOSAL_H_
#define MHPROPOSAL_H_
#include "MHTransition.h"
template <class S>
class MHProposal {
public:
	virtual S newInitialState() = 0;

	virtual MHTransition<S> newTransition(S& state) = 0;

};

#endif /* MHPROPOSAL_H_ */
