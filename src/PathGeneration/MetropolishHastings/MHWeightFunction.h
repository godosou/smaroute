/*
 * MHLinkAndPathCost.h
 *
 *  Created on: Apr 26, 2012
 *      Author: jchen
 */

#ifndef MHWEIGHTFUNCTION_H_
#define MHWEIGHTFUNCTION_H_
#include "patLinkAndPathCost.h"
#include "MHPath.h"
#include "patNBParameters.h"
#include "MHWeight.h"
#include "patRoadBase.h"
#include "patWeightFunction.h"
#include <tr1/unordered_map>
class patMultiModalPath;
class patNode;
using namespace std::tr1;
class MHWeightFunction: public patLinkAndPathCost,
		public MHWeight<MHPath>,
		public patWeightFunction {
public:

	/**
	 * Constructor.
	 */
	MHWeightFunction(const map<ARC_ATTRIBUTES_TYPES, double>& link_coef,
			const double &link_scale, const double &ps_scale,
			const double obs_scale );

	virtual MHWeightFunction* clone() const;
	MHWeightFunction(const MHWeightFunction& another);
	/**
	 * Deconstructor.
	 */
	virtual ~MHWeightFunction();

	double getObsWeightScale() const;
	/**
	 * Calculate the log weight of a path (without correction).
	 * @param path: the path to be calculated.
	 */
	virtual double logWeigthOriginal(const patMultiModalPath& path) const;

	double computeObsWeight(const patMultiModalPath& path) const;
	double logWeightWithoutCorrection(const MHPath& path) const {
		return logWeigthOriginal(path);
	}
	/**
	 * Calculate the log weight of a path.
	 * @param path: the path to be calculated.
	 */
	virtual double logWeight(const MHPath& path) const;

	void setPathProbas(const map<const patMultiModalPath, double>* path_probas);

	double calculateObsScale(const patMultiModalPath& sp_path);
protected:
	// CONFIGURATION

	const map<const patMultiModalPath, double>* m_path_probas;
	double m_obs_scale;
};

#endif /* MHLINKANDPATHCOST_H_ */
