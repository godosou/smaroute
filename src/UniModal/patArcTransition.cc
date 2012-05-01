#include <fstream>

#include <gsl/gsl_integration.h>
#include "patConst.h"

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

//in Utilities
#include "patDisplay.h"
#include "patNBParameters.h"

//in Models
#include "patMeasurementModel.h"
#include "patMeasurementModelV2.h"
#include "patTrafficModelComplex.h"
patArcTransition::patArcTransition(patMultiModalPath aPath,
		patGpsPoint* aGpsPoint, patGpsPoint* bGpsPoint) :
		m_value(-1.0), m_value_raw(-1.0), m_value_simple(-1.0), m_path(aPath), m_prev_gps_point(
				aGpsPoint), m_curr_gps_point(bGpsPoint) {

}

ostream& operator<<(ostream& str, const patArcTransition& x) {
	str << "<" << x.m_prev_gps_point->getTimeStamp() << " - "
			<< x.m_curr_gps_point->getTimeStamp() << "> ";
	str << x.m_path;

	return str;
}

bool operator==(const patArcTransition& aTran, const patArcTransition& bTran) {
	if (aTran.m_prev_gps_point == bTran.m_prev_gps_point
			&& aTran.m_curr_gps_point == bTran.m_curr_gps_point
			&& aTran.m_path == bTran.m_path) {
		return true;
	}

	return false;

}
bool operator<(const patArcTransition& aTran, const patArcTransition& bTran) {

	//compare current time;
	if (aTran.m_curr_gps_point < bTran.m_curr_gps_point) {
		return true;
	}
	if (bTran.m_curr_gps_point > aTran.m_curr_gps_point) {
		return false;
	}

	//compare prev time;
	if (aTran.m_prev_gps_point < bTran.m_prev_gps_point) {
		return true;
	}
	if (bTran.m_prev_gps_point > aTran.m_prev_gps_point) {
		return false;
	}

	//compare path

	return (aTran.m_path < bTran.m_path);
}
/**
 *
 *@param k
 **[0] e_a
 */
double baseFunction_raw(double* k, size_t dim, void* params) {
	struct base_params * fp = (struct base_params *) params;
	/*	{
	 double distance_pow = pow(fp->network.d_curr,2) + \
								pow((k[0]-fp->network.e_d_curr)*fp->network.l_curr,2);
	 double accuracy_pow = pow(fp->gps.std_x_curr,2);

	 return exp(-distance_pow/accuracy_pow);

	 }	*/

	double rtn =
			exp(
					-((pow(fp->network.d_curr, 2)
							+ pow(
									(k[0] - fp->network.e_d_curr)
											* fp->network.l_curr, 2))
							/ pow(fp->gps.std_x_curr, 2)));

	//DEBUG_MESSAGE("delta "<<fp->gps.std_x_curr<<"rtn"<<rtn);
	return rtn;
}/**
 *@param
 *k[0] e_a
 *k[1] e_b
 **/
double baseFunction_simple_diff(double* k, size_t dim, void* params) {
	struct base_params * fp = (struct base_params *) params;

	double d_curr_2 = pow(fp->network.d_curr, 2)
			+ pow((k[0] - fp->network.e_d_curr) * fp->network.l_curr, 2);
	double d_prev_2 = pow(fp->network.d_prev, 2)
			+ pow((k[1] - fp->network.e_d_prev) * fp->network.l_prev, 2);

	return exp(
			-((d_curr_2) / (pow(fp->gps.std_x_curr, 2)))
					- ((d_prev_2) / (pow(fp->gps.std_x_prev, 2))));
}
/**
 *@param
 *k[0] e_a
 *k[1] e_b
 **/
double baseFunction_simple_same(double* k, size_t dim, void* params) {
	/*
	 if (k[1]>k[0]){
	 return 0.0;
	 }
	 */
	struct base_params * fp = (struct base_params *) params;

	double d_curr_2 = pow(fp->network.d_curr, 2)
			+ pow((k[0] - fp->network.e_d_curr) * fp->network.l_curr, 2);
	double d_prev_2 = pow(fp->network.d_prev, 2)
			+ pow((k[1] - fp->network.e_d_prev) * fp->network.l_prev, 2);

	return exp(
			-((d_curr_2) / (pow(fp->gps.std_x_curr, 2)))
					- ((d_prev_2) / (pow(fp->gps.std_x_prev, 2))));
}

