/*
 * patComputeBoundingBox.h
 *
 *  Created on: Aug 21, 2012
 *      Author: jchen
 */

#ifndef PATCOMPUTEBOUNDINGBOX_H_
#define PATCOMPUTEBOUNDINGBOX_H_
#include "patGeoBoundingBox.h"
#include <vector>
class patComputeBoundingBox {
public:
	patComputeBoundingBox();
	static patGeoBoundingBox computeByGPSFiles(const std::vector<std::string>& files);
	virtual ~patComputeBoundingBox();
};

#endif /* PATCOMPUTEBOUNDINGBOX_H_ */
