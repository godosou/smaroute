/*
 * MHSamplingWeightFunction.h
 *
 *  Created on: Jul 30, 2012
 *      Author: jchen
 */

#ifndef MHSAMPLINGWEIGHTFUNCTION_H_
#define MHSAMPLINGWEIGHTFUNCTION_H_

#include "patLinkAndPathCost.h"
#include "MHPath.h"
#include "patNBParameters.h"
#include "MHWeight.h"
#include "patRoadBase.h"
#include "patWeightFunction.h"
#include "MHWeightFunction.h"
#include <tr1/unordered_map>
class patMultiModalPath;
class patNode;
using namespace std::tr1;
class MHSamplingWeightFunction: public MHWeightFunction,
		public patLinkAndPathCost {
public:
	MHSamplingWeightFunction(const map<ARC_ATTRIBUTES_TYPES, double>& link_coef,
			const double &link_scale, const double &ps_scale,
			const double obs_scale);
	MHSamplingWeightFunction(const MHSamplingWeightFunction& another);

	virtual ~MHSamplingWeightFunction();

	virtual MHSamplingWeightFunction* clone() const;
	void setPathProbas(const map<const patMultiModalPath, double>* path_probas);

	double calculateObsScale(const patMultiModalPath& sp_path);
	virtual patMultiModalPath getMostLikelyPath(const patMultiModalPath& sp_path) const;
	double getObsWeightScale() const;
	/**
	 * Calculate the log weight of a path (without correction).
	 * @param path: the path to be calculated.
	 */
	virtual double logWeigthOriginal(const patMultiModalPath& path) const;
	void calibrate(const patMultiModalPath& path);
	double computeObsWeight(const patMultiModalPath& path) const;

protected:
	// CONFIGURATION

	const int m_function_type;
	const map<const patMultiModalPath, double>* m_path_probas;
	double m_obs_scale;
};

#endif /* MHSAMPLINGWEIGHTFUNCTION_H_ */
