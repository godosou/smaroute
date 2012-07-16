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

