#include <fstream>
#include "patDisplay.h" 
#include <gsl/gsl_integration.h>
#include "patConst.h"

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_monte.h>
#include <gsl/gsl_monte_plain.h>
#include <gsl/gsl_monte_miser.h>
#include <gsl/gsl_monte_vegas.h>

#include "patArcTransition.h"
#include "patErrMiscError.h"
#include "patError.h"
#include "patString.h"
#include "patErrNullPointer.h"

#include "patNBParameters.h"
#include "patMeasurementModel.h"
#include "patMeasurementModelV1.h"
#include "patMeasurementModelV2.h"
#include "patTrafficModel.h"
#include "patTrafficModelTriangular.h"
#include "patTrafficModelComplex.h"
patArcTransition::patArcTransition(patPathJ aPath, patGpsPoint* aGpsPoint,
		patGpsPoint* bGpsPoint) :
		value(-1.0), valueRaw(-1.0), valueSimple(-1.0), path(aPath), prevGpsPoint(
				aGpsPoint), currGpsPoint(bGpsPoint) {
			path.detChangePoints();

}

patArcTransition::patArcTransition(list<patArc*> listOfArcs,
		patGpsPoint* aGpsPoint, patGpsPoint* bGpsPoint) :
		value(-1.0), valueRaw(-1.0), valueSimple(-1.0), path(listOfArcs), prevGpsPoint(
				aGpsPoint), currGpsPoint(bGpsPoint) {

	path = patPathJ(listOfArcs);
			path.detChangePoints();

}

patArcTransition::patArcTransition(
		list<pair<patArc*, TransportMode> > listOfArcs,
		patGpsPoint* curr_GpsPoint,
		patGpsPoint* prev_GpsPoint) :
		value(-1.0), valueRaw(-1.0), valueSimple(-1.0), path(listOfArcs), prevGpsPoint(
				curr_GpsPoint), currGpsPoint(prev_GpsPoint) {

	path = patPathJ(listOfArcs);
			path.detChangePoints();

}
ostream& operator<<(ostream& str, const patArcTransition& x) {
	str << "<" << x.prevGpsPoint->getTimeStamp() << " - "
			<< x.currGpsPoint->getTimeStamp() << "> ";
	str << x.path;

	return str;
}

bool operator==(const patArcTransition& aTran, const patArcTransition& bTran) {
	if (aTran.prevGpsPoint == bTran.prevGpsPoint
			&& aTran.currGpsPoint == bTran.currGpsPoint
			&& aTran.path == bTran.path) {
		return true;
	}

	return false;

}
bool operator<(const patArcTransition& aTran, const patArcTransition& bTran) {

	//compare current time;
	if (aTran.currGpsPoint < bTran.currGpsPoint) {
		return true;
	}
	if (bTran.currGpsPoint > aTran.currGpsPoint) {
		return false;
	}

	//compare prev time;
	if (aTran.prevGpsPoint < bTran.prevGpsPoint) {
		return true;
	}
	if (bTran.prevGpsPoint > aTran.prevGpsPoint) {
		return false;
	}

	//compare path

	return (aTran.path < bTran.path);
}
/**
 *
 *@param k
 **[0] e_a
 */
