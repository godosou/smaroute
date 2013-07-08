#include "patConst.h"
#include <gsl/gsl_math.h>
#include <gsl/gsl_monte.h>
#include <gsl/gsl_monte_plain.h>
#include <gsl/gsl_monte_miser.h>
#include <gsl/gsl_monte_vegas.h>
#include "patPathJ.h"
#include "patNetwork.h"
#include "patArc.h"
#include "patGpsPoint.h"
#include "patPathProbaAlgoV2.h" 
#include "patTripGraph.h" 
#include "patDisplay.h"
#include "patErr.h"
#include "patErrMiscError.h"

/*
*
*/

patPathProbaAlgoV2::patPathProbaAlgoV2(patPathJ* thePath,
 vector<patGpsPoint>* theGpsSequence,
 patNetwork* theNetwork):
	gpsSequence(theGpsSequence), 
	baseNetwork(theNetwork),
	path(thePath),
	proba(theGpsSequence->size(),0),
	pointTime(theGpsSequence->size(),0),
	pointArcProba(theGpsSequence->size(),map<patArc*,patReal>())
{
	//DEBUG_MESSAGE("Initiate path probability calculation");
}
patPathProbaAlgoV2::patPathProbaAlgoV2( vector<patGpsPoint>* theGpsSequence,
 patNetwork* theNetwork):
	gpsSequence(theGpsSequence), 
	baseNetwork(theNetwork),
	proba(theGpsSequence->size(),0),
	pointTime(theGpsSequence->size(),0),
	path(NULL),
	pointArcProba(theGpsSequence->size(),map<patArc*,patReal>())
{
	//DEBUG_MESSAGE("Initiate path probability calculation");
}
pair<patReal, patReal> patPathProbaAlgoV2::calPathLengthBound(){
	patReal definedSpeed = 7.5;//m/s
	
	pair<patReal, patReal> lengthBound;
	patULong timeLength = gpsSequence->back().getTimeStamp()-gpsSequence->front().getTimeStamp();
	
	lengthBound.first = 0;
	lengthBound.second = definedSpeed * timeLength * 1.5;
	
	return lengthBound;

}

patReal patPathProbaAlgoV2::calPathLength(){
	
	pathLength=0.0;
	list<patArc*> * listOfArcs = path->getArcList();
	
	for(list<patArc*>::iterator arcIter = listOfArcs->begin();
				arcIter!=listOfArcs->end();
				++arcIter){
				
		pathLength+=(*arcIter)->getLength();
	
	}
	
	return pathLength;
}

patBoolean patPathProbaAlgoV2::isPathLengthReasonable(){
	calPathLength();
	pair<patReal,patReal> lengthBound=calPathLengthBound();
	if(pathLength<=lengthBound.second && pathLength>=lengthBound.first){
		return true;
	}
	
	return false;
}


void patPathProbaAlgoV2::initArc(){
	proba[0]=1.0;
	pointArcProba[0][path->front()] = 1.0;	
}

patReal patPathProbaAlgoV2::calAverageSpeed(){
	patReal avegSpeed = 0.0;
	for(patULong i = 0; i<gpsSequence->size();++i){
		avegSpeed += gpsSequence->at(i).getSpeed();
	}
	
	avegSpeed /= gpsSequence->size();
	DEBUG_MESSAGE("average speed"<<avegSpeed/3.6);
	return avegSpeed/3.6;
}


patReal patPathProbaAlgoV2::getArcTime(patArc* theArc){
	patReal definedSpeed = calAverageSpeed();//m/s
	return (theArc->getLength())/(definedSpeed);
}

void patPathProbaAlgoV2::calTimeParams(){
	list<patArc*> * listOfArcs = path->getArcList();
	map<char*, patReal> distanceToArc = calDistancePoint2Arc(&(gpsSequence->at(0)),listOfArcs->front());
	//DEBUG_MESSAGE("cal time params:distance cal OK");
	patReal e_d = calPerpendicularFootOnArc(distanceToArc);
	e_d = (e_d<0)?0:e_d;
	e_d = (e_d>1)?1:e_d;
	
	//DEBUG_MESSAGE("cal time params:foot cal OK");
	patReal byTime = 0.0;
	patReal byDelta2 = (((e_d>0.0)?e_d:1) * getArcTime(listOfArcs->front()))*(((e_d>0.0)?e_d:1) * getArcTime(listOfArcs->front()));
	
	//DEBUG_MESSAGE("cal time params:delta cal ok");
	for(list<patArc*>::iterator arcIter = listOfArcs->begin();
						arcIter!=listOfArcs->end();
						++arcIter){
		struct time_params aParams;
		aParams.mu_tau = byTime;
		aParams.delta_tau = sqrt(byDelta2);
		aParams.mu_t = getArcTime(const_cast<patArc*>(*arcIter));
		aParams.delta_t=aParams.mu_t;
		
		byTime = byTime+aParams.mu_t;
		byDelta2 = byDelta2+aParams.mu_t * aParams.mu_t;
		timeParams[const_cast<patArc*>(*arcIter)]=aParams;
		
		//DEBUG_MESSAGE("mu tau:"<<aParams.mu_tau);
		//DEBUG_MESSAGE("delta tau:"<<aParams.delta_tau);
		//DEBUG_MESSAGE("mu t:"<<aParams.mu_t);
		//DEBUG_MESSAGE("delta t:"<<aParams.delta_t);
		
	}
	
		//DEBUG_MESSAGE("cal time params:arc iterate OK");
	
	patReal timeOffSet = (float)gpsSequence->at(0).getTimeStamp()-e_d * getArcTime(listOfArcs->front());
	//DEBUG_MESSAGE("cal time params:time offset cal ok");
	
	for(patULong i = 0;
		i < gpsSequence->size();
		++i){
		pointTime[i]=(float)gpsSequence->at(i).getTimeStamp()-timeOffSet;
		//DEBUG_MESSAGE("point time "<<i<<pointTime[i]);
	}	
	//DEBUG_MESSAGE("cal time params:point time cal ok");

}

patReal patPathProbaAlgoV2::run_gps(){
	//DEBUG_MESSAGE("exp"<<exp(-1/0.0));
	//DEBUG_MESSAGE("small"<<1/pow(10,100));
	//if(isPathLengthReasonable()==patFALSE){
		//return 0.0;
	//}
	
	calTimeParams();
	list<patArc*> * listOfArcs = path->getArcList();
	//DEBUG_MESSAGE("Get Arc List;");
	patReal probaPath = 1.0 * gpsSequence->at(0).getArcDDRValue(listOfArcs->front(),baseNetwork)/gpsSequence->at(0).getDDRSum();
	DEBUG_MESSAGE("set the init path proba"<<probaPath);
	if(probaPath==0.0){
		return 0.0;
	}
	initArc();
	for(patULong j = 1;
		j < gpsSequence->size();
		++j){
	DEBUG_MESSAGE("Calculate point:"<<j);
		
		calPointProba(j);
		if(proba[j] == 0.0){
			return 0.0;
		}
		//DEBUG_MESSAGE("point proba "<<j<<":"<<proba[j]);
	}
	DEBUG_MESSAGE("init proba"<<probaPath);
	for(patULong j=0;j<proba.size();j++){
	DEBUG_MESSAGE("point proba "<<j<<":"<<proba[j]);
		/*
		if(proba[j]==0.0){
			probaPath  *= 1/-(-301-log(1000));
	
		}
		else{
		probaPath  *= 1/-(log(proba[j])-log(1000));
		}
		*/
		probaPath  *= proba[j];
	}
		DEBUG_MESSAGE("path proba:"<<probaPath);

	return probaPath;
}

