/*
 * MHEqualWeightFunction.cc
 *
 *  Created on: Jul 30, 2012
 *      Author: jchen
 */

#include "MHEqualWeightFunction.h"

MHEqualWeightFunction::MHEqualWeightFunction() {
	// TODO Auto-generated constructor stub

}

MHEqualWeightFunction::MHEqualWeightFunction(
		const MHEqualWeightFunction& another) :
		MHWeightFunction::MHWeightFunction(another) {

}
MHEqualWeightFunction* MHEqualWeightFunction::clone() const{
	return new MHEqualWeightFunction(*this);
}
double MHEqualWeightFunction::logWeigthOriginal(
		const patMultiModalPath& path) const{
	return -1;
}

MHEqualWeightFunction::~MHEqualWeightFunction() {
}


void MHEqualWeightFunction::calibrate(const patMultiModalPath& path) {
}