double baseFunction_raw(double* k, size_t dim, void* params) {
	struct base_params * fp = (struct base_params *) params;
	/*	{
	 patReal distance_pow = pow(fp->network.d_curr,2) + \
								pow((k[0]-fp->network.e_d_curr)*fp->network.l_curr,2);
	 patReal accuracy_pow = pow(fp->gps.std_x_curr,2);

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

void patArcTransition::setNetworkParams(patNetwork* theNetwork,
		struct network_params& p) {

	path.assignModeForNoInformation();
	p.l_curr = path.back()->getLength();
	map<char*, double> distanceToArc = currGpsPoint->distanceTo(theNetwork,
			path.back());
	p.d_curr = distanceToArc["ver"];
	p.e_d_curr = currGpsPoint->calPerpendicularFootOnArc(distanceToArc);

	p.l_prev = path.front()->getLength();
	map<char*, double> distanceToArc_Prev = prevGpsPoint->distanceTo(
			theNetwork, path.front());
	p.d_prev = distanceToArc_Prev["ver"];
	p.e_d_prev = prevGpsPoint->calPerpendicularFootOnArc(distanceToArc_Prev);
	p.intermediate_lengths = path.getIntermediateUnimodalLengths();
	p.transport_modes = path.getUnimodalModes();
	if (path.getArcListPointer()->size() == 1) {
		//DEBUG_MESSAGE("only one arc");
		p.l_inter = 0.0;
	} else {
		p.l_inter = path.computeLength() - p.l_curr - p.l_prev;
	}
	p.a_total = path.()->size();

//	DEBUG_MESSAGE(p->l_prev<<","<<p->l_inter<<","<<p->l_curr);
	//p->t_w = dealIntersections(theNetwork);

}

double patArcTransition::calProbability_raw(patNetwork* theNetwork,
		struct gps_params GP) {

	//map<char*, patReal> distanceToArc = currGpsPoint->distanceTo(theNetwork, path.back());
	//valueRaw = currGpsPoint->measureDDR(distanceToArc);
	valueRaw =
			(valueRaw < 0.0) ?
					currGpsPoint->getArcDDRValue(path.back(), theNetwork) :
					valueRaw;
	//DEBUG_MESSAGE("raw arc transition value "<<valueRaw);
	return valueRaw;
	}
list<double> patArcTransition::calProbability(patNetwork* theNetwork,
		struct gps_params GP) {
	list<double> values;
	values.push_back(
			calProbability(patNBParameters::the()->algoInSelection, theNetwork,
					GP));
	values.push_back(calProbability("raw", theNetwork, GP));
	values.push_back(calProbability("simple", theNetwork, GP));

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

double patArcTransition::calMultiModalProbability(patNetwork* theNetwork,
		struct gps_params GP) {
	if (path.isUniModal()) {
		return 0.0;
	}

	else if (path.getNbrOfChangePoints() == 1) {
		return 0.0;
	} else if (path.getNbrOfChangePoints() > 1) {
		return 0.0;
	}

}
double patArcTransition::calProbability(patString algoType,
		patNetwork* theNetwork, struct gps_params GP) {
	if (algoType == "raw") {
		return calProbability_raw(theNetwork, GP);
	}
	double res, err;

	double *xl;
	double *xu;
//	DEBUG_MESSAGE(algoType);

	struct base_params params;
	params.gps = GP;
	double resConst;
	DEBUG_MESSAGE("========Start =========");
	setNetworkParams(theNetwork, params.network);
	
/*
	
	DEBUG_MESSAGE("gps params"<<"time_diff:"<<GP.time_diff<<"; mu_v_curr:"<<GP.mu_v_curr<<";std_v_curr:"<<GP.std_v_curr<<
	 ";mu_v_prev:"<<GP.mu_v_prev<<";std_v_prev:"<<GP.std_v_prev<<";mu_v_inter:"<<GP.mu_v_inter<<
	 ";std_v_inter:"<<GP.std_v_inter<<";std_x_prev:"<<GP.std_x_prev<<";std_x_curr:"<<GP.std_x_curr<<
	 ";v_denom_prev"<<GP.v_denom_prev<<";v_denom_curr"<<GP.v_denom_curr<<";v_denom_inter"<<GP.v_denom_inter);
*/	
	gsl_monte_function G;

	G.params = &params;
	unsigned long dimension;
	path.detChangePoints();
	if (algoType == "simple") {
		xl = new double[2];
		xu = new double[2];
		xl[0] = 0.0;
		xu[0] = 1.0; //e_a
		xl[1] = 0.0;
		xu[1] = 1.0; //e_b

		dimension = 2;
		resConst = 1;

		if (path.nbrOfArcs() == 1) { //the same arc;

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

	} else if (algoType == "complex" && path.isUniModal()) {
		DEBUG_MESSAGE("unimodal transition with arcs:"<<params.network.a_total);
		xl = new double[2];
		xu = new double[2];
		struct link_ddr_range curr_ddr_range = currGpsPoint->getLinkDDRRange(
				getLastArc());
		struct link_ddr_range prev_ddr_range = prevGpsPoint->getLinkDDRRange(
				getFirstArc());

		xl[0] = curr_ddr_range.lower_bound;
		xu[0] = curr_ddr_range.upper_bound; //\ell_k
		xl[1] = prev_ddr_range.lower_bound;
		xu[1] = prev_ddr_range.upper_bound; // ell_{k-1}

		
		 DEBUG_MESSAGE("previous link ddr: "<<xl[0]<<"-"<<xu[0]);
		 DEBUG_MESSAGE("current link ddr: "<<xl[1]<<"-"<<xu[1]);
		DEBUG_MESSAGE("total arcs: "<<params.network.a_total);
		 DEBUG_MESSAGE("time diff: "<<params.gps.time_diff);
		 DEBUG_MESSAGE("distance: "<<params.network.d_curr<<","<<params.network.d_prev);
		 DEBUG_MESSAGE("foot: "<<params.network.e_d_curr<<","<<params.network.e_d_prev);
		 DEBUG_MESSAGE("length: "<<params.network.l_curr<<","<<params.network.l_inter<<","<<params.network.l_prev);
		dimension = 2;

		params.TM = new patTrafficModelComplex;
		params.TM->setParams(params.gps, params.network);
		params.MM = new patMeasurementModelV2();
		params.MM->setParams(params.gps, params.network);

		resConst = params.TM->getConstant() * params.MM->getConstant();
		DEBUG_MESSAGE("const:"<<resConst<<"="<<params.TM->getConstant() <<"*"<<params.MM->getConstant());
		G.f = &baseFunction_TM;

	} else if (algoType == "complex" && path.getNbrOfChangePoints() == 1) {
		DEBUG_MESSAGE("multi-modal change: "<<path.getNbrOfChangePoints());
		xl = new double[3];
		xu = new double[3];
		struct link_ddr_range curr_ddr_range = currGpsPoint->getLinkDDRRange(
				getLastArc());
		struct link_ddr_range prev_ddr_range = prevGpsPoint->getLinkDDRRange(
				getFirstArc());

		xl[0] = curr_ddr_range.lower_bound;
		xu[0] = curr_ddr_range.upper_bound; //\ell_k
		xl[1] = prev_ddr_range.lower_bound;
		xu[1] = prev_ddr_range.upper_bound; // ell_{k-1}

		xl[2] = prevGpsPoint->getTimeStamp();
		xu[2] = currGpsPoint->getTimeStamp(); /// t_c

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
		resConst =  params.TM->getConstant() * params.TM->getConstant() * params.MM->getConstant();
		//DEBUG_MESSAGE("const:"<<resConst<<"="<<params.TM->getConstant() <<"*"<<params.MM->getConstant());
		G.f = &baseFunction_MM_2modes;
	}

	for (unsigned long i = 0; i < dimension; ++i) {
		if (xl[i] >= xu[i]) {
			DEBUG_MESSAGE(
					"algorithm:" << algoType << "invalid bound "
							<< path.nbrOfArcs() << " " << i << ":" << xl[i]
							<< "," << xu[i]);
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
	if (patNBParameters::the()->integrationMethod == "miser")
	{
		gsl_monte_miser_state *s = gsl_monte_miser_alloc(dimension);
		gsl_monte_miser_integrate(&G, xl, xu, dimension, calls, r, s, &res,
				&err);
		gsl_monte_miser_free(s);
		//DEBUG_MESSAGE("miser integration result:" << res << "," << err);
	}
	else if (patNBParameters::the()->integrationMethod == "vegas") {
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
	if(res!=res){
		WARNING("nan integration result");
	}
	value = resConst * res;
	valueSimple = value;
	DEBUG_MESSAGE("arc tran"<<value<<"="<<resConst<<"*"<<res);
	gsl_rng_free(r);

	delete (params.TM);
	delete (params.MM);
	DEBUG_MESSAGE("=========Done========");

	return value;
}

vector<double> patArcTransition::dealIntersections(patNetwork* theNetwork) {
	vector<double> waitings;
	list<patArc*>* listOfArcs = path.getArcListPointer();
	if (listOfArcs->size() < 2) {
		return waitings;
	}
	patArc* prevArc = listOfArcs->front();
	list<patArc*>::iterator arcIter = listOfArcs->begin();
	arcIter++;
	patError* err;
	for (; arcIter != listOfArcs->end(); arcIter++) {
		double t_w = calWaitingTime(prevArc, const_cast<patArc*>(*arcIter),
				theNetwork, err);
		if (t_w >= 0.0) {
			waitings.push_back(t_w);
		}
		prevArc = const_cast<patArc*>(*arcIter);
	}

	return waitings;
}

double patArcTransition::calWaitingTime(patArc* upArc, patArc* downArc,
		patNetwork* theNetwork, patError*& err) {

	if (upArc->m_down_node_id != downArc->m_up_node_id) {
		stringstream str;
		str << (*upArc) << " and " << (*downArc) << " are not connected";
		err = new patErrMiscError(str.str());
		WARNING(err->describe());

	}
	patNode* intersection = theNetwork->getNodeFromUserId(upArc->m_down_node_id);

	if (intersection == NULL) {
		err = new patErrNullPointer("patNode");
		WARNING(err->describe());
		return -1.0;
	}
	//DEBUG_MESSAGE("node type"<<intersection->attributes.type);
	if (intersection->attributes.type == "traffic_signals") {
//DEBUG_MESSAGE("a t raffic signals at"<<*intersection);
		return calSignalWaiting(upArc, downArc, intersection, theNetwork);
	} else if (upArc->m_attributes.priority != downArc->m_attributes.priority) {
		return patNBParameters::the()->stopPenalty;
	}

	return -1.0;
}

double patArcTransition::calSignalWaiting(patArc* upArc, patArc* downArc,
		patNode* intersection, patNetwork* theNetwork) {
	set<unsigned long>* successors = &(intersection->userSuccessors);
	map<patArc*, double> downHeading;
	unsigned long left = 0;
	unsigned long right = 0;
	double headingChange = 180.0 - (downArc->m_attributes.heading)
			- upArc->m_attributes.heading;
	headingChange =
			(headingChange > 360.0) ? (headingChange - 360.0) : headingChange;
	headingChange =
			(headingChange < 0.0) ? (headingChange + 360.0) : headingChange;

	for (set<unsigned long>::iterator iter1 = successors->begin();
			iter1 != successors->end(); ++iter1) {

		patArc* downStream = theNetwork->getArcFromNodesUserId(
				intersection->userId, *iter1);
		if (downStream == NULL)
		{
			continue;
		}
		double h1 = 180.0
				- (downStream->m_attributes.heading - upArc->m_attributes.heading);
		h1 = (h1 > 360.0) ? (h1 - 360.0) : h1;
		h1 = (h1 < 0.0) ? (h1 + 360.0) : h1;

		if (fabs(h1) < 10.0) {
			continue;
		}

		if (h1 < headingChange) {
			right += 1;
		}
		if (h1 > headingChange) {
			left += 1;
		}
	}

	if (right == 0) {
		return patNBParameters::the()->rightTurnPenalty;
	}
	if (left == 0 && right == 1) {
		return patNBParameters::the()->goStraightPenalty;
	}
	if (left == 0 && right > 1) {
		return patNBParameters::the()->rightTurnPenalty;
	}
	if (left > 0 && right > 0) {
		return patNBParameters::the()->goStraightPenalty;
	}
}

patPathJ patArcTransition::getPath() {

	return path;
}
double patArcTransition::getValue(patString type, patNetwork* theNetwork,
		struct gps_params GP) {
	if (type == "raw") {
		if (valueRaw < 0.0) {
			calProbability_raw(theNetwork, GP);
		}
		return valueRaw;
	} else if (type == "simple") {
		if (valueSimple < 0.0) {
			calProbability("simple", theNetwork, GP);
		}
		return valueSimple;
	}
	if (value < 0.0) {
		calProbability(patNBParameters::the()->algoInSelection, theNetwork, GP);
	}
	return value;
}

patGpsPoint* patArcTransition::getPrevGpsPoint() {
	return prevGpsPoint;
}
patGpsPoint* patArcTransition::getCurrGpsPoint() {
	return currGpsPoint;
}
patArc* patArcTransition::getFirstArc() {
	return path.front();
}
patArc* patArcTransition::getLastArc() {
	return path.back();
}
