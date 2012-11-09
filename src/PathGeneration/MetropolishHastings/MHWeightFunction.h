/*
 * MHLinkAndPathCost.h
 *
 *  Created on: Apr 26, 2012
 *      Author: jchen
 */

#ifndef MHWEIGHTFUNCTION_H_
#define MHWEIGHTFUNCTION_H_
#include "MHPath.h"
#include "patNBParameters.h"
#include "MHWeight.h"
#include "patWeightFunction.h"
#include "patLinkAndPathCost.h"
using namespace std::tr1;
class MHWeightFunction:
		public MHWeight<MHPath>,
		public patWeightFunction {
public:
			MHWeightFunction();
	MHWeightFunction(const MHWeightFunction& another);
	virtual ~MHWeightFunction();
	virtual MHWeightFunction* clone() const=0;

	virtual double logWeigthOriginal(const patMultiModalPath& path) const=0;
	virtual patMultiModalPath getMostLikelyPath(const patMultiModalPath& sp_path) const;
	/**
	 * Call logWeigthOriginal
	 */
	double logWeightWithoutCorrection(const MHPath& path) const;

	/**
	 * Calculate the log weight of a path.
	 * @param path: the path to be calculated.
	 */
	virtual void calibrate(const patMultiModalPath& path) =0 ;
	virtual double logWeight(const MHPath& path) const;

};

#endif /* MHLINKANDPATHCOST_H_ */