patReal patPathProbaAlgoV2::calPointProba(patULong j){
	list<patArc* > * listOfArcs = path->getArcList();

	proba[j]=0.0;
	map<patArc*, patReal>* theLinkDDR = gpsSequence->at(j).getLinkDDR();
	
	for(map<patArc*, patReal>::iterator arcIter = theLinkDDR->begin();
						arcIter!=theLinkDDR->end();
						++arcIter){
		if(path->isLinkInPath(arcIter->first)){
			DEBUG_MESSAGE("cal proba for"<<*(arcIter->first));
			pointArcProba[j][arcIter->first] = calPointArcProba_V4(j,pair<patArc*,patReal>(arcIter->first,arcIter->second));
			
			DEBUG_MESSAGE("point arc value:"<<pointArcProba[j][arcIter->first]);
			proba[j]+=pointArcProba[j][arcIter->first];
		}
	}
	
	DEBUG_MESSAGE("point proba "<<j<<":"<<proba[j]);
	
	return proba[j];
}

patReal patPathProbaAlgoV2::run_arc(){
	path = new patPathJ();
 return 0;
}


void patPathProbaAlgoV2::setFirstArcTimeParams(patArc* newArc){
	map<char*, patReal> distanceToArc = calDistancePoint2Arc(&(gpsSequence->at(0)),newArc);
	//DEBUG_MESSAGE("cal time params:distance cal OK");
	patReal e_d = calPerpendicularFootOnArc(distanceToArc);
	e_d = (e_d<0)?0:e_d;
	e_d = (e_d>1)?1:e_d;
	
	//DEBUG_MESSAGE("cal time params:foot cal OK");
	struct time_params aParams;
	aParams.mu_tau = 0.0;
	aParams.delta_tau = (((e_d>0.0)?e_d:1) * getArcTime(newArc));
	aParams.mu_t = getArcTime(newArc);
	aParams.delta_t=aParams.mu_t;
		
	timeParams[newArc]=aParams;

	patReal timeOffSet = gpsSequence->at(0).getTimeStamp()-e_d * getArcTime(newArc);
	//DEBUG_MESSAGE("cal time params:time offset cal ok");
	
	for(patULong i = 0;
		i < gpsSequence->size();
		++i){
 		pointTime[i]=gpsSequence->at(i).getTimeStamp()-timeOffSet;
		//DEBUG_MESSAGE("point time "<<i<<pointTime[i]);
	}	
	//DEBUG_MESSAGE("cal time params:point time cal ok");

}

void patPathProbaAlgoV2::dealFirstArc(patArc* newArc){
	setFirstArcTimeParams(newArc);
	initArc();
	initArcProba = 1.0*(*(gpsSequence->at(0).getLinkDDR()))[newArc]/gpsSequence->at(0).getDDRSum();

}


void patPathProbaAlgoV2::pushArc(patArc* newArc){
	path->addArcToBack(newArc);
	if(arcContributes.empty()==true){
		dealFirstArc(newArc);
		return ;
	}
	setArcTimeParams(newArc);
	calArcProba(newArc);
	
}
void patPathProbaAlgoV2::popArc(){
	patArc* deleteArc = path->deleteArcFromBack();
	
	if(deleteArc == NULL){
		return;
	}
	map<patArc*, map<patULong, patReal> >::iterator arcFound = arcContributes.find(deleteArc);
	if(arcFound!=arcContributes.end()){
		for(map<patULong,patReal>::iterator gpsIter = arcFound->second.begin();
				gpsIter != arcFound->second.end();
				++gpsIter
		){
			proba[gpsIter->first] -= gpsIter->second;
		}
	}
	arcContributes.erase(deleteArc);
	timeParams.erase(deleteArc);
	
}

patReal baseFunction_N1(patReal* k, size_t dim, void* params){
	struct time_params * fp = (struct time_params *)params;
	
	return exp(-pow((k[0] - fp->mu_t)/(fp->delta_t),2))/k[0];
}

void patPathProbaAlgoV2::setArcTimeParams(patArc* newArc){

	patReal byTime = 0.0;
	patReal byDelta2 = 0.0;
	list<patArc*>* arcList = path->getArcList();
	
	for(list<patArc*>::iterator arcIter = arcList->begin();
					(*arcIter) != newArc;
					++arcIter){
					
		if(arcIter == arcList->end()){
			return;
		}
		
		byTime += timeParams[*arcIter].mu_t;
		byDelta2 += timeParams[*arcIter].delta_t * timeParams[*arcIter].delta_t;
	
	}
	
	struct time_params aParams;

	aParams.mu_t = getArcTime(newArc);
	aParams.delta_t=aParams.mu_t;
	
	aParams.mu_tau = byTime;
	aParams.delta_tau = sqrt(byDelta2);
	
	timeParams[newArc]=aParams;

}

void patPathProbaAlgoV2::calArcProba(patArc* newArc){
	arcContributes[newArc]=map<patULong,patReal>();
	for(int j = 0;j<gpsSequence->size();++j){
		pair<patArc*,patReal> arcInDomain = gpsSequence->at(j).isInArcDomain(newArc);
		if(arcInDomain.second>=0){
		
			patReal pointArcProba = calPointArcProba(j,arcInDomain);
			proba[j]+=pointArcProba;
			arcContributes[newArc][j]=pointArcProba;
		}
		
	}
}

patReal patPathProbaAlgoV2::calPathProba(){
	patReal pathProba = initArcProba;
	
	for(int i = 0; i<proba.size();++i){
		pathProba*=proba[i];
	}
	
	return pathProba;
}

void patPathProbaAlgoV2::setParams(patULong j,patArc* theArc, struct base_f_params * p){
	
	p->l_a = theArc->getLength();
	p->mu_tau = timeParams[theArc].mu_tau;
	p->mu_t = timeParams[theArc].mu_t;
	p->delta_tau = timeParams[theArc].delta_tau;
	p->delta_t = timeParams[theArc].delta_t;
	
	p->t_j_1 = pointTime[j-1];
	p->t_j = pointTime[j];
	
	p->delta_j = gpsSequence->at(j).getHorizonAccuracy();
	p->delta_j_1 = gpsSequence->at(j-1).getHorizonAccuracy();
	
	
	//DEBUG_MESSAGE("length:"<<p->l_a);
	//DEBUG_MESSAGE("tau: mu "<<p->mu_tau<<", delta"<<p->delta_tau);
	//DEBUG_MESSAGE("t: mu "<<p->mu_t<<", delta"<<p->delta_t);
	//DEBUG_MESSAGE(" t_j_1 "<<p->t_j_1);
	//DEBUG_MESSAGE(" delta_j  "<<p->delta_j);
	//DEBUG_MESSAGE(" delta_j_1 "<<p->delta_j_1);
	
	
	
	
	map<char*, patReal> distanceToArc = calDistancePoint2Arc(&(gpsSequence->at(j)),theArc);
	p->d_j = distanceToArc["ver"];
		//	DEBUG_MESSAGE("distance cal  OK"<<p->d_j);

	p->e_j = calPerpendicularFootOnArc(distanceToArc);
//		DEBUG_MESSAGE("per foot set OK"<<p->e_j);

	distanceToArc = calDistancePoint2Arc(&(gpsSequence->at(j-1)),theArc);
	p->d_j_1 = distanceToArc["ver"];
	//			DEBUG_MESSAGE("distance cal  OK"<<p->d_j_1);

	p->e_j_1 = calPerpendicularFootOnArc(distanceToArc);
	//DEBUG_MESSAGE("per foot set set OK"<<p->e_j_1);
}	

