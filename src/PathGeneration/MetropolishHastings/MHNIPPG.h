/*
 * MHNIPPG.h
 *
 *  Created on: Jul 30, 2012
 *      Author: jchen
 */

#ifndef MHNIPPG_H_
#define MHNIPPG_H_

#include "MHNodeInsertionProbability.h"
#include <tr1/unordered_map>
class patNode;

class MHNIPPG: public MHNodeInsertionProbability {
public:

	std::tr1::unordered_map<const patNode*, double> calculate(
			const patNode* origin, const patNode* destination) const;
	virtual ~MHNIPPG();
};

#endif /* MHNIPPG_H_ */