void patArcTransition::setNetworkParams(struct network_params& p) {

	//m_path.assignModeForNoInformation();
	p.l_curr = m_path.back()->getLength();
	map<char*, double> distance_to_arc = m_curr_gps_point->distanceTo(
			m_path.back());
	p.d_curr = distance_to_arc["ver"];
	p.e_d_curr = m_curr_gps_point->calPerpendicularFootOnArc(distance_to_arc);

	p.l_prev = m_path.front()->getLength();
	map<char*, double> distance_to_arc_Prev = m_prev_gps_point->distanceTo(
			m_path.front());
	p.d_prev = distance_to_arc_Prev["ver"];
	p.e_d_prev = m_prev_gps_point->calPerpendicularFootOnArc(
			distance_to_arc_Prev);
	p.intermediate_lengths = m_path.getIntermediateUnimodalLengths();
	p.transport_modes = m_path.getUnimodalModes();
	if (m_path.size() == 1) {
		//DEBUG_MESSAGE("only one arc");
		p.l_inter = 0.0;
	} else {
		p.l_inter = m_path.computeLength() - p.l_curr - p.l_prev;
	}
	p.a_total = m_path.size();

}

double patArcTransition::calProbability_raw(struct gps_params GP) {

	//map<char*, double> distanceToArc = m_curr_gps_point->distanceTo( m_path.back());
	//m_value_raw = m_curr_gps_point->measureDDR(distanceToArc);
	m_value_raw =
			(m_value_raw < 0.0) ?
					m_curr_gps_point->getDDR()->getArcDDRValue(m_path.back()) :
					m_value_raw;
	//DEBUG_MESSAGE("raw arc transition value "<<m_value_raw);
	return m_value_raw;
}
list<double> patArcTransition::calProbability(struct gps_params GP) {
	list<double> values;
	values.push_back(
			calProbability(patNBParameters::the()->algoInSelection, GP));
	values.push_back(calProbability("raw", GP));
	values.push_back(calProbability("simple", GP));

	return values;
}
/**
 *@param
 *k[0] e_a
 *k[1] e_b
 **/
double baseFunction_SIM(double* k, size_t dim, void* params) {
	struct base_params * fp = (struct base_params *) params;

	double d_curr_2 = pow(fp->network.d_curr, 2)
			+ pow((k[0] - fp->network.e_d_curr) * fp->network.l_curr, 2);
	double d_prev_2 = pow(fp->network.d_prev, 2)
			+ pow((k[1] - fp->network.e_d_prev) * fp->network.l_prev, 2);

	return exp(
			-((d_curr_2) / (2 * pow(fp->gps.std_x_curr, 2)))
					- ((d_prev_2) / (2 * pow(fp->gps.std_x_prev, 2)))
					- pow(
							(k[0] * fp->network.l_curr
									+ (1 - k[1]) * fp->network.l_prev
									+ fp->network.l_inter) / (fp->gps.time_diff)
									- fp->gps.mu_v_curr, 2)
							/ (2 * pow(fp->gps.std_v_curr, 2)));
}
double baseFunction_Triangular(double* k, size_t dim, void* params) {

	struct base_params * fp = (struct base_params *) params;
	return fp->TM->integral(k[0], k[1]) * fp->MM->integral(k[0], k[1]);
}
double baseFunction_TM(double* k, size_t dim, void* params) {
	struct base_params * fp = (struct base_params *) params;
	//DEBUG_MESSAGE(k[0]<<","<< k[1]);
	return fp->TM->integral(k[0], k[1]) * fp->MM->integral(k[0], k[1]);
}

double baseFunction_MM_2modes(double* k, size_t dim, void* params) {

	struct base_params * fp = (struct base_params *) params;
	return fp->TM->integral(k[0], k[1], k[2]) * fp->MM->integral(k[0], k[1]);
}