patReal patPathProbaAlgoV2::calPerpendicularFootOnArc(map<char*, patReal> distanceToArc){
	if(isnan(distanceToArc["ver"])){
		DEBUG_MESSAGE("wrong ver");
		return distanceToArc["ver"];
	}
	else if(distanceToArc["position"]==-1){
		return -sqrt(pow(distanceToArc["link"],2)-pow(distanceToArc["ver"],2))/distanceToArc["length"];
	}
	
	else if(distanceToArc["position"]==0){
		return sqrt(pow(distanceToArc["up"],2)-pow(distanceToArc["ver"],2))/distanceToArc["length"];
	}
	
	else if(distanceToArc["position"]==11){
		return 1+sqrt(pow(distanceToArc["link"],2)-pow(distanceToArc["ver"],2))/distanceToArc["length"];
	}

}

/**
* base function to be integrated
*@param k[0], epsilon
*@param k[1], t
*@param k[2], tau
*@param dim, dimention of the integration
*@param params, parameters for the function
* return the value of the function
*/
patReal baseFunction(patReal* k, size_t dim, void* params){
	struct base_f_params * fp = (struct base_f_params *)params;
	patReal rtnValue =  2.0 * sqrt((pow(fp->d_j,2)+pow(((k[0]-fp->e_j)*fp->l_a),2)))/pow(fp->delta_j,2)\
			*exp(-(pow(fp->d_j,2)+pow(((k[0]-fp->e_j)*fp->l_a),2))/pow(fp->delta_j,2))\
			*exp(-(pow(((fp->t_j-k[0]*k[1]-fp->mu_tau)/fp->delta_tau),2)+pow(((k[1]-fp->mu_t)/fp->delta_t),2)))\
			*exp(-(((pow(fp->d_j_1,2)+pow((((fp->t_j_1-k[2])/k[1]-fp->e_j_1)*fp->l_a),2))/pow(fp->delta_j_1,2))+pow(((k[2]-fp->mu_tau)/fp->delta_tau),2)));
	
	//DEBUG_MESSAGE("function value"<<rtnValue);
	return rtnValue;
}

/**
*
*@param k
**[0] e
**[1] t
**[2] t_a
**[3] tau_a
**[4] tau_b
**[5] t_b
**[6] e_b
*/
patReal baseFunction_2(patReal* k, size_t dim, void* params){
	struct base_f_params_2 * fp = (struct base_f_params_2 *)params;
	patReal rtnValue=0.0;
	list<struct inner_f_params> preArcs = fp->prev_arcs;
	
	for(list<struct inner_f_params>::iterator arcIter=fp->prev_arcs.begin();
				arcIter!=fp->prev_arcs.end();
				++arcIter){
		
		if(k[3]<(fp->t_j-k[2]) || k[4]>=k[3]){ 
			rtnValue += 0.0;
		}
		patReal item1 = exp(-pow(((fp->t_j_1- k[6] * k[5] - (*arcIter).mu_tau_b)/((*arcIter).delta_tau_b)),2)\
					-pow(((k[5] - (*arcIter).mu_t_b)/((*arcIter).delta_t_b)),2))\
					* (*arcIter).ddr_b / (k[5] * 2 * pi * (*arcIter).delta_t_b * (*arcIter).delta_tau_b  );
		 patReal item2 =1.0;
		if( (*arcIter).delta_by>0.0){
			 item2 = exp(-pow(((k[3] - k[4] - (*arcIter).mu_by)/((*arcIter).delta_by)),2)) / ((*arcIter).delta_by * sqrt(2 * pi));
		}
		
		rtnValue += item1 * item2;
	
	}
	//DEBUG_MESSAGE("function value"<<rtnValue);
	rtnValue *= exp(-( pow(fp->d_a,2)+pow(((k[0]-fp->e_d) * fp->l_a),2)/ pow(fp->delta_j,2)) - \
					2 * pow(((k[2]-fp->mu_t_a) / fp->delta_t_a),2))/k[1];
	
	//DEBUG_MESSAGE("function value"<<rtnValue);
	return rtnValue;
}


void patPathProbaAlgoV2::setParams_2(patULong j,patArc* theArc, struct base_f_params_2 * p){
		
	map<char*, patReal> distanceToArc = calDistancePoint2Arc(&(gpsSequence->at(j)),theArc);
	p->d_a = distanceToArc["ver"];
		//DEBUG_MESSAGE("distance cal  OK"<<p->d_a);
		p->l_a = theArc->getLength();
	p->delta_j = gpsSequence->at(j).getHorizonAccuracy();

	p->e_d = calPerpendicularFootOnArc(distanceToArc);
//		DEBUG_MESSAGE("per foot set OK"<<p->e_j);


	p->mu_tau_a = timeParams[theArc].mu_tau;
	p->mu_t_a = timeParams[theArc].mu_t;
	p->delta_tau_a = timeParams[theArc].delta_tau;
	p->delta_t_a = timeParams[theArc].delta_t;
	
	p->t_j = pointTime[j];
	p->t_j_1 = pointTime[j-1];
	
	
	//DEBUG_MESSAGE("length:"<<p->l_a);
	//DEBUG_MESSAGE("tau: mu "<<p->mu_tau<<", delta"<<p->delta_tau);
	//DEBUG_MESSAGE("t: mu "<<p->mu_t<<", delta"<<p->delta_t);
	//DEBUG_MESSAGE(" t_j_1 "<<p->t_j_1);
	//DEBUG_MESSAGE(" delta_j  "<<p->delta_j);
	//DEBUG_MESSAGE(" delta_j_1 "<<p->delta_j_1);
	
	map<patArc*,patReal>* prevLinkDDR = gpsSequence->at(j-1).getLinkDDR();
	//DEBUG_MESSAGE("prev domain:"<< prevLinkDDR->size());
	for(map<patArc*,patReal>::iterator arcIter = prevLinkDDR->begin();
					arcIter != prevLinkDDR->end();
					++arcIter){ 
	
		list<patArc*> interSeg = path->getSeg(arcIter->first,theArc);
		//DEBUG_MESSAGE("seg length"<<interSeg.size());
		if(interSeg.empty()){
			continue;
		}
		struct inner_f_params aParam;

		aParam.mu_t_b = timeParams[arcIter->first].mu_t;
		aParam.delta_t_b = timeParams[arcIter->first].delta_t;
		aParam.mu_tau_b = timeParams[arcIter->first].mu_tau;
		aParam.delta_tau_b = timeParams[arcIter->first].delta_tau;
		aParam.ddr_b = arcIter->second;

		//DEBUG_MESSAGE("seg length"<<interSeg.size());
		aParam.mu_by = 0.0;
		aParam.delta_by = 0.0;
		interSeg.pop_back();
		for(list<patArc*>::iterator iter2 = interSeg.begin();
				iter2 != interSeg.end();
				++iter2){
			aParam.mu_by += timeParams[*iter2].mu_t;
			aParam.delta_by += timeParams[*iter2].delta_t;
		}
		
		(p->prev_arcs).push_back(aParam);
		//DEBUG_MESSAGE("inner set ok 1"); 
	}
	
	//DEBUG_MESSAGE("param set ok"); 

}	

