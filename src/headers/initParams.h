/*
 * initParams.h
 *
 *  Created on: Jul 11, 2012
 *      Author: jchen
 */

#ifndef INITPARAMS_H_
#define INITPARAMS_H_




#include "patError.h"
#include "patNBParameters.h"
void initParameters(string param_file) {

	patError* err(NULL);

	//Read parameters

	patNBParameters::the()->readFile(param_file, err);
	patNBParameters::the()->init(err);
	if (err != NULL) {
		exit(-1);
	}
}
#endif /* INITPARAMS_H_ */
