/*
 * patUtilityFunction.h
 *
 *  Created on: Jun 1, 2012
 *      Author: jchen
 */

#ifndef PATUTILITYFUNCTION_H_
#define PATUTILITYFUNCTION_H_
#include "patLinkAndPathCost.h"
#include "patNBParameters.h"
class patUtilityFunction: public patLinkAndPathCost {
public:
	patUtilityFunction(const map<ARC_ATTRIBUTES_TYPES, double>& link_coef,
			const double &link_scale, const double &ps_scale);

	virtual ~patUtilityFunction();
};

#endif /* PATUTILITYFUNCTION_H_ */
