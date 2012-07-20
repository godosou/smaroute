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

#include  "patNetworkElements.h"
patUtilityFunction::~patUtilityFunction() {
	// TODO Auto-generated destructor stub
}

patUtilityFunction::patUtilityFunction(
		const map<ARC_ATTRIBUTES_TYPES, double>& link_coef,
		const double &link_scale, const double &ps_scale) :
		patLinkAndPathCost::patLinkAndPathCost(link_coef, link_scale, ps_scale) {

}

