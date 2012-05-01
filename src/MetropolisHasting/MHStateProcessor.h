/*
 * MHStateProcessor.h
 *
 *  Created on: Feb 29, 2012
 *      Author: jchen
 */

#ifndef MHSTATEPROCESSOR_H_
#define MHSTATEPROCESSOR_H_

template <class S>
class MHStateProcessor {
public:
	virtual void start() =0;

	virtual void processState(const S &state) =0;

	virtual void end() =0;

};

#endif /* MHSTATEPROCESSOR_H_ */
