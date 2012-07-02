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
class patMultiModalPath;
class patUtilityFunction: public patLinkAndPathCost {
public:
	patUtilityFunction(
			double link_scale = patNBParameters::the()->utility_link_scale,
			double length_coef = patNBParameters::the()->utility_length_coef,
			double ps_coef = patNBParameters::the()->utility_ps_coef,
			double sb_coef = patNBParameters::the()->utility_sb_coef);
	/*
	 * Get attributes of a path for the utility function.
	 * @param path: the path.
	 * @return: hashed map with key and value.
	 */
	map<ARC_ATTRIBUTES_TYPES, double> getAttributes(
			const patMultiModalPath& path) const;

	virtual ~patUtilityFunction();
};

#endif /* PATUTILITYFUNCTION_H_ */
