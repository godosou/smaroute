/*
 * patGetPathProbasFromObservations.h
 *
 *  Created on: Jul 11, 2012
 *      Author: jchen
 */

#ifndef PATGETPATHPROBASFROMOBSERVATIONS_H_
#define PATGETPATHPROBASFROMOBSERVATIONS_H_

#include "patObservation.h"
#include "patMultiModalPath.h"
#include <map>
#include <vector>
class patGetPathProbasFromObservations {
public:
	patGetPathProbasFromObservations();

	const std::map<const patMultiModalPath, double> getPathProbas(const std::vector<patObservation>& observations) const;
	virtual ~patGetPathProbasFromObservations();
};

#endif /* PATGETPATHPROBASFROMOBSERVATIONS_H_ */
