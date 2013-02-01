/*
 * MHPathSimWeight.h
 *
 *  Created on: Jul 30, 2012
 *      Author: jchen
 */

#ifndef MHPATHSIMWEIGHT_H_
#define MHPATHSIMWEIGHT_H_

#include <MHWeight.h>
#include "patWeightFunction.h"
#include "MHPath.h"
class patMultiModalPath;
class MHPathSimWeight: public MHWeight<MHPath>, public patWeightFunction {
public:
	MHPathSimWeight(const double& path_distance_scale);
	double logWeigthOriginal(const patMultiModalPath& path) const;

	void setPath(const patMultiModalPath* m_original_path);	virtual ~MHPathSimWeight();
protected:
	const patMultiModalPath* m_original_path;
	const double m_path_distance_scale;
};

#endif /* MHPATHSIMWEIGHT_H_ */
