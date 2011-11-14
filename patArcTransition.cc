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
patArcTransition::patArcTransition(patPathJ aPath,
				patGpsPoint* aGpsPoint,
				patGpsPoint* bGpsPoint):
		value(-1.0),
		valueRaw(-1.0),
		valueSimple(-1.0),
		path(aPath),
		prevGpsPoint(aGpsPoint),
		currGpsPoint(bGpsPoint)
	{
	
	
}

patArcTransition::patArcTransition(list<patArc*> listOfArcs,
				patGpsPoint* aGpsPoint,
				patGpsPoint* bGpsPoint):
		value(-1.0),
		valueRaw(-1.0),
		valueSimple(-1.0),
		path(listOfArcs),
		prevGpsPoint(aGpsPoint),
		currGpsPoint(bGpsPoint)
	{
	
	path = patPathJ(listOfArcs);
}

ostream& operator<<(ostream& str, const patArcTransition& x)  {
	str<<"<"<<x.prevGpsPoint->getTimeStamp()<<" - "<<x.currGpsPoint->getTimeStamp()<<"> ";
	str<<x.path;
	
	return str;
}


bool operator==(const patArcTransition& aTran, const patArcTransition& bTran) {
	if(	aTran.prevGpsPoint == bTran.prevGpsPoint &&
		aTran.currGpsPoint == bTran.currGpsPoint &&
		aTran.path == bTran.path
	){
		return true;
	}
	
	return false;

}
bool operator<(const patArcTransition& aTran, const patArcTransition& bTran) {
	
	//compare current time;
	if (aTran.currGpsPoint < bTran.currGpsPoint){
		return true;
	}
	if (bTran.currGpsPoint > aTran.currGpsPoint){
		return false;
	}
	
	//compare prev time;
	if (aTran.prevGpsPoint < bTran.prevGpsPoint){
		return true;
	}
	if (bTran.prevGpsPoint > aTran.prevGpsPoint){
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
patReal baseFunction_raw(patReal* k, size_t dim, void* params){
	struct base_params * fp = (struct base_params *)params;
/*	{
		patReal distance_pow = pow(fp->network.d_curr,2) + \
								pow((k[0]-fp->network.e_d_curr)*fp->network.l_curr,2);
	patReal accuracy_pow = pow(fp->gps.std_x_curr,2);
		
		return exp(-distance_pow/accuracy_pow);
	
	}	*/
	
	patReal rtn = exp(	-((pow(fp->network.d_curr,2)+ \
				pow((k[0] - fp->network.e_d_curr) * fp->network.l_curr,2))/ \
				pow(fp->gps.std_x_curr,2)));
				
				//DEBUG_MESSAGE("delta "<<fp->gps.std_x_curr<<"rtn"<<rtn);
				return rtn;
}
/**
*
*@param k
**[0] e_a
*/
patReal baseFunction_raw_1(patReal k , void* params){
	struct base_params * fp = (struct base_params *)params;
/*	{
		patReal distance_pow = pow(fp->network.d_curr,2) + \
								pow((k[0]-fp->network.e_d_curr)*fp->network.l_curr,2);
	patReal accuracy_pow = pow(fp->gps.std_x_curr,2);
		
		return exp(-distance_pow/accuracy_pow);
	
	}	*/
	
	patReal rtn = exp(	-((pow(fp->network.d_curr,2)+ \
				pow((k - fp->network.e_d_curr) * fp->network.l_curr,2))/ \
				pow(fp->gps.std_x_curr,2)));
				
				//DEBUG_MESSAGE("delta "<<fp->gps.std_x_curr<<"rtn"<<rtn);
				return rtn;
}
/**
*
*@param k
**[0] e_a
**[1] t_a
**[2] e_b
*/
patReal baseFunction_arc_arc_same(patReal* k, size_t dim, void* params){
	if(k[2]-k[0] >= 0.0){
		//DEBUG_MESSAGE("0: "<<k[0]<<","<<k[2]<<","<<k[1]);
		return 0.0;
	}
	struct base_params * fp = (struct base_params *)params;
	
	patReal v_b  = (k[2] * fp->network.l_curr)/( - fp->gps.time_diff + k[0] * k[1]);
	

	patReal v_a = fp->network.l_curr / k[1];
	

	if(v_b <0 || v_a <0){
				//DEBUG_MESSAGE("v_b,v_a:"<<","<<v_b<<","<<v_a);

		return 0.0;
	}
	//DEBUG_MESSAGE("v_b,v_a:"<<","<<v_b<<","<<v_a<<","<<fp->gps.mu_v_prev<<","<<fp->gps.mu_v_curr);
	//patReal deltaConst =  1/(2 * M_PI * params.gps.std_v_prev  * params.gps.std_v_curr);
	//patReal d_curr_2 = 	pow(fp->network.d_curr,2)+pow((k[0] - fp->network.e_d_curr) * fp->network.l_curr,2);	
	//patReal d_prev_2 = pow(fp->network.d_prev,2)+pow((k[2] - fp->network.e_d_prev) * fp->network.l_prev,2);
	//patReal pdf = 1.0 /(pow(M_PI * fp->gps.std_x_curr * fp->gps.std_x_prev,2)); //biviariate normal
	/*return pdf * exp(	-((d_curr_2)/( 2.0 * pow(fp->gps.std_x_curr,2))) \
				-pow((v_b - fp->gps.mu_v_prev)/fp->gps.std_v_prev,2)/2 \
				-((d_prev_2)/(2.0 * pow(fp->gps.std_x_prev,2))) \
				-pow((v_a - fp->gps.mu_v_curr)/fp->gps.std_v_curr,2)/2 );
	*/
	//patReal rtn =  exp(	-((d_curr_2)/(  pow(fp->gps.std_x_curr,2))) \
	//			-pow((v_b - fp->gps.mu_v_prev)/fp->gps.std_v_prev,2)/2 \
	//			-((d_prev_2)/( pow(fp->gps.std_x_prev,2))) \
	//			-pow((v_a - fp->gps.mu_v_curr)/fp->gps.std_v_curr,2)/2 );
	//DEBUG_MESSAGE(rtn);
	patReal rtn = fp->MM->integral(k[0],k[2])*exp(-pow((v_b - fp->gps.mu_v_prev)/fp->gps.std_v_prev,2)/2-pow((v_a - fp->gps.mu_v_curr)/fp->gps.std_v_curr,2)/2);
	return rtn;
	/*
	return exp(	-((d_curr_2)/(  pow(fp->gps.std_x_curr,2))) \
				-pow((v_b - fp->gps.mu_v_prev)/fp->gps.std_v_prev,2)/2 \
				-((d_prev_2)/( pow(fp->gps.std_x_prev,2))) \
				-pow((v_a - fp->gps.mu_v_curr)/fp->gps.std_v_curr,2)/2 );//biviariate normal
	*/	
}
/**
*
*@param k
**[0] e_a
**[1] t_a
**[2] e_b
*/
patReal baseFunction_arc_arc_adj_no_stop(patReal* k, size_t dim, void* params){
	struct base_params * fp = (struct base_params *)params;
	
	patReal v_b  = (1-k[2]) * fp->network.l_prev / \
		(fp->gps.time_diff -k[0] * k[1]);

	patReal v_a = fp->network.l_curr / k[1];
	
	if(v_b <0 || v_a <0){
			//	DEBUG_MESSAGE("v_b,v_a:"<<fp->b_param.l_c<<","<<v_b<<","<<v_a);

		return 0.0;
	}
	patReal rtn = fp->MM->integral(k[0],k[2])*exp(-pow((v_b - fp->gps.mu_v_prev)/fp->gps.std_v_prev,2)/2-pow((v_a - fp->gps.mu_v_curr)/fp->gps.std_v_curr,2)/2);
	
	return rtn;
	//return  exp(	-(( d_curr_2)/ (pow(fp->gps.std_x_curr,2))) \
	//			-pow((v_b - fp->gps.mu_v_prev)/fp->gps.std_v_prev,2)/2 \
	//			-((d_prev_2)/( pow(fp->gps.std_x_prev,2))) \
	//			-pow((v_a - fp->gps.mu_v_curr)/fp->gps.std_v_curr,2)/2 );//cdf rayleigh
				
}


/**
*
*@param k
**[0] e_a
**[1] t_a
**[2] e_b
**[3] t_w
*/
patReal baseFunction_arc_arc_adj(patReal* k, size_t dim, void* params){
	struct base_params * fp = (struct base_params *)params;
	
	patReal v_b  = (1-k[2]) * fp->network.l_prev / \
		(fp->gps.time_diff -k[0] * k[1] - fp->network.t_w[0]);

	patReal v_a = fp->network.l_curr / k[1];
	
	if(v_b <0 || v_a <0){
			//	DEBUG_MESSAGE("v_b,v_a:"<<fp->b_param.l_c<<","<<v_b<<","<<v_a);

		return 0.0;
	}
	//////patReal deltaConst =  1/(2 * M_PI * params.gps.std_v_prev  * params.gps.std_v_curr);
	patReal d_curr_2 = 	pow(fp->network.d_curr,2)+pow((k[0] - fp->network.e_d_curr) * fp->network.l_curr,2);	
	patReal d_prev_2 = pow(fp->network.d_prev,2)+pow((k[2] - fp->network.e_d_prev) * fp->network.l_prev,2);
	/* biviariate
	patReal pdf = 1.0 /(pow(M_PI * fp->gps.std_x_curr * fp->gps.std_x_prev,2));
		
	return pdf * exp(	-((d_curr_2 )/ (2.0 * pow(fp->gps.std_x_curr,2))) \
				-pow((v_b - fp->gps.mu_v_prev)/fp->gps.std_v_prev,2)/2 \
				-((d_prev_2)/(2.0 * pow(fp->gps.std_x_prev,2))) \
				-pow((v_a - fp->gps.mu_v_curr)/fp->gps.std_v_curr,2)/2 )
				* (1/fp->network.t_w[0]);//probability density function for t_w;
	*/
	patReal rtn = fp->MM->integral(k[0],k[2])*\
		exp(-pow((v_b - fp->gps.mu_v_prev)/fp->gps.std_v_prev,2)/2\
			-pow((v_a - fp->gps.mu_v_curr)/fp->gps.std_v_curr,2)/2)\
	* (1/fp->network.t_w[0]);
	return rtn;
	//return  exp(	-((d_curr_2 )/ (pow(fp->gps.std_x_curr,2))) \
	//			-pow((v_b - fp->gps.mu_v_prev)/fp->gps.std_v_prev,2)/2 \
	//			-((d_prev_2)/(pow(fp->gps.std_x_prev,2))) \
	//			-pow((v_a - fp->gps.mu_v_curr)/fp->gps.std_v_curr,2)/2 )
	//			* (1/fp->network.t_w[0]);//probability density function for t_w;

	}

/**
*
*@param k
**[0] e_a
**[1] t_a
**[2] e_b
**[3] v_c
**[4] t_w
**/
patReal baseFunction_arc_arc_inter(patReal* k, size_t dim, void* params){
	struct base_params * fp = (struct base_params *)params;
	
	patReal t_w = 0.0;
	patReal t_w_p = 1.0;
	for(patULong i = 4; i< dim; ++i){
		t_w += k[i];
		t_w_p/=fp->network.t_w[i-4];
	}
	
	patReal v_b = (1-k[2]) * fp->network.l_prev / \
	(fp->gps.time_diff -k[0] * k[1] - t_w  - (fp->network.l_inter)/k[3] );
	patReal v_a = fp->network.l_curr / k[1];
	if(v_b <0 || v_a <0){
		return 0.0;
	}
	//probaConst = 1/(sqrt(pow(2 * M_PI,3)) * params.gps.std_v_prev  * params.gps.std_v_inter * params.gps.std_v_curr);
	
	patReal d_curr_2 = 	pow(fp->network.d_curr,2)+pow((k[0] - fp->network.e_d_curr) * fp->network.l_curr,2);	
	patReal d_prev_2 = pow(fp->network.d_prev,2)+pow((k[2] - fp->network.e_d_prev) * fp->network.l_prev,2);
	
	/*bivariate normal
	patReal pdf = 1.0 /(pow(M_PI * fp->gps.std_x_curr * fp->gps.std_x_prev,2));
	
	//patReal deltaConst  = 2 * 2 /(pow(fp->gps.std_x_curr,2) * pow(fp->gps.std_x_prev,2));
	return  pdf * t_w_p * exp(-((d_curr_2)/(2.0 *pow(fp->gps.std_x_curr,2))) \
				-pow((v_b - fp->gps.mu_v_prev)/fp->gps.std_v_prev,2)/2 \
				-pow((k[3] - fp->gps.mu_v_inter)/fp->gps.std_v_inter,2)/2 \
				-((d_prev_2)/(2.0 * pow(fp->gps.std_x_prev,2))) \
				-pow((v_a - fp->gps.mu_v_curr)/fp->gps.std_v_curr,2)/2 );//probability density function for t_w;
	*/
	patReal rtn = fp->MM->integral(k[0],k[2])*\
	exp(-pow((v_b - fp->gps.mu_v_prev)/fp->gps.std_v_prev,2)/2\
		-pow((v_a - fp->gps.mu_v_curr)/fp->gps.std_v_curr,2)/2
		-pow((k[3] - fp->gps.mu_v_inter)/fp->gps.std_v_inter,2)/2 );
	return rtn;
//	return  t_w_p * exp(-((d_curr_2)/(pow(fp->gps.std_x_curr,2))) \
//				-pow((v_b - fp->gps.mu_v_prev)/fp->gps.std_v_prev,2)/2 \
//				-pow((k[3] - fp->gps.mu_v_inter)/fp->gps.std_v_inter,2)/2 \
//				-((d_prev_2)/(pow(fp->gps.std_x_prev,2))) \
//				-pow((v_a - fp->gps.mu_v_curr)/fp->gps.std_v_curr,2)/2 );//probability density function for t_w;
	
}
/**
*@param
*k[0] e_a
*k[1] e_b
**/
patReal baseFunction_simple_diff(patReal* k, size_t dim, void* params){
	struct base_params * fp = (struct base_params *)params;
	
	patReal d_curr_2 = 	pow(fp->network.d_curr,2)+pow((k[0] - fp->network.e_d_curr) * fp->network.l_curr,2);	
	patReal d_prev_2 = pow(fp->network.d_prev,2)+pow((k[1] - fp->network.e_d_prev) * fp->network.l_prev,2);
	
	return  exp(	-((d_curr_2 )/ (pow(fp->gps.std_x_curr,2))) \
				-((d_prev_2)/(pow(fp->gps.std_x_prev,2))) );
}
/**
*@param
*k[0] e_a
*k[1] e_b
**/
patReal baseFunction_simple_same(patReal* k, size_t dim, void* params){
	/*
	if (k[1]>k[0]){
		return 0.0;
	}
	*/
	struct base_params * fp = (struct base_params *)params;
	
	patReal d_curr_2 = 	pow(fp->network.d_curr,2)+pow((k[0] - fp->network.e_d_curr) * fp->network.l_curr,2);	
	patReal d_prev_2 = pow(fp->network.d_prev,2)+pow((k[1] - fp->network.e_d_prev) * fp->network.l_prev,2);
	
	return  exp(	-((d_curr_2 )/ (pow(fp->gps.std_x_curr,2))) \
				-((d_prev_2)/(pow(fp->gps.std_x_prev,2))) );
}

void patArcTransition::setNetworkParams(patNetwork* theNetwork, 
			struct network_params * p){
	
	
		p->l_curr = path.back()->getLength();
		map<char*, patReal> distanceToArc = currGpsPoint->distanceTo(theNetwork, path.back());
		p->d_curr = distanceToArc["ver"];
		p->e_d_curr = currGpsPoint->calPerpendicularFootOnArc(distanceToArc);
	
		p->l_prev = path.front()->getLength();
		map<char*, patReal> distanceToArc_Prev = prevGpsPoint->distanceTo(theNetwork, path.front());
		p->d_prev = distanceToArc_Prev["ver"];
		p->e_d_prev = prevGpsPoint->calPerpendicularFootOnArc(distanceToArc_Prev);
		
	if (path.getArcList()->size()==1){
		
		p->l_inter=0;
	}
	else{
		p->l_inter = path.computePathLength()- p->l_curr - p->l_prev;
	}
	p->a_total=path.getArcList()->size();
//		DEBUG_MESSAGE(p->l_prev<<","<<p->l_inter<<","<<p->l_curr);
	p->t_w = dealIntersections(theNetwork);
	
}


patReal patArcTransition::calProbability_raw(patNetwork* theNetwork, struct gps_params GP){

	//map<char*, patReal> distanceToArc = currGpsPoint->distanceTo(theNetwork, path.back());
	//valueRaw = currGpsPoint->measureDDR(distanceToArc);
	valueRaw = (valueRaw<0.0)?currGpsPoint->getArcDDRValue(path.back(),theNetwork):valueRaw;
	//DEBUG_MESSAGE("raw arc transition value "<<valueRaw);
	return valueRaw; 
	/*
	map<char*, patReal> distanceToArc = currGpsPoint->distanceTo(theNetwork, path.back());
	DEBUG_MESSAGE("another integration result:"<<currGpsPoint->measureDDR(distanceToArc));
	
	 struct base_params params;
	params.gps = GP;
	patReal resConst;
	setNetworkParams(theNetwork,&(params.network));
	gsl_monte_function G ;
	
	G.params = &params;
	
	       gsl_integration_workspace * w 
         = gsl_integration_workspace_alloc (1000);
       
       patReal result, error;
       gsl_function F;
       F.function = &baseFunction_raw_1;
       F.params = &params;
     
       gsl_integration_qags (&F, 0, 1, 0, 1e-7, 1000,
                             w, &result, &error); 
     
       gsl_integration_workspace_free (w);
     DEBUG_MESSAGE("result"<<result);
	DEBUG_MESSAGE("gps params"<<"time_diff:"<<GP.time_diff<<"; mu_v_curr:"<<GP.mu_v_curr<<";std_v_curr:"<<GP.std_v_curr<<
	";mu_v_prev:"<<GP.mu_v_prev<<";std_v_prev:"<<GP.std_v_prev<<";mu_v_inter:"<<GP.mu_v_inter<<
	";std_v_inter:"<<GP.std_v_inter<<";std_x_prev:"<<GP.std_x_prev<<";std_x_curr:"<<GP.std_x_curr<<
	";v_denom_prev"<<GP.v_denom_prev<<";v_denom_curr"<<GP.v_denom_curr<<";v_denom_inter"<<GP.v_denom_inter);
	
	DEBUG_MESSAGE("e_d_curr"<<params.network.e_d_curr
			<<"e_d_prevr"<<params.network.e_d_prev
			<<"e_d_curr"<<params.network.d_curr
			<<"e_d_prev"<<params.network.d_prev);
	
	patReal res,err;
	patReal *xl;
	patReal *xu;
	
	patULong dimension;
	xl = new patReal[1];
		xu = new patReal[1];
		xl[0] = 0.0;
		xu[0] = 1.0;//e_a
		
		dimension = 1;
		G.f = &baseFunction_raw;
	G.dim = dimension;
	//G[2] = dimension;
	
	const gsl_rng_type *T;
	gsl_rng *r;


	
	size_t calls = 500000;
     
	gsl_rng_env_setup ();
	//DEBUG_MESSAGE("setup o k");

	T = gsl_rng_default;
	r = gsl_rng_alloc (T);
	{
		//DEBUG_MESSAGE("start integration, previous arcs"<<params.prev_arcs.size());
		gsl_monte_miser_state *s = gsl_monte_miser_alloc (dimension);
		gsl_monte_miser_integrate (&G, xl, xu, dimension, calls, r, s,
                                    &res, &err);
		gsl_monte_miser_free (s);
		//DEBUG_MESSAGE("2 integration result:"<<res<<","<<err);
		//display_results ("miser", res, err);
	}
	valueRaw = res;
	DEBUG_MESSAGE("raw arc transition value "<<valueRaw<<","<<err);
	return value;
	*/
}
list<patReal>  patArcTransition::calProbability(patNetwork* theNetwork, struct gps_params GP){
	list<patReal> values;
	values.push_back(calProbability(patNBParameters::the()->algoInSelection,theNetwork,GP));
	values.push_back(calProbability("raw",theNetwork,GP));
	values.push_back(calProbability("simple",theNetwork,GP));
	
	return values;
}
/*
void patArcTransition::setParam_discrete(struct base_params params, struct discrete_params* dp){
	dp->time_diff = params.gps.time_diff;
	dp->beta_0=params.network.mu_t_inter;
	dp->beta_a=params.network.l_curr/params.gps.mu_v_curr;
	dp->beta_b=params.network.l_prev/params.gps.mu_v_prev;
	
	dp->gamma_0= params.network.std_t_inter * params.network.std_t_inter;
	patReal t1=params.network.l_curr * params.gps.std_v_curr/(params.gps.mu_v_curr * params.gps.mu_v_curr);
	dp->gamma_a= t1 * t1;
	dp->gamma_b= (params.network.l_prev * params.gps.std_v_prev/(params.gps.mu_v_prev * params.gps.mu_v_prev))*\
	(params.network.l_prev * params.gps.std_v_prev/(params.gps.mu_v_prev * params.gps.mu_v_prev));
	
}

patReal patArcTransition::positionTransition_discrete(struct discrete_params* dp){
	mean= dp->beta_0 + dp->beta_a * e_a + dp->beta_b * (1.0-e_b);
	var = dp->gamma_0 + dp->gamma_a * e_a * e_a + dp->gamma_b * (1.0-e_b) * (1.0-e_b);
	
	return exp(-(dp->time_diff-mean)*(dp->time_diff-mean)/(2.0*var))/sqrt(2.0 * M_PI * var);
}

patReal patArcTransition::calProbability_discrete(patNetwork* theNetwork, struct gps_params GP){
	struct base_params params;
	params.gps = GP;
	setNetworkParams(theNetwork,&(params.network));
	struct discrete_params dp;
	setParam_discrete(params,&dp);
	positionTransition_discrete(&dp);
	
	
}
*/

/**
 *@param
 *k[0] e_a
 *k[1] e_b
 **/
patReal baseFunction_SIM(patReal* k, size_t dim, void* params){
	struct base_params * fp = (struct base_params *)params;

	patReal d_curr_2 = 	pow(fp->network.d_curr,2)+pow((k[0] - fp->network.e_d_curr) * fp->network.l_curr,2);	
	patReal d_prev_2 = pow(fp->network.d_prev,2)+pow((k[1] - fp->network.e_d_prev) * fp->network.l_prev,2);
	
	return  exp(	-((d_curr_2 )/ (2 * pow(fp->gps.std_x_curr,2))) \
				-((d_prev_2)/(2 * pow(fp->gps.std_x_prev,2))) \
				-pow((k[0] * fp->network.l_curr+(1-k[1]) * fp->network.l_prev + fp->network.l_inter)/(fp->gps.time_diff) - fp->gps.mu_v_curr,2)/(2*pow(fp->gps.std_v_curr, 2)));
}
/*
patReal baseFunction_Triangular(patReal* k, size_t dim, void* params){

	struct base_params * fp = (struct	base_params *)params;
	patReal v=(k[0]*fp->network.l_curr+(1-k[1])*fp->network.l_prev+fp->network.l_inter)/fp->gps.time_diff;
	patReal s=1.0;
	if (v<0 || v>=fp->gps.max_v_inter){
		return 0.0;
	}
	else if (v==0.0){
		s = fp->gps.p_0;
	}
	else if (v>0 && v<= fp->gps.mu_v_inter) {
		s=(1-fp->gps.p_0)*v/fp->gps.mu_v_inter+fp->gps.p_0;
	}
	else if (v>fp->gps.mu_v_inter && v<fp->gps.max_v_inter){
		s=(fp->gps.max_v_inter-v)/(fp->gps.max_v_inter-fp->gps.mu_v_inter);
	}

								  
	patReal d_curr_2 = 	pow(fp->network.d_curr,2)+pow((k[0] - fp->network.e_d_curr) * fp->network.l_curr,2);	
	patReal d_prev_2 = pow(fp->network.d_prev,2)+pow((k[1] - fp->network.e_d_prev) * fp->network.l_prev,2);
	
	return s * exp(	-((d_curr_2 )/ (2 * pow(fp->gps.std_x_curr,2))) \
				   -((d_prev_2)/(2 * pow(fp->gps.std_x_prev,2))));
}
 */

patReal baseFunction_Triangular(patReal* k, size_t dim, void* params){
	
	struct base_params * fp = (struct	base_params *)params;
	return fp->TM->integral(k[0],k[1]) * fp->MM->integral(k[0],k[1]);
}
patReal baseFunction_TM(patReal* k, size_t dim, void* params){
	
	struct base_params * fp = (struct	base_params *)params;
	return fp->TM->integral(k[0],k[1]) * fp->MM->integral(k[0],k[1]);
}


patReal patArcTransition::calProbability(
	patString algoType,patNetwork* theNetwork, struct gps_params GP){
	//DEBUG_MESSAGE("algo:"<<algoType);
	if(algoType =="raw"){
		return calProbability_raw(theNetwork, GP);
	}
	patReal res,err;

	patReal *xl;
	patReal *xu;
//	DEBUG_MESSAGE(algoType);
	
    struct base_params params;
	params.gps = GP;
	patReal resConst;
	setNetworkParams(theNetwork,&(params.network));
	
	/*
	DEBUG_MESSAGE("gps params"<<"time_diff:"<<GP.time_diff<<"; mu_v_curr:"<<GP.mu_v_curr<<";std_v_curr:"<<GP.std_v_curr<<
	";mu_v_prev:"<<GP.mu_v_prev<<";std_v_prev:"<<GP.std_v_prev<<";mu_v_inter:"<<GP.mu_v_inter<<
	";std_v_inter:"<<GP.std_v_inter<<";std_x_prev:"<<GP.std_x_prev<<";std_x_curr:"<<GP.std_x_curr<<
	";v_denom_prev"<<GP.v_denom_prev<<";v_denom_curr"<<GP.v_denom_curr<<";v_denom_inter"<<GP.v_denom_inter);
	*/
	gsl_monte_function G ;
	
	G.params = &params;
	patULong dimension;
	if (algoType == "simple"){	
		xl = new patReal[2];
		xu = new patReal[2];
		xl[0] = 0.0;
		xu[0] = 1.0;//e_a
		xl[1] = 0.0;
		xu[1] = 1.0;//e_b
		
		dimension = 2;
		resConst = 1 ;
		
	if(path.nbrOfArcs()==1){//the same arc;

		G.f = &baseFunction_simple_same;
		
	}
	else{
		G.f = &baseFunction_simple_diff;
		
	}
	
	}
	
	
/*
 simplified measuremet model with simplied traffic model
 */
else if(algoType=="sim2"){
	xl = new patReal[2];
	xu = new patReal[2];
	xl[0] = 0.0;
	xu[0] = 1.0;//\ell_k
	xl[1] = 0.0;
	xu[1] = 1.0;// ell_{k-1}
	dimension = 2;
	resConst = params.network.l_curr * params.network.l_prev;
	
	G.f = &baseFunction_SIM;
	
}
else if (algoType=="triangular"){
	xl = new patReal[2];
	xu = new patReal[2];
	struct link_ddr_range curr_ddr_range=currGpsPoint->getLinkDDRRange(getLastArc());
	struct link_ddr_range prev_ddr_range=prevGpsPoint->getLinkDDRRange(getFirstArc());
	
	xl[0] = curr_ddr_range.lower_bound;
	xu[0] = curr_ddr_range.upper_bound;//\ell_k
	xl[1] = prev_ddr_range.lower_bound;
	xu[1] = prev_ddr_range.upper_bound;// ell_{k-1}
	dimension = 2;
	
	params.TM = new patTrafficModelTriangular();
	params.TM->setParams(params.gps,params.network);
	params.MM = new patMeasurementModelV2();
	params.MM->setParams(params.gps,params.network);
	resConst = params.TM->getConstant() * params.MM->getConstant();
//	DEBUG_MESSAGE("const:"<<resConst<<"="<<params.TM->getConstant() <<"*"<<params.MM->getConstant());
	G.f = &baseFunction_Triangular;
	
}
else if (algoType=="complex"){
	xl = new patReal[2];
	xu = new patReal[2];
	struct link_ddr_range curr_ddr_range=currGpsPoint->getLinkDDRRange(getLastArc());
	struct link_ddr_range prev_ddr_range=prevGpsPoint->getLinkDDRRange(getFirstArc());
	
	xl[0] = curr_ddr_range.lower_bound;
	xu[0] = curr_ddr_range.upper_bound;//\ell_k
	xl[1] = prev_ddr_range.lower_bound;
	xu[1] = prev_ddr_range.upper_bound;// ell_{k-1}
	
	/*
	DEBUG_MESSAGE("previous link ddr"<<xl[0]<<"-"<<xu[0]);
	DEBUG_MESSAGE("current link ddr"<<xl[1]<<"-"<<xu[1]);
	
	DEBUG_MESSAGE(params.gps.time_diff);
	DEBUG_MESSAGE(params.network.d_curr<<","<<params.network.d_prev);
	DEBUG_MESSAGE(params.network.e_d_curr<<","<<params.network.e_d_prev);
	DEBUG_MESSAGE(params.network.l_curr<<","<<params.network.l_inter<<","<<params.network.l_prev);
	*/
	dimension = 2;
	params.TM = new patTrafficModelComplex;
	params.TM->setParams(params.gps,params.network);
	params.MM = new patMeasurementModelV2();
	params.MM->setParams(params.gps,params.network);
	resConst = params.TM->getConstant() * params.MM->getConstant();
	//DEBUG_MESSAGE("const:"<<resConst<<"="<<params.TM->getConstant() <<"*"<<params.MM->getConstant());
	G.f = &baseFunction_TM;
	
}
else if (algoType=="ts"){
	params.TM = new patTrafficModelTriangular();
	params.TM->setParams(params.gps,params.network);
	params.MM = new patMeasurementModelV2();
	params.MM->setParams(params.gps,params.network);
//	DEBUG_MESSAGE("nb of arcs"<<path.nbrOfArcs())
	if(path.nbrOfArcs()==1){//the same arc;
	
		xl = new patReal[3];
		xu = new patReal[3];
		xl[0] = 0.0;
		xu[0] = 1.0;//e_a
		xl[1] = 0.0;
		xu[1] = 5 * params.network.l_curr /params.gps.mu_v_curr;//t_a
		xl[2] = 0.0;
		xu[2] = 1.0;//e_b
//		DEBUG_MESSAGE(xu[1]);
		dimension = 3;
		resConst = params.MM->getConstant()*1 / (2 * M_PI * params.gps.std_v_prev * params.gps.std_v_curr * params.gps.v_denom_curr * params.gps.v_denom_prev);
			
		G.f = &baseFunction_arc_arc_same;
		
	}
	
	else if(path.nbrOfArcs()==2){//adjacent arc;
		resConst =  params.MM->getConstant()*1 / (2 * M_PI * params.gps.std_v_prev * params.gps.std_v_curr* params.gps.v_denom_curr * params.gps.v_denom_prev);
		if(params.network.t_w.empty()){
		dimension = 3;
		xl = new patReal[dimension];
		xu = new patReal[dimension];
		xl[0] = 0.0;
		xu[0] = 1.0;//e_a
		xl[1] = 0.0;
		xu[1] = 5 *  params.network.l_curr /params.gps.mu_v_curr;//t_a
		xl[2] = 0.0;
		xu[2] = 1.0;//e_b
		
		G.f = &baseFunction_arc_arc_adj_no_stop;
		}
		else{
		xl = new patReal[4];
		xu = new patReal[4];
		xl[0] = 0.0;
		xu[0] = 1.0;//e_a
		xl[1] = 0.0;
		xu[1] = 5 *  params.network.l_curr /params.gps.mu_v_curr;//t_a
		xl[2] = 0.0;
		xu[2] = 1.0;//e_b
		xl[3] = 0.0;
		xu[3] = (params.network.t_w[0] < params.gps.time_diff)?params.network.t_w[0] : params.gps.time_diff;//t_w
		
		dimension = 4;
		G.f = &baseFunction_arc_arc_adj;
		}
	}
	
	else{
		dimension = 4 + params.network.t_w.size();
		xl = new patReal[dimension];
		xu = new patReal[dimension];
		xl[0] = 0.0;
		xu[0] = 1.0;//e_a
		xl[1] = 0.0;
		xu[1] = 5 * params.network.l_curr /params.gps.mu_v_curr;//t_a
		xl[2] = 0.0;
		xu[2] = 1.0;//e_b
		xl[3] = 0.0;
		xu[3] = 60.0;//v_c
		for(patULong i = 4;i<dimension;++i){
			
		xl[i] = 0.0;
		xu[i] = (params.network.t_w[i-4] < params.gps.time_diff)?params.network.t_w[i-4] : params.gps.time_diff;//t_w
		}
		resConst =  params.MM->getConstant()*1 / (sqrt(2 * M_PI) * 2 * M_PI * \
		params.gps.std_v_prev * params.gps.std_v_curr * params.gps.std_v_inter \
		* params.gps.v_denom_curr * params.gps.v_denom_prev * params.gps.v_denom_inter);
		G.f = &baseFunction_arc_arc_inter;		
	}
	}
	for(patULong i = 0; i<dimension;++i){
		if(xl[i] >= xu[i]){
			DEBUG_MESSAGE("algorithm:"<<algoType<<"invalid bound "<<path.nbrOfArcs()<<" "<<i<<":"<<xl[i]<<","<<xu[i]);
		}
	}
	G.dim = dimension;
	//G[2] = dimension;
	
	const gsl_rng_type *T;
	gsl_rng *r;


	
	size_t calls = patNBParameters::the()->nbrOfIntegrationCalls;
     
	gsl_rng_env_setup ();
	//DEBUG_MESSAGE("setup o k");

	T = gsl_rng_default;
	r = gsl_rng_alloc (T);
	if(patNBParameters::the()->integrationMethod == "miser")
	
	{
		//DEBUG_MESSAGE("start integration, previous arcs"<<params.prev_arcs.size());
		gsl_monte_miser_state *s = gsl_monte_miser_alloc (dimension);
		gsl_monte_miser_integrate (&G, xl, xu, dimension, calls, r, s,
                                    &res, &err);
		gsl_monte_miser_free (s);
		//DEBUG_MESSAGE("2 integration result:"<<res<<","<<err);
		//display_results ("miser", res, err);
	}
	else if(patNBParameters::the()->integrationMethod == "vegas")
	{
		   gsl_monte_vegas_state *s = gsl_monte_vegas_alloc (dimension);
     
         gsl_monte_vegas_integrate (&G, xl, xu, dimension, calls/4, r, s,
                                    &res, &err);
     
        // printf ("converging...\n");
     
         do
           {
             gsl_monte_vegas_integrate (&G, xl, xu, dimension, calls/2, r, s,
                                        &res, &err);
			//DEBUG_MESSAGE("chisq"<<gsl_monte_vegas_chisq (s));
           }
         while (fabs (gsl_monte_vegas_chisq (s) - 1.0) > 0.5);
     
     
         gsl_monte_vegas_free (s);

	
	}
	else{
		gsl_monte_plain_state *s = gsl_monte_plain_alloc (dimension);
         gsl_monte_plain_integrate (&G, xl, xu, dimension, calls, r, s, 
                                    &res, &err);
         gsl_monte_plain_free (s);
	}
	value= resConst * res;
	valueSimple = value	;
	//DEBUG_MESSAGE("arc tran"<<value<<"="<<resConst<<"*"<<res);
	gsl_rng_free (r);
	
	delete(params.TM);
	delete(params.MM);
	return value;
}


vector<patReal> patArcTransition::dealIntersections(
	patNetwork* theNetwork){
	vector<patReal> waitings;
	list<patArc*>* listOfArcs = path.getArcList();
	if (listOfArcs->size()<2){
		return waitings;
	}
	patArc* prevArc = listOfArcs->front();
	list<patArc*>::iterator arcIter = listOfArcs->begin();
	arcIter++;
	patError* err;
	for(;
				arcIter!=listOfArcs->end();
				arcIter++){
		patReal t_w = calWaitingTime(prevArc,const_cast<patArc*>(*arcIter),theNetwork,err);
		if (t_w >=0.0){
		waitings.push_back(t_w);
		}
		prevArc = const_cast<patArc*>(*arcIter);
	}
	
	return waitings;
}

patReal patArcTransition::calWaitingTime(patArc* upArc, patArc* downArc,
	patNetwork* theNetwork,patError*& err){
	
	if(upArc->downNodeId != downArc->upNodeId){
    stringstream str ;
    str << (*upArc)<<" and " << (*downArc)<<" are not connected" ;
    err = new patErrMiscError(str.str()) ;
    WARNING(err->describe());

	}
	patNode* intersection = theNetwork->getNodeFromUserId(upArc->downNodeId);
	
	if(intersection ==NULL){
		err = new patErrNullPointer("patNode") ;
    WARNING(err->describe()) ;
    return -1.0;
	}
	//DEBUG_MESSAGE("node type"<<intersection->attributes.type);
	if(intersection->attributes.type == "traffic_signals"){
		//DEBUG_MESSAGE("a t raffic signals at"<<*intersection);
		return calSignalWaiting(upArc, downArc,intersection,theNetwork);
	}
	else if(upArc->attributes.priority != downArc->attributes.priority){
	   return patNBParameters::the()->stopPenalty;
	}
	
	return -1.0;
}

patReal patArcTransition::calSignalWaiting(patArc* upArc,patArc* downArc,patNode* intersection,
patNetwork* theNetwork){
	set<patULong>* successors = &(intersection->userSuccessors);
	map<patArc*, patReal> downHeading;
	patULong left=0;
	patULong right=0;
	patReal headingChange = 180.0-(downArc->attributes.heading) - upArc->attributes.heading;
	headingChange = (headingChange>360.0)?(headingChange-360.0):headingChange;
	headingChange = (headingChange<0.0)?(headingChange+360.0):headingChange;
		
	for(set<patULong>::iterator iter1 = successors->begin();
			iter1 != successors->end();
			++iter1){
		
		patArc* downStream = theNetwork->getArcFromNodesUserId(intersection->userId, *iter1);
		if(downStream == NULL)
		{
			continue;
		}
		patReal h1 = 180.0 - (downStream->attributes.heading - upArc->attributes.heading);
		h1 = (h1>360.0)?(h1-360.0):h1;
		h1 = (h1<0.0)?(h1+360.0):h1;
		
		if(fabs(h1)<10.0){
			continue;
		}
		
		if(h1<headingChange){
			right +=1;
		}
		if(h1>headingChange){
			left +=1;
		}
	}
	
	if(right==0 ){
		return patNBParameters::the()->rightTurnPenalty;
	}
	if (left == 0 && right ==1){
		return patNBParameters::the()->goStraightPenalty;
	} 
	if(left == 0 && right >1){
		return patNBParameters::the()->rightTurnPenalty;
	}
	if(left>0 && right >0)
	{
		return patNBParameters::the()->goStraightPenalty;
	}
}

patPathJ patArcTransition::getPath(){

	return path;
}
patReal patArcTransition::getValue(patString type,patNetwork* theNetwork, struct gps_params GP){
	if(type == "raw"){
		if(valueRaw < 0.0){
			calProbability_raw( theNetwork,  GP);
		}
		return valueRaw;
	}
	else if(type=="simple"){
	if(valueSimple < 0.0){
			calProbability("simple", theNetwork,  GP);
		}
		return valueSimple;
	}
	if (value<0.0){
		calProbability(patNBParameters::the()->algoInSelection, theNetwork,  GP);
	}
	return value;
}


	patGpsPoint* patArcTransition::getPrevGpsPoint(){
		return prevGpsPoint;
	}
	patGpsPoint* patArcTransition::getCurrGpsPoint(){
		return currGpsPoint;
	}
	patArc* patArcTransition::getFirstArc(){
		return path.front();
	}
	patArc* patArcTransition::getLastArc(){
		return path.back();
	}