/*
 * patWeightFunction.h
 *
 *  Created on: Jun 26, 2012
 *      Author: jchen
 */

#ifndef PATWEIGHTFUNCTION_H_
#define PATWEIGHTFUNCTION_H_

class patMultiModalPath;
class patWeightFunction {
public:
	virtual double logWeigthOriginal(const patMultiModalPath& path) const=0;


};

#endif /* PATWEIGHTFUNCTION_H_ */