void patPathProbaAlgoV2::setParams_3(patULong j,patArc* theArc
							, struct curr_arc_params * p
							, map<patArc*, struct prev_arc_params>* lp){
		
	map<char*, patReal> distanceToArc = calDistancePoint2Arc(&(gpsSequence->at(j)),theArc);
	p->d_a = distanceToArc["ver"];
		//DEBUG_MESSAGE("distance cal  OK"<<p->d_a);
		p->l_a = theArc->getLength();
	p->delta_j = gpsSequence->at(j).getHorizonAccuracy();

	p->e_d = calPerpendicularFootOnArc(distanceToArc);
//		DEBUG_MESSAGE("per foot set OK"<<p->e_j);


	p->mu_tau_a = timeParams[theArc].mu_tau;
	p->mu_t_a = timeParams[theArc].mu_t;
	p->delta_tau_a = timeParams[theArc].delta_tau;
	p->delta_t_a = timeParams[theArc].delta_t;
	
	p->t_j = pointTime[j];
	p->t_j_1 = pointTime[j-1];
	
	
	//DEBUG_MESSAGE("length:"<<p->l_a);
	//DEBUG_MESSAGE("tau: mu "<<p->mu_tau<<", delta"<<p->delta_tau);
	//DEBUG_MESSAGE("t: mu "<<p->mu_t<<", delta"<<p->delta_t);
	//DEBUG_MESSAGE(" t_j_1 "<<p->t_j_1);
	//DEBUG_MESSAGE(" delta_j  "<<p->delta_j);
	//DEBUG_MESSAGE(" delta_j_1 "<<p->delta_j_1);
	
	map<patArc*,patReal>* prevLinkDDR = gpsSequence->at(j-1).getLinkDDR();
	//DEBUG_MESSAGE("prev domain:"<< prevLinkDDR->size());
	for(map<patArc*,patReal>::iterator arcIter = prevLinkDDR->begin();
					arcIter != prevLinkDDR->end();
					++arcIter){ 
	
		list<patArc*> interSeg = path->getSeg(arcIter->first,theArc);
		//DEBUG_MESSAGE("seg length"<<interSeg.size());
		if(interSeg.empty()){
			continue;
		}
		struct prev_arc_params aParam;

		aParam.mu_t_b = timeParams[arcIter->first].mu_t;
		aParam.delta_t_b = timeParams[arcIter->first].delta_t;
		aParam.mu_tau_b = timeParams[arcIter->first].mu_tau;
		aParam.delta_tau_b = timeParams[arcIter->first].delta_tau;
		aParam.ddr_b = arcIter->second;

		//DEBUG_MESSAGE("seg length"<<interSeg.size());
		aParam.mu_by = 0.0;
		aParam.delta_by = 0.0;
		interSeg.pop_back();
		for(list<patArc*>::iterator iter2 = interSeg.begin();
				iter2 != interSeg.end();
				++iter2){
			aParam.mu_by += timeParams[*iter2].mu_t;
			aParam.delta_by += pow(timeParams[*iter2].delta_t,2);
		}
		aParam.delta_by = sqrt(aParam.delta_by);
		(*lp)[arcIter->first] = aParam;
		//DEBUG_MESSAGE("inner set ok 1"); 
	}
	
	//DEBUG_MESSAGE("param set ok"); 

}	

patReal patPathProbaAlgoV2::calArcDDR(patULong j,patArc* theArc){
	return gpsSequence->at(j).measureDDR(gpsSequence->at(j).distanceTo(baseNetwork,theArc));
}
patReal baseFunction_Uncondition(patReal* k, size_t dim, void* params){
	struct base_f_params * fp = (struct base_f_params *)params;
	patReal rtnValue = exp(-(pow(((fp->t_j-k[0]*k[1]-fp->mu_tau)/fp->delta_tau),2)+pow(((k[1]-fp->mu_t)/fp->delta_t),2)));
	//DEBUG_MESSAGE("function value"<<rtnValue);
	return rtnValue;
}



patReal patPathProbaAlgoV2::calPointArcProba_Uncondition(patULong j, pair<patArc*,patReal> arcDDR){
	/*
	//patReal probaConstant = 1.00 / (2*pi*timeParams[arcDDR.first].delta_tau * timeParams[arcDDR.first].delta_t);
	
	patReal probaConstant = 1.00 / (sqrt(pow(2*pi,3)) * pow(timeParams[arcDDR.first].delta_tau,2) * timeParams[arcDDR.first].delta_t);
	
	patReal res,err;
	patReal xl[3]={0,timeParams[arcDDR.first].mu_t-3 * timeParams[arcDDR.first].delta_t,timeParams[arcDDR.first].mu_tau-3 * timeParams[arcDDR.first].delta_tau};
	patReal xu[3]={1,timeParams[arcDDR.first].mu_t+3 * timeParams[arcDDR.first].delta_t,timeParams[arcDDR.first].mu_tau+3 * timeParams[arcDDR.first].delta_tau};
	
	//patReal xl[3]={0,-200,-200};
	//patReal xu[3]={1,200,200};
	
	struct base_f_params params;
	setParams(j,arcDDR.first,&params);
		//DEBUG_MESSAGE("param set ok");

	const gsl_rng_type *T;
    gsl_rng *r;
     
    gsl_monte_function G = { &baseFunction, 3, &params };
    */ 
	
	struct base_f_params_2 params;
	setParams_2(j,arcDDR.first,&params);
	
	patReal maxDelta = 0.0;
	for(list<struct inner_f_params>::iterator paramIter = params.prev_arcs.begin();
			paramIter != params.prev_arcs.end();
			++paramIter){
			
		if((*paramIter).delta_t_b>maxDelta){
			maxDelta =(*paramIter).delta_t_b ;
		}
	}
	patReal probaConstant = 1.00 / (2 * pi * pow(timeParams[arcDDR.first].delta_t,2));
	
	patReal res,err;
	patReal xl[7]={	0,
					0,
					0,
					pointTime[j]- 5 * 1.0 * timeParams[arcDDR.first].delta_t-30,
					pointTime[j]- 5 * maxDelta-30,
					0,
					0,
				};
	patReal xu[7]={	1,
					timeParams[arcDDR.first].mu_t+5.0 * timeParams[arcDDR.first].delta_t,	
					timeParams[arcDDR.first].mu_t+5.0 * timeParams[arcDDR.first].delta_t,
					pointTime[j],
					pointTime[j-1],
					100,
					1,
				};
		//DEBUG_MESSAGE("param set ok");

	const gsl_rng_type *T;
    gsl_rng *r;
     //DEBUG_MESSAGE("OK1");
    gsl_monte_function G = { &baseFunction_2, 7, &params };
	
    size_t calls = 500000;
     
    gsl_rng_env_setup ();
     	//DEBUG_MESSAGE("setup o k");

    T = gsl_rng_default;
    r = gsl_rng_alloc (T);
	//DEBUG_MESSAGE("alloc ok");
     /*
       {
         gsl_monte_plain_state *s = gsl_monte_plain_alloc (3);
         gsl_monte_plain_integrate (&G, xl, xu, 3, calls, r, s, 
                                    &res, &err);
         gsl_monte_plain_free (s);
		
		DEBUG_MESSAGE("integration result:"<<res<<","<<err);
       }
	   */
       {
		DEBUG_MESSAGE("start integration, previous arcs"<<params.prev_arcs.size());
         gsl_monte_miser_state *s = gsl_monte_miser_alloc (7);
         gsl_monte_miser_integrate (&G, xl, xu, 7, calls, r, s,
                                    &res, &err);
         gsl_monte_miser_free (s);
		 DEBUG_MESSAGE("integration result:"<<res<<","<<err);
         //display_results ("miser", res, err);
       }
     
	 /*
       {
         gsl_monte_vegas_state *s = gsl_monte_vegas_alloc (3);
     
         gsl_monte_vegas_integrate (&G, xl, xu, 3, 10000, r, s,
                                    &res, &err);
         display_results ("vegas warm-up", res, err);
     
         printf ("converging...\n");
     
         do
           {
             gsl_monte_vegas_integrate (&G, xl, xu, 3, calls/5, r, s,
                                        &res, &err);
             printf ("result = % .6f sigma = % .6f "
                     "chisq/dof = %.1f\n", res, err, s->chisq);
           }
         while (fabs (s->chisq - 1.0) > 0.5);
     
         display_results ("vegas final", res, err);
     
         gsl_monte_vegas_free (s);
       }
     */
	 	//DEBUG_MESSAGE("arc proba cal ok:"<<res<<","<<probaConstant);

       gsl_rng_free (r);
     return res * probaConstant;
}


