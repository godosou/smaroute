#include <fstream>

#include <gsl/gsl_integration.h>
#include "patConst.h"

#include<sstream>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_monte.h>
#include <gsl/gsl_monte_plain.h>
#include <gsl/gsl_monte_miser.h>
#include <gsl/gsl_monte_vegas.h>

#include "patErrMiscError.h"
#include "patError.h"
#include "patString.h"
#include "patErrNullPointer.h"

//in the same library
#include "patArcTransition.h"
#include "patMeasurement.h"
//in Utilities
#include "patDisplay.h"
#include "patNBParameters.h"

//in Models
#include "patMeasurementDDR.h"
#include "patMeasurementModel.h"
#include "patMeasurementModelV2.h"
#include "patTrafficModelComplex.h"
#include "patBTMeasurementModel.h"
#include "patAccelMeasurementModel.h"

patArcTransition::patArcTransition(patMultiModalPath path,
		patMeasurement* measurement_prev, patMeasurement* measurement_curr) :
		m_value(-1.0), m_path(path), m_prev_measurement(measurement_prev), m_curr_measurement(
				measurement_curr) {

}

ostream& operator<<(ostream& str, const patArcTransition& x) {
	str << "<" << x.m_prev_measurement->getTimeStamp() << " - "
			<< x.m_curr_measurement->getTimeStamp() << "> ";
	str << x.m_path;

	return str;
}

bool operator==(const patArcTransition& aTran, const patArcTransition& bTran) {
	if (aTran.m_path == bTran.m_path){
		if ((aTran.m_prev_measurement == bTran.m_prev_measurement)
			&& (aTran.m_curr_measurement == bTran.m_curr_measurement)){
			return true;
		}
		else{
			if (!aTran.m_prev_measurement->isGPS() && !bTran.m_prev_measurement->isGPS() && !aTran.m_curr_measurement->isGPS() && !bTran.m_curr_measurement->isGPS()){
				return true;
			}
		}
	}
	// if ((aTran.m_prev_measurement == bTran.m_prev_measurement)
	// 		&& (aTran.m_curr_measurement == bTran.m_curr_measurement)
	// 		&& aTran.m_path == bTran.m_path) {
	// 	return true;
	// 	if (aTran.m_curr_measurement->getMeasurementType() == ACCEL) {
	// 		if (aTran.m_path.getDistanceToStop()->back()
	// 				== bTran.m_path.getDistanceToStop()->back()) {
	// 			return true;
	// 		}
	// 		else{
	// 			return false;
	// 		}
	// 	} else {
	// 		return true;
	// 	}
	// }

	return false;

}

bool operator<(const patArcTransition& aTran, const patArcTransition& bTran) {

	if (!aTran.m_prev_measurement->isGPS() && !bTran.m_prev_measurement->isGPS() && !aTran.m_curr_measurement->isGPS() && !bTran.m_curr_measurement->isGPS()){
		return (aTran.m_path < bTran.m_path);
	}
	else{
	//compare current time;
		if (aTran.m_curr_measurement < bTran.m_curr_measurement) {
			return true;
		}
		if (aTran.m_curr_measurement > bTran.m_curr_measurement) {
			return false;
		}

	//compare prev time;
		if (aTran.m_prev_measurement < bTran.m_prev_measurement) {
			return true;
		}
		if (aTran.m_prev_measurement > bTran.m_prev_measurement) {
			return false;
		}
	}
// 	if (aTran.m_curr_measurement->getMeasurementType() == ACCEL) {
// 		if (aTran.m_path.getDistanceToStop()->back()
// 				< bTran.m_path.getDistanceToStop()->back()) {
// 			return true;
// 		}
// 		if (aTran.m_path.getDistanceToStop()->back()
// 				> bTran.m_path.getDistanceToStop()->back()) {
// 			return false;
// 		}
// 	}
// //compare path

	return (aTran.m_path < bTran.m_path);
}
double baseFunction_TM(double* k, size_t dim, void* params) {
	struct base_params * fp = (struct base_params *) params;
	return fp->TM->integral(k[0], k[1]) * fp->MM_prev->integral(k[0])
			* fp->MM_curr->integral(k[1]);
}

