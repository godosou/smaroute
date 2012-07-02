/*
 * patUtilityFunction.cc
 *
 *  Created on: Jun 1, 2012
 *      Author: jchen
 */

#include "patUtilityFunction.h"
#include <vector>
#include "patMultiModalPath.h"
#include "patArc.h"
#include <tr1/unordered_map>
using namespace std;
using namespace std::tr1;

patUtilityFunction::~patUtilityFunction() {
	// TODO Auto-generated destructor stub
}

patUtilityFunction::patUtilityFunction(double link_scale, double length_coef,
		double ps_coef, double sb_coef) :
		patLinkAndPathCost::patLinkAndPathCost(link_scale, length_coef, ps_coef,
				sb_coef) {

}

map<ARC_ATTRIBUTES_TYPES, double> patUtilityFunction::getAttributes(
		const patMultiModalPath& path) const {
	map<ARC_ATTRIBUTES_TYPES, double> attributes;
	for (map<ARC_ATTRIBUTES_TYPES, double>::const_iterator a_iter =
			m_link_coefficients.begin(); a_iter != m_link_coefficients.end();
			++a_iter) {
		//DEBUG_MESSAGE(a_iter->second<<"*"<<arc->getAttribute(a_iter->first));
		double cost = 0.0;
		vector<const patArc*> arcs = path.getArcList();
		for (vector<const patArc*>::const_iterator arc_iter = arcs.begin();
				arc_iter != arcs.end(); ++arc_iter) {
			cost += ((double) (*arc_iter)->getAttribute(a_iter->first));
		}
		attributes[a_iter->first] = cost; //FIXME scale parameter
	}
	return attributes;
}