/**
*
*@param k
**[0] e
**[1] t_a
**[2] tau_b
**[3] t_b
**[4] e_b
*/
patReal baseFunction_5(patReal* k, size_t dim, void* params){
	struct curr_arc_params * fp = (struct curr_arc_params *)params;
	
	if (k[2]< (fp->t_j-k[3])){
		return 0;
	}
	else{
		//distance*exp(-distance*distance/(accuracy*accuracy))/(0.5*accuracy*accuracy);
		/*
		return sqrt(pow(fp->d_a,2)+pow((k[0] - fp->e_d) * fp->l_a,2))\
				* exp(-((pow(fp->d_a,2)+pow((k[0] - fp->e_d) * fp->l_a,2))/pow(fp->delta_j,2))\
				-pow(((fp->t_j_1 - k[4] * k[3] - fp->mu_tau_a)/ fp->delta_tau_a),2)\
				-pow(((k[3] - fp->mu_t_a)/ fp->delta_t_a),2)\
				-pow(((k[1] - fp->mu_t_a)/ fp->delta_t_a),2)\
				)/ (k[3] * 0.5 * pow(fp->delta_j,2));
		//*/	
		///*
		return exp(-((pow(fp->d_a,2)+pow((k[0] - fp->e_d) * fp->l_a,2))/pow(fp->delta_j,2))\
				-pow(((fp->t_j_1 - k[4] * k[3] - fp->mu_tau_a)/ fp->delta_tau_a),2)\
				-pow(((k[3] - fp->mu_t_a)/ fp->delta_t_a),2)\
				-pow(((k[1] - fp->mu_t_a)/ fp->delta_t_a),2)\
				)/ k[3];
//*/
		
	}
}

/**
*
*@param k
**[0] e
**[1] t_a
**[2] tau_a
**[3] tau_b
**[4] t_b
**[5] e_b
*/
patReal baseFunction_6(patReal* k, size_t dim, void* params){
	struct base_f_param_3 * fp = (struct base_f_param_3 *)params;
	
	if (k[2]< (fp->a_param.t_j-k[1]) || k[3] >= k[2]){
		return 0;
	}
	else{
	/*
		return sqrt(pow(fp->a_param.d_a,2)+pow((k[0] - fp->a_param.e_d) * fp->a_param.l_a,2)) \
				* exp(-((pow(fp->a_param.d_a,2)+pow((k[0] - fp->a_param.e_d) * fp->a_param.l_a,2))/pow(fp->a_param.delta_j,2))\
				-pow(((fp->a_param.t_j_1 - k[4] * k[5] - fp->b_param.mu_tau_b)/ fp->b_param.delta_tau_b),2)\
				-pow(((k[4] - fp->b_param.mu_t_b)/ fp->b_param.delta_t_b),2)\
				-pow(((k[2]-k[3]-fp->b_param.mu_by)/fp->b_param.delta_by),2)\
				-pow(((k[1] - fp->a_param.mu_t_a)/ fp->a_param.delta_t_a),2)\
				)/ (k[4] * 0.5 * pow(fp->a_param.delta_j,2));
	//*/
	///*
		return exp(-((pow(fp->a_param.d_a,2)+pow((k[0] - fp->a_param.e_d) * fp->a_param.l_a,2))/pow(fp->a_param.delta_j,2))\
				-pow(((fp->a_param.t_j_1 - k[4] * k[5] - fp->b_param.mu_tau_b)/ fp->b_param.delta_tau_b),2)\
				-pow(((k[4] - fp->b_param.mu_t_b)/ fp->b_param.delta_t_b),2)\
				-pow(((k[2]-k[3]-fp->b_param.mu_by)/fp->b_param.delta_by),2)\
				-pow(((k[1] - fp->a_param.mu_t_a)/ fp->a_param.delta_t_a),2)\
				)/ k[4];
		//*/
	}
}