double baseFunction_MM_2modes(double* k, size_t dim, void* params) {

	struct base_params * fp = (struct base_params *) params;
	return fp->TM->integral(k[0], k[1], k[2]) * fp->MM_prev->integral(k[0])
			* fp->MM_curr->integral(k[1]);
}

bool patArcTransition::setModels() {

	m_param.TM = new patTrafficModelComplex(m_prev_measurement,
			m_curr_measurement, &m_path);

	MeasurementType prev_measurement_type =
			m_prev_measurement->getMeasurementType();
	MeasurementType curr_measurement_type =
			m_curr_measurement->getMeasurementType();
	switch (prev_measurement_type) {
	case GPS:
		//DEBUG_MESSAGE("GPS");
		m_param.MM_prev = new patMeasurementModelV2(m_prev_measurement,
				m_path.front(), m_path.frontMode());
		break;
	case BT:
		//DEBUG_MESSAGE("Dependency not GPS, but BT");
		m_param.MM_prev = new patBTMeasurementModel(m_prev_measurement,
				m_path.front(), m_path.frontMode());
		break;
	case ACCEL:
		//DEBUG_MESSAGE("Dependency not GPS, but ACCEL");

		m_param.MM_prev = new patAccelMeasurementModel(m_prev_measurement,
				m_path.front(), m_path.frontMode(), 0.0); //FIXME

		break;
	default:
		return false;
	}

	switch (curr_measurement_type) {
	case GPS:
		//DEBUG_MESSAGE("GPS");
		m_param.MM_curr = new patMeasurementModelV2(m_curr_measurement,
				m_path.back(), m_path.backMode());
		break;
	case BT:
		m_param.MM_curr = new patBTMeasurementModel(m_curr_measurement,
				m_path.back(), m_path.backMode());
		break;
	case ACCEL:
		m_param.MM_curr = new patAccelMeasurementModel(m_curr_measurement,
				m_path.back(), m_path.backMode(),
				m_path.getDistanceToStop()->back());
		break;
	default:
		return false;
	}

	return true;

}
	 patArcTransition::~patArcTransition(){
		//deleteModels();
	}

