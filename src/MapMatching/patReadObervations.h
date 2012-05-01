/*
 * patReadObervations.h
 *
 *  Created on: Nov 25, 2011
 *      Author: jchen
 */

#ifndef PATREADOBERVATIONS_H_
#define PATREADOBERVATIONS_H_
#include "patError.h"

class patReadObervations {
public:
	patReadObervations();
	static void readFromFile(string file_name, patError*& err);
	virtual ~patReadObervations();
};

#endif /* PATREADOBERVATIONS_H_ */
