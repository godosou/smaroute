/*
 * patMultiModalPathProba.cc
 *
 *  Created on: Aug 5, 2011
 *      Author: jchen
 */

#include "patMultiModalPathProba.h"
#include "patDisplay.h"
#include "patGpsPoint.h"
patMultiModalPathProba::patMultiModalPathProba() {
	// TODO Auto-generated constructor stub

}

patMultiModalPathProba::~patMultiModalPathProba() {
	// TODO Auto-generated destructor stub
}


patReal patMultiModalPathProba::firstPointProba(){
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
patReal patMultiModalPathProba::run(){
	patReal probaPath=1.0;
	firstPointProba(algoType);
	if(probaPath==0.0){
		return 0.0;
	}
	for(patULong k = 1;
		k < gpsSequence->size();
		++k){
		//DEBUG_MESSAGE("point "<<j);
		calPointProba(k);
		if(proba[k] == 0.0){
			DEBUG_MESSAGE("algo:"<<algoType<<"zero probability at ["<<(k+1)<<"]"
					<<gpsSequence->at(k)->getTimeStamp()<<" total gps:"<<gpsSequence->size());
			return 0.0;
		}
	}
	for(patULong k=0;k<proba.size();k++){
		//DEBUG_MESSAGE("point "<<j+1<<": "<<proba[j])
		if (proba[k]>1.0){
			DEBUG_MESSAGE("point "<<k+1<<" probability greater than 1.0: "<<proba[k])
		}

		probaPath  *= proba[j];
	}
	//DEBUG_MESSAGE("path proba"<<probaPath);

	return probaPath;
}



patReal patMultiModalPathProba::calPointProba(short k){
	if (k==0){
		WARNING("wrong parameter:"<<k);
		return -1.0;
	}

	/*
	if( gpsSequence->at(j-1)->getType()!="normal_speed")
	{
		at = patNBParameters::the()->lowSpeedAlgo;
	}
	*/

	proba[k]=0.0;
	map<patArc*, patReal>* theLinkDDR = gpsSequence->at(k)->getLinkDDR();
	patReal backLength = 0.0;
	for(map<patArc*, patReal>::iterator arcIter = theLinkDDR->begin();
						arcIter != theLinkDDR->end();
						++arcIter){
		pointArcProba[k][arcIter->first] = 0.0;
		if(path->isLinkInPath(arcIter->first)){
			pointArcProba[k][arcIter->first] = calPointArcProba(k,pair<patArc*,patReal>(arcIter->first,arcIter->second));

			proba[k]+=pointArcProba[k][arcIter->first];
		}
	}


	proba[k]=proba[k]/getPointSimpleDDR(k-1);


	return proba[k];
}

patReal patMultiModalPathProba::getPointSimpleDDR(short k){

	patReal pointDDRSum=0.0;
	map<patArc*,patReal>* prevLinkDDR = gpsSequence->at(k)->getLinkDDR();
	for(map<patArc*,patReal>::iterator arcIter = prevLinkDDR->begin();
		arcIter != prevLinkDDR->end();
		++arcIter){
		if (path->isLinkInPath(arcIter->first)){
			pointDDRSum+=arcIter->first->getLength() * arcIter->second;
		}
	}
	if (pointDDRSum==0.0){
		DEBUG_MESSAGE("zero point ddr sum at point"<<k);
	}
	return pointDDRSum;
}

patReal patMultiModalPathProba::calPointArcProba(short k,pair<patArc*,patReal> arc_DDR){

	/*
	 *This part is for low speed
	if(algoType == "raw" || j==0){
		DEBUG_MESSAGE("arc ddr:"<<arcDDR.first<<","<<arcDDR.second);

		return arcDDR.second/path->getPathLength();
	}
	 */

	patReal rtnValue=0.0;
	struct gps_params GP;
	GP.time_diff = -1.0;
	map<patArc*,patReal>* prevLinkDDR = gpsSequence->at(k-1)->getLinkDDR();
	for(map<patArc*,patReal>::iterator arcIter = prevLinkDDR->begin();
					arcIter != prevLinkDDR->end();
					++arcIter){
		list<pair<patArc*, TransportMode> > interSeg= path->getMultimodalSeg(arcIter->first,arc_DDR.first);

		if(interSeg.empty()){
			continue;
		}
		patArcTransition theTran(interSeg,
				gpsSequence->at(k-1),
				gpsSequence->at(k)
				);
		patReal tranValue;
		if(GP.time_diff <= 0.0){
				gpsSequence->at(k)->setGpsParams(&GP, gpsSequence->at(k-1), originalGpsPoints);
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
