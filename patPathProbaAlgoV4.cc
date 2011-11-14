#include "patConst.h"
#include "patPathJ.h"
#include "patNetwork.h"
#include "patArc.h"
#include "patGpsPoint.h"
#include "patPathProbaAlgoV4.h" 
#include "patTripGraph.h" 
#include "patDisplay.h"
#include "patError.h"
#include "patErrMiscError.h"
#include "patArcTransition.h"
#include "patNBParameters.h"
#include "patPower.h"

/*
*
*/

patPathProbaAlgoV4::patPathProbaAlgoV4(patPathJ* thePath,
 vector<patGpsPoint*>* theGpsSequence,
 patNetwork* theNetwork,
 set<patArcTransition>* theArcTranSet,
vector<patGpsPoint>* theOriginalGpsPoints):
	gpsSequence(theGpsSequence), 
	baseNetwork(theNetwork),
	path(thePath),
	proba(theGpsSequence->size(),0),
	pointArcProba(theGpsSequence->size(),map<patArc*,patReal>()),
	arcTranSet(theArcTranSet),
	originalGpsPoints(theOriginalGpsPoints)
{
	//DEBUG_MESSAGE("Initiate path probability calculation");
}

void patPathProbaAlgoV4::initArc(){
//	DEBUG_MESSAGE("INIT");
	proba[0]=1.0;
	pointArcProba[0][path->front()] = 1.0;	
	proba[0] = calPointProba("raw",0);
	
	//DEBUG_MESSAGE("poit proba 0"<<proba[0]);
}
patReal patPathProbaAlgoV4::run_raw(){
	patReal probaPath = 1.0;
	for(patULong j = 0;
		j < gpsSequence->size();
		++j){

		calPointProba("raw",j);
		if(proba[j] == 0.0){
			DEBUG_MESSAGE("zero value at"<<j);
			return 0.0;
		}
//		DEBUG_MESSAGE("point proba "<<j<<":"<<proba[j]);
	}
	
	for(patULong j=0;j<proba.size();j++){
		probaPath  *= proba[j];
	}
//DEBUG_MESSAGE("raw proba "<<probaPath);
	return probaPath;
}

patReal patPathProbaAlgoV4::run_MapMatching(){
	patReal value = 0.0;
	patULong j = 0;
	for(;
		j < gpsSequence->size();
		++j){

		value += pow(calDistance(j),2);
		
	}
	return value;
}

patReal patPathProbaAlgoV4::calMapMatchingPoint(patULong j){

	patReal rtnValue = 0.0;
	map<patArc*, patReal>* theLinkDDR = gpsSequence->at(j)->getLinkDDR();
	patULong k = 0;
	for(map<patArc*, patReal>::iterator arcIter = theLinkDDR->begin();
						arcIter!=theLinkDDR->end();
						++arcIter){
		if(path->isLinkInPath(arcIter->first)){

		map<char*,patReal> distanceArc = gpsSequence->at(j)->distanceTo(baseNetwork, arcIter->first);
		if(distanceArc["link"] <= distanceArc["ver"] / sin(60.0) ){
				
			rtnValue +=  distanceArc["link"];
		}
		}
	}	
	
	return rtnValue;
}

patReal patPathProbaAlgoV4::calDistance(patULong j){

	list<patArc*>* arcList=path->getArcList();
	patReal dist=1000000;
	for (list<patArc*>::iterator arcIter=arcList->begin(); arcIter!=arcList->end(); ++arcIter) {
		map<char*,patReal> distanceArc = gpsSequence->at(j)->distanceTo(baseNetwork, *arcIter);
		
		dist=dist>distanceArc["link"]?distanceArc["link"]:dist;
		
	}
	//DEBUG_MESSAGE("GPS point "<<j<<", distance:"<<dist);
	return dist;
}

patReal patPathProbaAlgoV4::run_dist(){
	patReal pathDistance = 0.0;
	patULong j = 0;
	for(;
		j < gpsSequence->size();
		++j){

		pathDistance += calDistance(j);
		
	}
	

	return pathDistance/(j+1);
}


patReal patPathProbaAlgoV4::run(patString type){
	
	if(type == "raw"){
		return run_raw();
	}
	else if(type=="dist"){
		return run_dist();
	}
	else if(type=="mm"){
		return run_MapMatching();
	}
	else{
		return run_ts(type);
	}
	return 0.0;
	
}

patReal patPathProbaAlgoV4::firstPointProba(patString algoType){
	proba[0]=0.0;
	map<patArc*, patReal>* theLinkDDR = gpsSequence->at(0)->getLinkDDR();
	
	for(map<patArc*, patReal>::iterator arcIter = theLinkDDR->begin();
		arcIter != theLinkDDR->end();
		++arcIter){
		pointArcProba[0][arcIter->first] = 0.0;
		if(path->isLinkInPath(arcIter->first)){
			proba[0]+=arcIter->second;
		}
	}
	if(proba[0]/path->getPathLength()>1.0){
		
		DEBUG_MESSAGE(proba[0]/path->getPathLength()<<"="<<proba[0]<<"/"<<path->getPathLength());
	}
	proba[0]=proba[0]/path->getPathLength();
	return proba[0];
	
}

