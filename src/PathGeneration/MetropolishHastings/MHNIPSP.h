/*
 * MHNIPSP.h
 *
 *  Created on: Jul 30, 2012
 *      Author: jchen
 */

#ifndef MHNIPSP_H_
#define MHNIPSP_H_

#include "MHNodeInsertionProbability.h"
#include <tr1/unordered_map>
class patNode;

class MHNIPSP: public MHNodeInsertionProbability {
public:
	std::tr1::unordered_map<const patNode*, double> calculate(
			const patNode* origin, const patNode* destination) const;
	virtual ~MHNIPSP();
};

#endif /* MHNIPSP_H_ */