double patArcTransition::calProbability(patString algoType,
		struct gps_params GP) {
	if (algoType == "raw") {
		return calProbability_raw(GP);
	}
	double res, err;

	double *xl;
	double *xu;
//	DEBUG_MESSAGE(algoType);

	struct base_params params;
	params.gps = GP;
	double resConst;
	DEBUG_MESSAGE("========Start =========");
	setNetworkParams(params.network);

	/*

	 DEBUG_MESSAGE("gps params"<<"time_diff:"<<GP.time_diff<<"; mu_v_curr:"<<GP.mu_v_curr<<";std_v_curr:"<<GP.std_v_curr<<
	 ";mu_v_prev:"<<GP.mu_v_prev<<";std_v_prev:"<<GP.std_v_prev<<";mu_v_inter:"<<GP.mu_v_inter<<
	 ";std_v_inter:"<<GP.std_v_inter<<";std_x_prev:"<<GP.std_x_prev<<";std_x_curr:"<<GP.std_x_curr<<
	 ";v_denom_prev"<<GP.v_denom_prev<<";v_denom_curr"<<GP.v_denom_curr<<";v_denom_inter"<<GP.v_denom_inter);
	 */
	gsl_monte_function G;

	G.params = &params;
	unsigned long dimension;
	if (algoType == "simple") {
		xl = new double[2];
		xu = new double[2];
		xl[0] = 0.0;
		xu[0] = 1.0; //e_a
		xl[1] = 0.0;
		xu[1] = 1.0; //e_b

		dimension = 2;
		resConst = 1;

		if (m_path.nbrOfArcs() == 1) { //the same arc;

			G.f = &baseFunction_simple_same;

		} else {
			G.f = &baseFunction_simple_diff;

		}

	}

	/*
	 simplified measuremet model with simplied traffic model
	 */
	else if (algoType == "sim2") {
		xl = new double[2];
		xu = new double[2];
		xl[0] = 0.0;
		xu[0] = 1.0; //\ell_k
		xl[1] = 0.0;
		xu[1] = 1.0; // ell_{k-1}
		dimension = 2;
		resConst = params.network.l_curr * params.network.l_prev;

		G.f = &baseFunction_SIM;

	} else if (algoType == "complex" && m_path.isUniModal()) {
		DEBUG_MESSAGE("unimodal transition with arcs:"<<params.network.a_total);
		xl = new double[2];
		xu = new double[2];
		patError* err_pointer;
		struct link_ddr_range curr_ddr_range =
				m_curr_gps_point->getDDR()->getArcDDRRange(getLastArc(),
						err_pointer);
		if (err_pointer != NULL) {

			m_value = 0.0;
			m_value_simple = m_value;

			delete (params.TM);
			delete (params.MM);

			return m_value;
		}
		struct link_ddr_range prev_ddr_range =
				m_prev_gps_point->getDDR()->getArcDDRRange(getFirstArc(),
						err_pointer);
		if (err_pointer != NULL) {

			m_value = 0.0;
			m_value_simple = m_value;

			delete (params.TM);
			delete (params.MM);

			return m_value;
		}
		xl[0] = curr_ddr_range.lower_bound;
		xu[0] = curr_ddr_range.upper_bound; //\ell_k
		xl[1] = prev_ddr_range.lower_bound;
		xu[1] = prev_ddr_range.upper_bound; // ell_{k-1}

		DEBUG_MESSAGE("previous link ddr: "<<xl[0]<<"-"<<xu[0]);
		DEBUG_MESSAGE("current link ddr: "<<xl[1]<<"-"<<xu[1]);
		DEBUG_MESSAGE("total arcs: "<<params.network.a_total);
		DEBUG_MESSAGE("time diff: "<<params.gps.time_diff);
		DEBUG_MESSAGE(
				"distance: "<<params.network.d_curr<<","<<params.network.d_prev);
		DEBUG_MESSAGE(
				"foot: "<<params.network.e_d_curr<<","<<params.network.e_d_prev);
		DEBUG_MESSAGE(
				"length: "<<params.network.l_curr<<","<<params.network.l_inter<<","<<params.network.l_prev);
		dimension = 2;

		params.TM = new patTrafficModelComplex;
		params.TM->setParams(params.gps, params.network);
		params.MM = new patMeasurementModelV2();
		params.MM->setParams(params.gps, params.network);

		resConst = params.TM->getConstant() * params.MM->getConstant();
		DEBUG_MESSAGE(
				"const:"<<resConst<<"="<<params.TM->getConstant() <<"*"<<params.MM->getConstant());
		G.f = &baseFunction_TM;

	} else if (algoType == "complex" && m_path.getChangePoints().size() == 1) {
		DEBUG_MESSAGE("multi-modal change: "<<m_path.getChangePoints().size());
		xl = new double[3];
		xu = new double[3];
		patError* err_pointer;

		struct link_ddr_range curr_ddr_range =
				m_curr_gps_point->getDDR()->getArcDDRRange(getLastArc(),
						err_pointer);
		if (err_pointer != NULL) {

			m_value = 0.0;
			m_value_simple = m_value;

			delete (params.TM);
			delete (params.MM);

			return m_value;
		}
		struct link_ddr_range prev_ddr_range =
				m_prev_gps_point->getDDR()->getArcDDRRange(getFirstArc(),
						err_pointer);
		if (err_pointer != NULL) {

			m_value = 0.0;
			m_value_simple = m_value;

			delete (params.TM);
			delete (params.MM);

			return m_value;
		}

		xl[0] = curr_ddr_range.lower_bound;
		xu[0] = curr_ddr_range.upper_bound; //\ell_k
		xl[1] = prev_ddr_range.lower_bound;
		xu[1] = prev_ddr_range.upper_bound; // ell_{k-1}

		xl[2] = m_prev_gps_point->getTimeStamp();
		xu[2] = m_curr_gps_point->getTimeStamp(); /// t_c

		/*
		 DEBUG_MESSAGE("previous link ddr"<<xl[0]<<"-"<<xu[0]);
		 DEBUG_MESSAGE("current link ddr"<<xl[1]<<"-"<<xu[1]);

		 DEBUG_MESSAGE(params.gps.time_diff);
		 DEBUG_MESSAGE(params.network.d_curr<<","<<params.network.d_prev);
		 DEBUG_MESSAGE(params.network.e_d_curr<<","<<params.network.e_d_prev);
		 DEBUG_MESSAGE(params.network.l_curr<<","<<params.network.l_inter<<","<<params.network.l_prev);
		 */
		dimension = 3;
		params.TM = new patTrafficModelComplex;
		params.TM->setParams(params.gps, params.network);
		params.MM = new patMeasurementModelV2();
		params.MM->setParams(params.gps, params.network);
		resConst = params.TM->getConstant() * params.TM->getConstant()
				* params.MM->getConstant();
		//DEBUG_MESSAGE("const:"<<resConst<<"="<<params.TM->getConstant() <<"*"<<params.MM->getConstant());
		G.f = &baseFunction_MM_2modes;
	}
	else if(m_path.getChangePoints().size() > 1){

		delete (params.TM);
		delete (params.MM);
		return 0.0;
	}
	else{
		DEBUG_MESSAGE("unknown transition");
		return 0.0;
	}

	for (unsigned long i = 0; i < dimension; ++i) {
		if (xl[i] >= xu[i]) {
			DEBUG_MESSAGE(
					"algorithm:" << algoType << "invalid bound " << m_path.nbrOfArcs() << " " << i << ":" << xl[i] << "," << xu[i]);
		}
	}
	G.dim = dimension;
	//G[2] = dimension;

	const gsl_rng_type *T;
	gsl_rng *r;

	size_t calls = patNBParameters::the()->nbrOfIntegrationCalls;

	gsl_rng_env_setup();
	//DEBUG_MESSAGE("setup o k");

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
	m_value = resConst * res;
	m_value_simple = m_value;
	DEBUG_MESSAGE("arc tran"<<m_value<<"="<<resConst<<"*"<<res);
	gsl_rng_free(r);

	delete (params.TM);
	delete (params.MM);
	DEBUG_MESSAGE("=========Done========");

	return m_value;
}

patMultiModalPath patArcTransition::getPath() {

	return m_path;
}
double patArcTransition::getValue(patString type, struct gps_params GP) {
	if (type == "raw") {
		if (m_value_raw < 0.0) {
			calProbability_raw(GP);
		}
		return m_value_raw;
	} else if (type == "simple") {
		if (m_value_simple < 0.0) {
			calProbability("simple", GP);
		}
		return m_value_simple;
	}
	if (m_value < 0.0) {
		calProbability(patNBParameters::the()->algoInSelection, GP);
	}
	return m_value;
}

patGpsPoint* patArcTransition::getPrevGpsPoint() {
	return m_prev_gps_point;
}
patGpsPoint* patArcTransition::getCurrGpsPoint() {
	return m_curr_gps_point;
}
const patArc* patArcTransition::getFirstArc() {
	return m_path.front();
}
const patArc* patArcTransition::getLastArc() {
	return m_path.back();
}
