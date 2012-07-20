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
#include "patPathSizeComputer.h"
using namespace std::tr1;
using namespace std;
class patRoadBase;
class patPathSizeComputer;
class patLinkAndPathCost {
public:

	patLinkAndPathCost(const map<ARC_ATTRIBUTES_TYPES, double>& link_coef,
			const double &link_scale, const double &ps_scale);
	patLinkAndPathCost(const patLinkAndPathCost& another) :
			m_link_cost_scale(another.m_link_cost_scale), m_link_coefficients(
					another.m_link_coefficients), m_pathsize_coefficient(
					another.m_pathsize_coefficient)

	{
		if (another.m_ps_computer != NULL) {
			m_ps_computer = another.m_ps_computer->clone();
		} else {
			m_ps_computer = NULL;
		}
	}

	virtual patLinkAndPathCost* clone() const;
	double getCost(const patRoadBase* road) const;
	double getCost(const patMultiModalPath& path) const;

	double getCostWithPathSize(const patMultiModalPath& path,
			double path_size) const;
	void setLinkCostScale(const double linkCostScale);
	void setPathSizeComputer(patPathSizeComputer* pathsize);
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

	map<string, double> getAttributes(const patMultiModalPath& path) const;
protected:
	const map<ARC_ATTRIBUTES_TYPES, double> m_link_coefficients;
	const double m_pathsize_coefficient;
	const double m_link_cost_scale;
	patPathSizeComputer* m_ps_computer;
};

#endif /* PATPATHCOST_H_ */
