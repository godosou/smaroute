/*
 * MHEqualWeightFunction.h
 *
 *  Created on: Jul 30, 2012
 *      Author: jchen
 */

#ifndef MHEQUALWEIGHTFUNCTION_H_
#define MHEQUALWEIGHTFUNCTION_H_

#include "MHWeightFunction.h"
#include "patMultiModalPath.h"
/**
 * Weight function returns equal log weight (-1) for any path
 */
class MHEqualWeightFunction: public MHWeightFunction {
public:

	/**
	 * Constructor do nothing
	 */
	MHEqualWeightFunction();

	/**
	 * Copy an object.
	 * @param another: another function to be copied
	 * Do nothing
	 */
	MHEqualWeightFunction(const MHEqualWeightFunction& another);

	/**
	 * Clone  an object and return the pointer.
	 * It creates a new object. The pointer should be manually destroyed when the lifetime is gone.
	 */
	MHEqualWeightFunction* clone() const;

	/**
	 * Compute the log weight (original without point size correction).
	 * @return -1 for any path
	 */
	double logWeigthOriginal(const patMultiModalPath& path) const;

 void calibrate(const patMultiModalPath& path);
	/**
	 * Deconstructor, do nothing.
	 */
	virtual ~MHEqualWeightFunction();
};

#endif /* MHEQUALWEIGHTFUNCTION_H_ */
