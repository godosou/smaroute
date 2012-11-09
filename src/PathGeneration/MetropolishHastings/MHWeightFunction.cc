#include "MHWeightFunction.h"
MHWeightFunction::MHWeightFunction(const MHWeightFunction& another) {

}

MHWeightFunction::MHWeightFunction() {

}
MHWeightFunction::~MHWeightFunction() {

}
double MHWeightFunction::logWeightWithoutCorrection(const MHPath& path) const {
	return logWeigthOriginal(path);
}

double MHWeightFunction::logWeight(const MHPath& path) const {
	return (logWeigthOriginal(path) - log(path.pointCombinationSize()));

}
// void MHWeightFunction::calibrate(const patMultiModalPath& path) {
//	 throw RuntimeException("It should not be called!!!!");
//}

 patMultiModalPath MHWeightFunction::getMostLikelyPath(const patMultiModalPath& sp_path) const{
	return sp_path;
}