patReal patPathProbaAlgoV2::calPointArcProba(patULong j,pair<patArc*,patReal> arcDDR){
	
	struct curr_arc_params currParams;
	map<patArc*, struct prev_arc_params> prevParams;
	setParams_3(j,arcDDR.first,&currParams, &prevParams);
     //DEBUG_MESSAGE("OK1");
   
	patReal res,err;
	patReal rtnValue = 0.0;
	
	for(map<patArc*, struct prev_arc_params>::iterator prevArcIter = prevParams.begin();
				prevArcIter != prevParams.end();
				++prevArcIter){
		
		if(pointArcProba[j-1].find(prevArcIter->first)==pointArcProba[j-1].end()
				||pointArcProba[j-1][prevArcIter->first]==0.0){
				
				continue;
		
		}
		patReal DDRShare= pointArcProba[j-1][prevArcIter->first]/proba[j-1];
		
		patReal probaConst;
		patReal upTau_a,downTau_a;
	
		if(prevArcIter->second.mu_by == 0.0 ){
			const gsl_rng_type *T;
			gsl_rng *r;
			upTau_a = currParams.t_j_1;
			downTau_a = currParams.t_j_1- 5 * currParams.delta_t_a;
			//DEBUG_MESSAGE("1 up down"<<upTau_a<<","<<downTau_a<<","<<currParams.mu_t_a);

			probaConst = 1/(pow(2*pi,2) * pow(currParams.delta_t_a,3) * currParams.delta_tau_a);
			gsl_monte_function G = { &baseFunction_5, 5, &currParams };
				
			patReal xl[5]={	0.0,
					0.0,
					downTau_a,
					0.0,
					0.0,
				};
			patReal xu[5]={	1.0,	
					currParams.mu_t_a+5.0 * currParams.delta_t_a,
					upTau_a,
					currParams.mu_t_a+5.0 * currParams.delta_t_a,
					1.0,
				};
				
			for(int id=0;id<5;id++){
				if(xl[id]>=xu[id]){
				DEBUG_MESSAGE("edrror bound "<<id<<":"<<xl[id]<<","<<xu[id]);
				
				}
			}
			size_t calls = 500000;
     
			gsl_rng_env_setup ();
			//DEBUG_MESSAGE("setup o k");

			T = gsl_rng_default;
			r = gsl_rng_alloc (T);
			{
				//DEBUG_MESSAGE("start integration, previous arcs"<<params.prev_arcs.size());
				gsl_monte_miser_state *s = gsl_monte_miser_alloc (5);
				gsl_monte_miser_integrate (&G, xl, xu, 5, calls, r, s,
                                    &res, &err);
				gsl_monte_miser_free (s);
				//DEBUG_MESSAGE("1 integration result:"<<res<<","<<err);
				//display_results ("miser", res, err);
			}
		}
		else{
			const gsl_rng_type *T;
			gsl_rng *r;

			
			struct base_f_param_3 params;
			params.a_param = currParams;
			params.b_param = prevArcIter->second;
			
			upTau_a = params.a_param.t_j;
			downTau_a = params.a_param.t_j- 5 * params.a_param.delta_t_a;
			//DEBUG_MESSAGE("2 up down"<<upTau_a<<","<<downTau_a);

			probaConst = 1/(sqrt(pow(2*pi,5)) * pow(params.a_param.delta_t_a,2) \
					* params.b_param.delta_t_b * params.b_param.delta_tau_b * params.b_param.delta_by);
			
			gsl_monte_function G = { &baseFunction_6, 6, &params };
			
			
			patReal xl[6]={	0.0,
					0.0,
					downTau_a,
					params.b_param.mu_tau_b - 5 * params.b_param.delta_tau_b,
					0.0,
					0.0,
				};
			patReal xu[6]={	1.0,	
					currParams.mu_t_a+5.0 * currParams.delta_t_a,
					upTau_a,
					upTau_a,
					params.b_param.mu_t_b + 5 * params.b_param.delta_t_b,
					1.0,
				};
			size_t calls = 500000;
     
			gsl_rng_env_setup ();
			//DEBUG_MESSAGE("setup o k");

			T = gsl_rng_default;
			r = gsl_rng_alloc (T);
			{
				//DEBUG_MESSAGE("start integration, previous arcs"<<params.prev_arcs.size());
				gsl_monte_miser_state *s = gsl_monte_miser_alloc (6);
				gsl_monte_miser_integrate (&G, xl, xu, 6, calls, r, s,
                                    &res, &err);
				gsl_monte_miser_free (s);
				//DEBUG_MESSAGE("2 integration result:"<<res<<","<<err);
				//display_results ("miser", res, err);
			}
		}
		
	DEBUG_MESSAGE("res,probaConst,ddrshare"<<res<<","<<probaConst<<","<<DDRShare<<","<<res * probaConst * DDRShare);
	   
	 rtnValue += res * probaConst * DDRShare;	
	   
	}
	DEBUG_MESSAGE("value"<<rtnValue);
	//rtnValue *= timeParams[arcDDR.first].arc_int;
	
	return rtnValue;
	
}



/**
*Convert epsilon position to real position
*/

patReal patPathProbaAlgoV2::epsitionToReal(patReal epsilon, patArc* theArc){
	return epsilon * theArc->getLength();
}

/*
*calculate ditance from a gps point to arc
*/
map<char*, patReal> patPathProbaAlgoV2::calDistancePoint2Arc(patGpsPoint* thePoint,patArc* theArc){
	patGeoCoordinates* upNodeGeoCoord = &(baseNetwork->getNodeFromUserId(theArc->m_up_node_id)->geoCoord);
	patGeoCoordinates* downNodeGeoCoord = &(baseNetwork->getNodeFromUserId(theArc->m_down_node_id)->geoCoord);
	
	return thePoint->distanceTo(upNodeGeoCoord,downNodeGeoCoord);
		

}


/**
*
*@param k
**[0] e_a
**[1] t_a
**[2] e_b
**[3] v_c
*/
patReal baseFunction_V4_4(patReal* k, size_t dim, void* params){
	struct base_f_param_4 * fp = (struct base_f_param_4 *)params;
	
	patReal v_b = (1-k[2]) * fp->b_param.l / (fp->a_param.t -k[0] * k[1] - fp->b_param.t - (fp->b_param.l_c)/k[3] );
	patReal v_a = fp->a_param.l / k[1];
	if(v_b <0 || v_a <0){
		return 0.0;
	}
	
	patReal deltaConst  = 2 * 2 /(pow(fp->a_param.delta_x,2) * pow(fp->b_param.delta_x,2));
	return deltaConst * exp(	-((pow(fp->a_param.d,2)+pow((k[0] - fp->a_param.e_d) * fp->a_param.l,2))/pow(fp->a_param.delta_x,2)) \
				-pow((v_b - fp->b_param.mu_v)/fp->b_param.delta_v,2)/2 \
				-pow((k[3] - fp->b_param.mu_v_c)/fp->b_param.delta_v_c,2)/2 \
				-((pow(fp->b_param.d,2)+pow((k[2] - fp->b_param.e_d) * fp->b_param.l,2))/pow(fp->b_param.delta_x,2)) \
				-pow((v_a - fp->a_param.mu_v)/fp->a_param.delta_v,2)/2 );
	
}

/**
*
*@param k
**[0] e_a
**[1] t_a
**[2] e_b
*/
patReal baseFunction_V4_3(patReal* k, size_t dim, void* params){
	struct base_f_param_4 * fp = (struct base_f_param_4 *)params;
	patReal v_b ;
	if(fp->b_param.l_c == 0.0){// adjacent arc 
			 v_b = (1-k[2]) * fp->b_param.l / (fp->a_param.t -k[0] * k[1] - fp->b_param.t);
	}
	else{//-1 the same arc a = b
		 v_b = (k[2] * fp->b_param.l)/( fp->b_param.t - fp ->a_param.t + k[0] * k[1]);
	}

	patReal v_a = fp->a_param.l / k[1];
	

	if(v_b <0 || v_a <0){
			//	DEBUG_MESSAGE("v_b,v_a:"<<fp->b_param.l_c<<","<<v_b<<","<<v_a);

		return 0.0;
	}
	patReal deltaConst  = 2 * 2 /(pow(fp->a_param.delta_x,2) * pow(fp->b_param.delta_x,2));
	return deltaConst * exp(	-((pow(fp->a_param.d,2)+pow((k[0] - fp->a_param.e_d) * fp->a_param.l,2))/pow(fp->a_param.delta_x,2)) \
				-pow((v_b - fp->b_param.mu_v)/fp->b_param.delta_v,2)/2 \
				-((pow(fp->b_param.d,2)+pow((k[2] - fp->b_param.e_d) * fp->b_param.l,2))/pow(fp->b_param.delta_x,2)) \
				-pow((v_a - fp->a_param.mu_v)/fp->a_param.delta_v,2)/2 );
	
}

