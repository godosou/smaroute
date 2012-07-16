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
class patNetworkBase;
class patNode;
using namespace std::tr1;
class MHWeightFunction: public patLinkAndPathCost, public MHWeight<MHPath> ,public patWeightFunction{
public:

	/**
	 * Constructor.
	 */
	MHWeightFunction(double link_scale=patNBParameters::the()->mh_link_scale, double length_coef=patNBParameters::the()->mh_length_coef,
			double ps_coef = patNBParameters::the()->mh_ps_coef,
			double obs_scale = patNBParameters::the()->mh_obs_scale,
			double sb_coef=patNBParameters::the()->mh_sb_coef);
	MHWeightFunction(map<ARC_ATTRIBUTES_TYPES, double>& link_coef, double &link_scale, double &ps_scale, double obs_scale);


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

	virtual double logWeightWithoutCorrection(const MHPath& path) const{
		return logWeigthOriginal(path);
	}
	/**
	 * Calculate the log weight of a path.
	 * @param path: the path to be calculated.
	 */
	 virtual double logWeight(const MHPath& path) const;


	void setPathProbas(const map<const patMultiModalPath, double>* path_probas);

	double calculateObsScale(const patNetworkBase* network,const patNode* origin, const patNode* destination);
	protected:
	// CONFIGURATION

	const map<const patMultiModalPath, double>* m_path_probas;
	double m_obs_scale;
};

#endif /* MHLINKANDPATHCOST_H_ */