patReal patPathProbaAlgoV4::run_ts(patString algoType){
	/*to do*/
	//patReal probaPath = 1.0 * gpsSequence->front()->getArcDDRValue(path->front(),baseNetwork)/gpsSequence->at(0)->getDDRSum();
	//probaPath = 1.0;
	
	patReal probaPath=1.0;
	firstPointProba(algoType);
	if(probaPath==0.0){
		return 0.0;
	}
	//DEBUG_MESSAGE(gpsSequence->size());
	for(patULong j = 1;
		j < gpsSequence->size();
		++j){
	//	DEBUG_MESSAGE("point "<<j);
		calPointProba(algoType,j);
		if(proba[j] == 0.0){
			DEBUG_MESSAGE("algo:"<<algoType<<"zero probability at ["<<(j+1)<<"]"<<gpsSequence->at(j)->getTimeStamp()<<" total gps:"<<gpsSequence->size());
			return 0.0;
		}
	}
	for(patULong j=0;j<proba.size();j++){
		//DEBUG_MESSAGE("point "<<j+1<<": "<<proba[j])
		if (proba[j]>1.0){
			DEBUG_MESSAGE("point "<<j+1<<" probability greater than 1.0: "<<proba[j])
		}

		probaPath  *= proba[j];
	}
	DEBUG_MESSAGE("path proba"<<probaPath);
	
	return probaPath;
}


patReal patPathProbaAlgoV4::calPointProba(patString algoType,patULong j){
	patString at = algoType;
	if(algoType=="ts"&&
		( j == 0 ||gpsSequence->at(j)->getType()!="normal_speed" ||
			(j>0 && gpsSequence->at(j-1)->getType()!="normal_speed")
		)
	){
		at = patNBParameters::the()->lowSpeedAlgo;
		
	}
	proba[j]=0.0;
	map<patArc*, patReal>* theLinkDDR = gpsSequence->at(j)->getLinkDDR();
	patReal backLength = 0.0;
	for(map<patArc*, patReal>::iterator arcIter = theLinkDDR->begin();
						arcIter != theLinkDDR->end();
						++arcIter){
		pointArcProba[j][arcIter->first] = 0.0;
		if(path->isLinkInPath(arcIter->first)){
			pointArcProba[j][arcIter->first] = calPointArcProba(at,j,pair<patArc*,patReal>(arcIter->first,arcIter->second));
			
			proba[j]+=pointArcProba[j][arcIter->first];	
		
		//		backLength+=arcIter->first->getLength();	
		}
	}
	
	if (j>0){
		DEBUG_MESSAGE(proba[j]<<","<<getPointSimpleDDR(j-1));
		proba[j]=proba[j]/getPointSimpleDDR(j-1);//normalization
		DEBUG_MESSAGE(j+1<<","<<proba[j]);
	}
	//if(backLength>0.0){
	//	proba[j] /= patPower(backLength/1000, patNBParameters::the()->pointProbaPower);
	//}
		
	return proba[j] ;
}

patReal patPathProbaAlgoV4::calPathProba(){
	patReal pathProba = initArcProba;
	
	for(int i = 0; i<proba.size();++i){
		pathProba+=log(proba[i]);
	}
	
	return pathProba;
}

patReal patPathProbaAlgoV4::getPointSimpleDDR(patULong g){
	
	patReal pointDDRSum=0.0;
	map<patArc*,patReal>* prevLinkDDR = gpsSequence->at(g)->getLinkDDR();
	for(map<patArc*,patReal>::iterator arcIter = prevLinkDDR->begin(); 
		arcIter != prevLinkDDR->end();
		++arcIter){ 
		if (path->isLinkInPath(arcIter->first)){
			pointDDRSum+=arcIter->first->getLength() * arcIter->second;
		}
	}	
	//DEBUG_MESSAGE(pointDDRSum);
	if (pointDDRSum==0.0){
		DEBUG_MESSAGE("zero point ddr sum at point"<<g);
	}
	return pointDDRSum;
}
patReal patPathProbaAlgoV4::calPointArcProba(patString algoType,patULong j,pair<patArc*,patReal> arcDDR){
	
	if(algoType == "raw" || j==0){
		DEBUG_MESSAGE("arc ddr:"<<arcDDR.first<<","<<arcDDR.second);
		
		return arcDDR.second/path->getPathLength();
	}
	patReal rtnValue=0.0;
	struct gps_params GP;
	GP.time_diff = -1.0;
	map<patArc*,patReal>* prevLinkDDR = gpsSequence->at(j-1)->getLinkDDR();
	for(map<patArc*,patReal>::iterator arcIter = prevLinkDDR->begin(); 
					arcIter != prevLinkDDR->end();
					++arcIter){ 

		list<pair<patArc*, TransportMode> >  interSeg = path->getSeg(arcIter->first,arcDDR.first);
		if(interSeg.empty()){
			continue;
		}
		patArcTransition theTran(interSeg,
				gpsSequence->at(j-1),
				gpsSequence->at(j)
				);
		patReal tranValue;
		if(GP.time_diff <= 0.0){
				gpsSequence->at(j)->setGpsParams(&GP, gpsSequence->at(j-1), originalGpsPoints);
			}
		
		set<patArcTransition>::iterator found = arcTranSet->find(theTran);
		if(found !=  arcTranSet->end()){
			tranValue = const_cast<patArcTransition*>(&(*found))->getValue(algoType,baseNetwork, GP);
		}
		else{
			
			 tranValue = theTran.getValue(algoType,baseNetwork, GP);
			arcTranSet->insert(theTran);
			
		}
		
			rtnValue += tranValue;

	}
//	DEBUG_MESSAGE(rtnValue);
	return rtnValue;
}