void patPathProbaAlgoV2::setParams_V4(patULong j,patArc* theArc
							, struct arc_params * p
							, map<patArc*, struct arc_params>* lp){
		
	map<char*, patReal> distanceToArc = calDistancePoint2Arc(&(gpsSequence->at(j)),theArc);
	
	p->l = theArc->getLength();
	
	p->d = distanceToArc["ver"];
		//DEBUG_MESSAGE("distance cal  OK"<<p->d_a);

	p->e_d = calPerpendicularFootOnArc(distanceToArc);
//		DEBUG_MESSAGE("per foot set OK"<<p->e_j);

	p->t = pointTime[j];
	p->l_c = 0;
	p->delta_x = gpsSequence->at(j).getHorizonAccuracy();
	
	p->mu_v = gpsSequence->at(j).getSpeedMS();
	p->delta_v = gpsSequence->at(j).getSpeedAccuracyMS();
	p->mu_v_c = 0.0;
	p->delta_v_c = 0.0;
	
	
	//DEBUG_MESSAGE("length:"<<p->l_a);
	//DEBUG_MESSAGE("tau: mu "<<p->mu_tau<<", delta"<<p->delta_tau);
	//DEBUG_MESSAGE("t: mu "<<p->mu_t<<", delta"<<p->delta_t);
	//DEBUG_MESSAGE(" t_j_1 "<<p->t_j_1);
	//DEBUG_MESSAGE(" delta_j  "<<p->delta_j);
	//DEBUG_MESSAGE(" delta_j_1 "<<p->delta_j_1);
	
	map<patArc*,patReal>* prevLinkDDR = gpsSequence->at(j-1).getLinkDDR();
	//DEBUG_MESSAGE("prev domain:"<< prevLinkDDR->size());
	for(map<patArc*,patReal>::iterator arcIter = prevLinkDDR->begin();
					arcIter != prevLinkDDR->end();
					++arcIter){ 
	
		list<patArc*> interSeg = path->getSeg(arcIter->first,theArc);
		//DEBUG_MESSAGE("seg length"<<interSeg.size());
		if(interSeg.empty()){
			continue;
		}
		struct arc_params aParam;
		
		aParam.l = arcIter->first->getLength();
		distanceToArc = calDistancePoint2Arc(&(gpsSequence->at(j-1)),arcIter->first);
	
		
		aParam.d = distanceToArc["ver"];
		aParam.e_d = calPerpendicularFootOnArc(distanceToArc);
		
		aParam.t = pointTime[j-1];
		aParam.mu_v = gpsSequence->at(j-1).getSpeedMS();
		aParam.delta_v = gpsSequence->at(j-1).getSpeedAccuracyMS();
		aParam.delta_x = gpsSequence->at(j-1).getHorizonAccuracy();
		
		aParam.mu_v_c = (aParam.mu_v+p->mu_v)/2;
		aParam.delta_v_c = sqrt(pow(aParam.delta_v,2)+pow(p->delta_v,2))/4;



		if(interSeg.size() == 1){
			aParam.l_c = -1.0;
			
		}
		if(interSeg.size() > 1){
			//DEBUG_MESSAGE("inter seg size"<<interSeg.size());
			interSeg.pop_back();
			interSeg.pop_front();
		
			aParam.l_c = 0.0;
			for(list<patArc*>::iterator iter2 = interSeg.begin();
				iter2 != interSeg.end();
				++iter2){
				aParam.l_c += (*iter2)->getLength();
			}
		}
		(*lp)[arcIter->first] = aParam;
		//DEBUG_MESSAGE("inner set ok 1"); 
	}
	
	waiting = dealIntersections();
	
	//DEBUG_MESSAGE("param set ok"); 

}	

