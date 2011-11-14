/*
 * patShortestpathTreeGeneral.h
 *
 *  Created on: Nov 1, 2011
 *      Author: jchen
 */

#ifndef PATSHORTESTPATHTREEGENERAL_H_
#define PATSHORTESTPATHTREEGENERAL_H_
#include "patRoadBase.h"
#include <list>
class patShortestpathTreeGeneral {
public:
	patShortestpathTreeGeneral();
	patPathJ getShortestPathTo(patNode* end_node);
	virtual ~patShortestpathTreeGeneral();
};

#endif /* PATSHORTESTPATHTREEGENERAL_H_ */
