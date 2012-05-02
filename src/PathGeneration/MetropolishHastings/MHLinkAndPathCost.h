/*
 * MHLinkAndPathCost.h
 *
 *  Created on: Apr 26, 2012
 *      Author: jchen
 */

#ifndef MHLINKANDPATHCOST_H_
#define MHLINKANDPATHCOST_H_
#include "patLinkCost.h"
#include "MHPath.h"
#include "MHWeight.h"
class MHLinkAndPathCost: public patLinkCost, public MHWeight<MHPath> {
public:
	// XML CONFIGURATION

	static const string LINKCOST_ELEMENT ;

	static const string ATTRIBUTE_ELEMENT;

	static const string COEFFICIENT_ELEMENT;

	static const string LINKCOSTSCALE_ELEMENT ;

	static const string NODELOOPSCALE_ELEMENT ;

	// DEFAULT VALUES

	static const double DEFAULT_NODELOOPSCALE = 0.0;

	static void configure();
	MHLinkAndPathCost();
	virtual ~MHLinkAndPathCost();

	void  setLinkCostScale(const double linkCostScale);
	double  getLinkCostScale() const;
	void  setNodeLoopScale(const double nodeLoopScale);
	double  getNodeLoopScale() const;
	double  getCost(const patArc* arc) const;
	double  logWeightWithoutCorrection(
			const MHPath& path) const;
	double logWeight(const MHPath& path) const;

private:
	// CONFIGURATION

	list<string> m_attributes;

	static map<string,double> m_link_coefficients;
	static map<string,double> m_path_coefficients;

	static double m_linkCostScale;

	static double m_nodeLoopScale;

};

#endif /* MHLINKANDPATHCOST_H_ */