patReal patPathProbaAlgoV2::calPointArcProba_V4(patULong j,pair<patArc*,patReal> arcDDR){
	
	struct arc_params currParams;
	map<patArc*, struct arc_params> prevParams;
	setParams_V4(j,arcDDR.first,&currParams, &prevParams);
     //DEBUG_MESSAGE("OK1");
   
	patReal res,err;
	patReal rtnValue = 0.0;
	
	for(map<patArc*, struct arc_params>::iterator prevArcIter = prevParams.begin();
				prevArcIter != prevParams.end();
				++prevArcIter){
		
		if(pointArcProba[j-1].find(prevArcIter->first)==pointArcProba[j-1].end()
				||pointArcProba[j-1][prevArcIter->first]==0.0){
				
				continue;
		
		}
		//patReal DDRShare= pointArcProba[j-1][prevArcIter->first]/proba[j-1];
		//patReal DDRShare= 1.0;
		patReal DDRShare= pointArcProba[j-1][prevArcIter->first]\
			/(proba[j-1] * gpsSequence->at(j-1).getArcDDRValue(prevArcIter->first, baseNetwork));
		patReal probaConst;
				struct base_f_param_4 params;
			params.a_param = currParams;
			params.b_param = prevArcIter->second;

		if(prevArcIter->second.l_c == 0.0 || prevArcIter->second.l_c < 0){
			const gsl_rng_type *T;
			gsl_rng *r;
			probaConst = 1/(2 * pi * params.b_param.delta_v  * params.a_param.delta_v);
			
			gsl_monte_function G = { &baseFunction_V4_3, 3, &params };
				
			patReal xl[3]={	0.0,
					0.0,
					0.0,
				};
			patReal xu[3]={	1.0,	
					5 * params.a_param.l / params.a_param.mu_v,
				1.0,
				};
			//DEBUG_MESSAGE("l_b"<<params.b_param.l);	
			for(int id=0;id<3;id++){
				if(xl[id]>=xu[id]){
				DEBUG_MESSAGE("edrror bound "<<id<<":"<<xl[id]<<","<<xu[id]);
				
				}
			}
			size_t calls = 500000;
     
			gsl_rng_env_setup ();
			//DEBUG_MESSAGE("setup o k");

			T = gsl_rng_default;
			r = gsl_rng_alloc (T);
			{
				//DEBUG_MESSAGE("start integration, previous arcs"<<params.prev_arcs.size());
				gsl_monte_miser_state *s = gsl_monte_miser_alloc (3);
				gsl_monte_miser_integrate (&G, xl, xu, 3, calls, r, s,
                                    &res, &err);
				gsl_monte_miser_free (s);
				//DEBUG_MESSAGE("1 integration result:"<<res<<","<<err);
				//display_results ("miser", res, err);
			}
		}
		else{
			const gsl_rng_type *T;
			gsl_rng *r;


			probaConst = 1/(sqrt(pow(2 * pi,3)) * params.b_param.delta_v  * params.b_param.delta_v_c * params.a_param.delta_v);
			
			gsl_monte_function G = { &baseFunction_V4_4, 4, &params };
			
			
			patReal xl[4]={	0.0,
					0.0,
					0.0,
					0.0,
				};
			patReal xu[4]={	1.0,	
					5 * params.a_param.l /params.a_param.mu_v,
					1.0,
					60,
				};
			size_t calls = 500000;
     
			gsl_rng_env_setup ();
			//DEBUG_MESSAGE("setup o k");

			T = gsl_rng_default;
			r = gsl_rng_alloc (T);
			{
				//DEBUG_MESSAGE("start integration, previous arcs"<<params.prev_arcs.size());
				gsl_monte_miser_state *s = gsl_monte_miser_alloc (4);
				gsl_monte_miser_integrate (&G, xl, xu, 4, calls, r, s,
                                    &res, &err);
				gsl_monte_miser_free (s);
				//DEBUG_MESSAGE("2 integration result:"<<res<<","<<err);
				//display_results ("miser", res, err);
			}
		}
		
	DEBUG_MESSAGE("l_c,res,probaConst,ddrshare:"<<prevArcIter->second.l_c<<","<<res<<","<<probaConst<<","<<DDRShare<<","<<res * probaConst * DDRShare);
	    
	 rtnValue += res * probaConst * DDRShare;	
	   
	}
	DEBUG_MESSAGE("value"<<rtnValue);
	//rtnValue *= timeParams[arcDDR.first].arc_int;
	
	return rtnValue;
	
}
patReal patPathProbaAlgoV2::calPointArcProba_raw(patULong j,pair<patArc*,patReal> arcDDR){
	
	struct arc_params currParams;
	map<patArc*, struct arc_params> prevParams;
	setParams_V4(j,arcDDR.first,&currParams, &prevParams);
     //DEBUG_MESSAGE("OK1");
   
	patReal res,err;
	patReal rtnValue = 0.0;
	
	for(map<patArc*, struct arc_params>::iterator prevArcIter = prevParams.begin();
				prevArcIter != prevParams.end();
				++prevArcIter){
		
		if(pointArcProba[j-1].find(prevArcIter->first)==pointArcProba[j-1].end()
				||pointArcProba[j-1][prevArcIter->first]==0.0){
				
				continue;
		
		}
		//patReal DDRShare= pointArcProba[j-1][prevArcIter->first]/proba[j-1];
		//patReal DDRShare= 1.0;
		patReal DDRShare= pointArcProba[j-1][prevArcIter->first]\
			/(proba[j-1] * gpsSequence->at(j-1).getArcDDRValue(prevArcIter->first, baseNetwork));
		patReal probaConst;
				struct base_f_param_4 params;
			params.a_param = currParams;
			params.b_param = prevArcIter->second;

		if(prevArcIter->second.l_c == 0.0 || prevArcIter->second.l_c < 0){
			const gsl_rng_type *T;
			gsl_rng *r;
			probaConst = 1/(2 * pi * params.b_param.delta_v  * params.a_param.delta_v);
			
			gsl_monte_function G = { &baseFunction_V4_3, 3, &params };
				
			patReal xl[3]={	0.0,
					0.0,
					0.0,
				};
			patReal xu[3]={	1.0,	
					5 * params.a_param.l / params.a_param.mu_v,
				1.0,
				};
			//DEBUG_MESSAGE("l_b"<<params.b_param.l);	
			for(int id=0;id<3;id++){
				if(xl[id]>=xu[id]){
				DEBUG_MESSAGE("edrror bound "<<id<<":"<<xl[id]<<","<<xu[id]);
				
				}
			}
			size_t calls = 500000;
     
			gsl_rng_env_setup ();
			//DEBUG_MESSAGE("setup o k");

			T = gsl_rng_default;
			r = gsl_rng_alloc (T);
			{
				//DEBUG_MESSAGE("start integration, previous arcs"<<params.prev_arcs.size());
				gsl_monte_miser_state *s = gsl_monte_miser_alloc (3);
				gsl_monte_miser_integrate (&G, xl, xu, 3, calls, r, s,
                                    &res, &err);
				gsl_monte_miser_free (s);
				//DEBUG_MESSAGE("1 integration result:"<<res<<","<<err);
				//display_results ("miser", res, err);
			}
		}
		else{
			const gsl_rng_type *T;
			gsl_rng *r;


			probaConst = 1/(sqrt(pow(2 * pi,3)) * params.b_param.delta_v  * params.b_param.delta_v_c * params.a_param.delta_v);
			
			gsl_monte_function G = { &baseFunction_V4_4, 4, &params };
			
			
			patReal xl[4]={	0.0,
					0.0,
					0.0,
					0.0,
				};
			patReal xu[4]={	1.0,	
					5 * params.a_param.l /params.a_param.mu_v,
					1.0,
					60,
				};
			size_t calls = 500000;
     
			gsl_rng_env_setup ();
			//DEBUG_MESSAGE("setup o k");

			T = gsl_rng_default;
			r = gsl_rng_alloc (T);
			{
				//DEBUG_MESSAGE("start integration, previous arcs"<<params.prev_arcs.size());
				gsl_monte_miser_state *s = gsl_monte_miser_alloc (4);
				gsl_monte_miser_integrate (&G, xl, xu, 4, calls, r, s,
                                    &res, &err);
				gsl_monte_miser_free (s);
				//DEBUG_MESSAGE("2 integration result:"<<res<<","<<err);
				//display_results ("miser", res, err);
			}
		}
		
	DEBUG_MESSAGE("l_c,res,probaConst,ddrshare:"<<prevArcIter->second.l_c<<","<<res<<","<<probaConst<<","<<DDRShare<<","<<res * probaConst * DDRShare);
	    
	 rtnValue += res * probaConst * DDRShare;	
	   
	}
	DEBUG_MESSAGE("value"<<rtnValue);
	//rtnValue *= timeParams[arcDDR.first].arc_int;
	
	return rtnValue;
	
}

list<patReal> patPathProbaAlgoV2::dealIntersections(){
	list<patReal> waitings;
	list<patArc*>* listOfArcs = path->getArcList();
	if (listOfArcs->size()<2){
		return waitings;
	}
	patArc* prevArc = listOfArcs->front();
	list<patAr*>::iterator arcIter = listOfArcs->begin();
	arcIter++;
	for(;
				arcIter!=listOfArcs->end();
				arcIter++){
		waitings.insert(calWaitingTime(prevArc,const_cast<patArc*>(*arcIter)));
		prevArc = const_cast<patArc*>(*arcIter);
	}
	
	return waitings;
}
patReal patPathProbaAlgoV2::calWaitingTime(patArc* upArc, patArc* downArc){
	
	if(upArc->m_down_node_id != downArc->m_up_node_id){
	    stringstream str ;
		
		patError* err;
    str << (*upArc)<<" and " << (*downArc)<<" are not connected" ;
    err = new patErrMiscError(str.str()) ;
    WARNING(err->describe());

	}
	patNode* intersection = theNetwork->getNodeFromUserId(upArc->m_down_node_id);
	
	if(intersection ==NULL){
		err = new patErrNullPointer("patNode") ;
    WARNING(err->describe()) ;
    return ;
	}
	
	if(intersection->attributes.type = "traffic_signal"){
		return calSignalWaiting(upArc, downArc,intersection);
	}
}

patReal patPathProbaAlgoV2::calSignalWaiting(patArc* upArc,patArc* downArc,patNode* intersection){
	set<patULong>* successors = &(intersection.userSuccesors);
	map<patArc*, patReal> downHeading;
	patULong left=0;
	patULong right=1;
	patReal headingChange = downArc->m_attributes.heading - upArc->m_attributes.heading;
	headingChange = (heading>360.0)?(heading-360.0):heading;
	headingChange = (heading<0.0)?(heading+360.0):heading;
		
	for(set<patULong>::iterator iter1 = successors->begin();
			iter1 != successors->end();
			++iter1){
		
		patArc* downStream = theNetwork->getArcFromNodeUserIds(intersection->userId, *iter1);
		if(downStream == NULL)
		{
			continue;
		}
		patReal h1 = 180.0 - (downStream->m_attributes.heading-upArc->m_attributes.heading);
		h1 = (heading>360.0)?(heading-360.0):heading;
		headingChange = (heading<0.0)?(heading+360.0):heading;
		
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
		return 5;
	}
	if (left == 0 && right ==1){
		return 20;
	} 
	if(left == 0 && right >1){
		return 30;
	}
	if(left>0 && right >0)
	{
		return 20;
	}
}