void patArcTransition::deleteModels() {
	//DEBUG_MESSAGE("deconstructer called");
	delete m_param.MM_prev;
	m_param.MM_prev = NULL;
	delete m_param.MM_curr;
	m_param.MM_curr = NULL;
	delete m_param.TM;
	m_param.TM = NULL;

}
double patArcTransition::calProbability(patString algoType) {
//	DEBUG_MESSAGE("change points" << m_path.getChangePoints().size());

	if (algoType != "complex") {
		m_value = 0.0;
		WARNING("wrong algorithm name");
		return 0.0;
	}
	if (m_path.getChangePoints().size() > 1) {
		//	DEBUG_MESSAGE(
		//			"too many multi-modal change: " << m_path.getChangePoints().size());

		m_value = 0.0;

		//DEBUG_MESSAGE("return 0");
		//DEBUG_MESSAGE("===done===");
		return 0.0;
	}
	patError* err_pointer(NULL);

	struct link_ddr_range curr_ddr_range =
			m_curr_measurement->getDDR()->getArcDDRRange(m_path.back(),
					m_path.back_road_travel().mode, err_pointer);
	if (err_pointer != NULL) {
		WARNING("current ddr not found");
		m_value = 0.0;
		return m_value;
	}
	struct link_ddr_range prev_ddr_range =
			m_prev_measurement->getDDR()->getArcDDRRange(m_path.front(),
					m_path.front_road_travel().mode, err_pointer);
	if (err_pointer != NULL) {

		m_value = 0.0;
		WARNING("prev ddr not found");
		return m_value;
	}
	delete err_pointer;
	err_pointer = NULL;
	double res, err;

	double *xl;
	double *xu;

	setModels();

//	DEBUG_MESSAGE("========Start =========");
	gsl_monte_function G;

//	DEBUG_MESSAGE(prev_ddr_range.lower_bound<<"-"<<prev_ddr_range.upper_bound<<", "<<curr_ddr_range.lower_bound<<"-"<<curr_ddr_range.upper_bound);
	G.params = &m_param;
	unsigned long dimension;

	double resConst =  m_param.MM_prev->getConstant()
			* m_param.MM_curr->getConstant();

//DEBUG_MESSAGE(resConst);
	if (algoType == "complex" && m_path.isUniModal()) {
		/*
		 DEBUG_MESSAGE(
		 "unimodal transition with arcs:" << m_path.size());
		 */
		xl = new double[2];
		xu = new double[2];

		xl[0] = prev_ddr_range.lower_bound;
		xu[0] = prev_ddr_range.upper_bound; // ell_{k-1}
		xl[1] = curr_ddr_range.lower_bound;
		xu[1] = curr_ddr_range.upper_bound; //\ell_k
		dimension = 2;
		G.f = &baseFunction_TM;

	} else if (algoType == "complex" && m_path.getChangePoints().size() == 1) {
		/*
		 DEBUG_MESSAGE(
		 "multi-modal change: " << m_path.getChangePoints().size());
		 */
		xl = new double[3];
		xu = new double[3];

		xl[0] = prev_ddr_range.lower_bound;
		xu[0] = prev_ddr_range.upper_bound; // ell_{k-1}
		xl[1] = curr_ddr_range.lower_bound;
		xu[1] = curr_ddr_range.upper_bound; //\ell_k

		xl[2] = m_prev_measurement->getTimeStamp();
		xu[2] = m_curr_measurement->getTimeStamp(); /// t_c
		if (xl[0] < 0.0 || xu[0] > 1.0 || xl[1] < 0.0 || xu[1] > 1.0) {
			DEBUG_MESSAGE(
					xl[0] << "-" << xu[0] << "," << xl[1] << "-" << xu[1]);
		}
		dimension = 3;
		G.f = &baseFunction_MM_2modes;
	}

	for (unsigned long i = 0; i < dimension; ++i) {

		if (xl[i] >= xu[i]) {
			DEBUG_MESSAGE(
					"algorithm:" << algoType << " invalid bound " << m_path.size() << " " << i << ":" << xl[i] << "," << xu[i]);
		}
	}
	G.dim = dimension;

	const gsl_rng_type *T;
	gsl_rng *r;

	size_t calls = patNBParameters::the()->nbrOfIntegrationCalls;

	gsl_rng_env_setup();

	T = gsl_rng_default;
	r = gsl_rng_alloc(T);
	if (patNBParameters::the()->integrationMethod == "miser") {
		gsl_monte_miser_state *s = gsl_monte_miser_alloc(dimension);
		gsl_monte_miser_integrate(&G, xl, xu, dimension, calls, r, s, &res,
				&err);
		gsl_monte_miser_free(s);
		//DEBUG_MESSAGE("miser integration result:" << res << "," << err);
	} else if (patNBParameters::the()->integrationMethod == "vegas") {
		gsl_monte_vegas_state *s = gsl_monte_vegas_alloc(dimension);

		gsl_monte_vegas_integrate(&G, xl, xu, dimension, 10000, r, s, &res,
				&err);
		//DEBUG_MESSAGE("vegas warm-up" << res << "," << err);

		do {
			gsl_monte_vegas_integrate(&G, xl, xu, dimension, calls / 5, r, s,
					&res, &err);
			//	DEBUG_MESSAGE(
			//		"result" << res << "sigma" << err << ",chisq"
			//			<< gsl_monte_vegas_chisq(s));
		} while (fabs(gsl_monte_vegas_chisq(s) - 1.0) > 0.5);

		gsl_monte_vegas_free(s);

	} else {
		gsl_monte_plain_state *s = gsl_monte_plain_alloc(dimension);
		gsl_monte_plain_integrate(&G, xl, xu, dimension, calls, r, s, &res,
				&err);
		gsl_monte_plain_free(s);
	}
	if (res != res) {
		WARNING("nan integration result");
	}
	delete[] xl;
	xl=NULL;
	delete[] xu;
	xu=NULL;
	m_value = resConst * res;
//DEBUG_MESSAGE("arc tran" << m_value << "=" << resConst << "*" << res);
	gsl_rng_free(r);

	deleteModels();
//DEBUG_MESSAGE("=========Done========");

	return m_value;
}

