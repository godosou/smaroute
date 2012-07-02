/*
 * patPathCost.h
 *
 *  Created on: Jun 1, 2012
 *      Author: jchen
 */

#ifndef PATLINKANDPATHCOST_H_
#define PATLINKANDPATHCOST_H_
//#include <map>
#include <tr1/unordered_map>
#include "patMultiModalPath.h"
#include "patNBParameters.h"
using namespace std::tr1;
using namespace std;
class patRoadBase;
class patLinkAndPathCost {
public:
//	patLinkAndPathCost();
	patLinkAndPathCost(double link_scale, double length_coef, double ps_coef,
			double sb_coef);
	patLinkAndPathCost(map<ARC_ATTRIBUTES_TYPES, double>& link_coef, double &link_scale, double &ps_scale);
	patLinkAndPathCost(const patLinkAndPathCost& another) {
		m_link_coefficients = another.m_link_coefficients;
		m_path_coefficients = another.m_path_coefficients;
		m_link_cost_scale = another.m_link_cost_scale;
		m_path_cost_scale = another.m_path_cost_scale;
		m_pathsize_coefficient = another.m_pathsize_coefficient;
	}
	virtual double getCost(const patRoadBase* road) const;
	virtual double getCost(const patMultiModalPath& path) const;

	virtual double getCostWithPathSize(const patMultiModalPath& path,
			double path_size) const;
	void setLinkCostScale(const double linkCostScale);
	void setPathSize(const map<const patMultiModalPath, double>* pathsize);
	/**
	 * get scale parameter $\mu$ of the link cost.
	 * @return double: the link cost scale.
	 */

	double getLinkCostScale() const;
	void deleteLinkCoefficient(ARC_ATTRIBUTES_TYPES coef_type);
	void setLinkCoefficient(ARC_ATTRIBUTES_TYPES coef_type, double coef_value);
	double getPathSizeCoefficient() const;

	/**
	 * Get link additive cost coefficient.
	 */
	const map<ARC_ATTRIBUTES_TYPES, double>& getLinkCoefficients() const;
	/**
	 * Get non-link additive cost coefficient.
	 */
	const unordered_map<const char*, double>& getPathCoefficients() const;
	virtual ~patLinkAndPathCost();
protected:
	map<ARC_ATTRIBUTES_TYPES, double> m_link_coefficients;
	unordered_map<const char*, double> m_path_coefficients;
	const map<const patMultiModalPath, double>* m_pathsize;
	double m_link_cost_scale;
	double m_path_cost_scale;
	double m_pathsize_coefficient;
};

#endif /* PATPATHCOST_H_ */