patMultiModalPath patArcTransition::getPath() {

	return m_path;
}
double patArcTransition::getValue(patString type) {

	if (m_value < 0.0) {
		calProbability(patNBParameters::the()->algoInSelection);
	}
	return m_value;
}

double patArcTransition::getValue(patString type, patMeasurement* prev_measurement, patMeasurement* curr_measurement) {

	if (prev_measurement->isGPS() || curr_measurement->isGPS() || m_prev_measurement->isGPS() || m_curr_measurement->isGPS() || m_value<0.0){
		return getValue(type);
	}
	double prev_original = 0.0;
	switch (m_prev_measurement->getMeasurementType()) {
	case BT:
		//DEBUG_MESSAGE("Dependency not GPS, but BT");
	{
		patBTMeasurementModel prev_m_bt_m(m_prev_measurement,
				m_path.front(), m_path.frontMode());
		prev_original = prev_m_bt_m.integral(1.0);
	}
		break;
	case ACCEL:
		//DEBUG_MESSAGE("Use cache");
{
		patAccelMeasurementModel prev_m_accel_m(m_prev_measurement,
				m_path.front(), m_path.frontMode(), 0.0); 

		prev_original = prev_m_accel_m.integral(1.0);
}		break;
	default:
		WARNING("WRONG MEASUREMENT TYPE");
		return false;
	}

	double curr_original = 0.0;
	switch (m_curr_measurement->getMeasurementType()) {
	case BT:
{		patBTMeasurementModel curr_m_bt_m(m_curr_measurement,
				m_path.back(), m_path.backMode());
		curr_original = curr_m_bt_m.integral(1.0);
		break;
}	case ACCEL:
{		patAccelMeasurementModel curr_m_accel_m(m_curr_measurement,
						m_path.back(), m_path.backMode(),
						m_path.getDistanceToStop()->back());
				curr_original = curr_m_accel_m.integral(1.0);
		}		break;
	default:
		WARNING("WRONG MEASUREMENT TYPE");
		return false;
	}

	double prev_new = 0.0;
	switch (prev_measurement->getMeasurementType()) {
	case BT:
		//DEBUG_MESSAGE("Dependency not GPS, but BT");
		{patBTMeasurementModel prev_m_bt(prev_measurement,
						m_path.front(), m_path.frontMode());
				prev_new = prev_m_bt.integral(1.0);
				}break;
	case ACCEL:
		//DEBUG_MESSAGE("Dependency not GPS, but ACCEL");
{
		patAccelMeasurementModel prev_m_accel(prev_measurement,
				m_path.front(), m_path.frontMode(), 0.0); //FIXME

		prev_new = prev_m_accel.integral(1.0);
		}break;
	default:
		WARNING("WRONG MEASUREMENT TYPE");
		return false;
	}

	double curr_new = 0.0;
	switch (curr_measurement->getMeasurementType()) {
	case BT:
		{patBTMeasurementModel curr_m_bt(curr_measurement,
						m_path.back(), m_path.backMode());
				curr_new = curr_m_bt.integral(1.0);
				}break;
	case ACCEL:
		{patAccelMeasurementModel curr_m_accel(curr_measurement,
						m_path.back(), m_path.backMode(),
						m_path.getDistanceToStop()->back());
				curr_new = curr_m_accel.integral(1.0);
				}break;
	default:
		WARNING("WRONG MEASUREMENT TYPE");
		return false;
	}
	return m_value * curr_new * prev_new / (prev_original * curr_original);
//	return m_value;
}

patMeasurement* patArcTransition::getPrevGpsPoint() {
	return m_prev_measurement;
}
patMeasurement* patArcTransition::getCurrGpsPoint() {
	return m_curr_measurement;
}
const patArc* patArcTransition::getFirstArc() {
	return m_path.front();
}
const patArc* patArcTransition::getLastArc() {
	return m_path.back();
}

