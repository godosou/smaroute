#include "patDisplay.h"
#include "patNode.h"
#include "patArc.h"
#include "patTripGraph.h"
#include "patNetwork.h" 
#include "patGpsPoint.h"
#include "patPathJ.h"
#include "patNode.h"
#include "patArc.h"
#include "patShortestPathAlgoDest.h"
#include "patShortestPathTreeDest.h"
#include "patType.h"
#include "patConst.h"
#include "patTripGraph.h"
#include "patDisplay.h"
#include "patPathDDR.h" 
#include "patArcTransition.h"
#include "patPathProbaAlgoV4.h"
#include "patNBParameters.h"

patTripGraph::patTripGraph():
	baseNetwork(NULL),
	gpsSequence(NULL),
	assocDomain(NULL)
{

}

patTripGraph::patTripGraph(patNetwork* theNetwork):
	baseNetwork(theNetwork),
	gpsSequence(NULL),
	assocDomain(NULL)
{

}

void patTripGraph::setOrigin(set<patNode*> origNodeSet){
	origSet = origNodeSet;
}

void patTripGraph::setSuccessor(patArc* upArc,patArc* downArc){
	if(successor.find(upArc)==successor.end()){
		successor[upArc]=set<patArc*>();
	}
	
	successor[upArc].insert(downArc);
	
}


map<patReal, set<patULong> > patTripGraph::sortOrig(vector<patGpsPoint>* theGpsSequence){
map<patArc*, patReal>* firstDDR = theGpsSequence->at(0).getLinkDDR();
	map<patULong, patReal> origDDRSum;
	for(map<patArc*, patReal>::iterator arcDDR = firstDDR->begin();
			arcDDR!=firstDDR->end();
			++arcDDR){
		
		if(origDDRSum.find(arcDDR->first->upNodeId)==origDDRSum.end()){
			origDDRSum[arcDDR->first->upNodeId]=0.0;
		}
		origDDRSum[arcDDR->first->upNodeId]+=arcDDR->second;
	}
	
	map<patReal,set<patULong> > ddrOrig;
	for(map<patULong,patReal>::iterator origIter=origDDRSum.begin();
			origIter!=origDDRSum.end();
			++origIter){
			
		if(ddrOrig.find(origIter->second)==ddrOrig.end()){
			ddrOrig[origIter->second]= set<patULong>();
		}
		
		ddrOrig[origIter->second].insert(origIter->first);
	
	}
	
	return ddrOrig;
}
map<patReal, set<patULong> > patTripGraph::sortDest(vector<patGpsPoint>* theGpsSequence){
	
	map<patArc*, patReal>* lastDDR = theGpsSequence->back().getLinkDDR();
	map<patULong, patReal> destDDRSum;
	for(map<patArc*, patReal>::iterator arcDDR = lastDDR->begin();
			arcDDR!=lastDDR->end();
			++arcDDR){
		
		if(destDDRSum.find(arcDDR->first->downNodeId)==destDDRSum.end()){
			destDDRSum[arcDDR->first->downNodeId]=0.0;
		}
		destDDRSum[arcDDR->first->downNodeId]+=arcDDR->second;
	}
	
	map<patReal,set<patULong> > ddrDest;
	for(map<patULong,patReal>::iterator destIter=destDDRSum.begin();
			destIter!=destDDRSum.end();
			++destIter){
			
		if(ddrDest.find(destIter->second)==ddrDest.end()){
			ddrDest[destIter->second]= set<patULong>();
		}
		
		ddrDest[destIter->second].insert(destIter->first);
	
	}
	
	return ddrDest;

}
void patTripGraph::genOrigSet_SortDDR(vector<patGpsPoint>* theGpsSequence,patULong k){
	
	map<patReal,set<patULong> > ddrOrig = sortOrig(theGpsSequence);
	map<patReal,set<patULong> >::iterator origIter = ddrOrig.end();
	
	
	for(int i=0;i<k;++i){
		origIter--;
			for(set<patULong>::iterator nodeIter = origIter->second.begin();
			nodeIter != origIter->second.end();
			++nodeIter){
				origSet.insert(baseNetwork->getNodeFromUserId(*nodeIter));
			}
		if(origIter==ddrOrig.begin()){
			break;
		}
	}
	
}
void patTripGraph::genOrigSet_Bound(vector<patGpsPoint>* theGpsSequence,patReal lowerBound, patReal upperBound){
	map<patReal,set<patULong> > ddrOrig = sortOrig( theGpsSequence);
	for(map<patReal,set<patULong> >::iterator origIter = ddrOrig.begin();
				origIter != ddrOrig.end();
				++origIter){
				
		if((origIter->first>=lowerBound && origIter->first <=upperBound)||
			(origIter->first>=39 && origIter->first <=40)
		){
			for(set<patULong>::iterator nodeIter = origIter->second.begin();
			nodeIter != origIter->second.end();
			++nodeIter){
				origSet.insert(baseNetwork->getNodeFromUserId(*nodeIter));
			}
		}
	}

}
void patTripGraph::genOrigSet_All(vector<patGpsPoint>* theGpsSequence){
		map<patArc*, patReal>* firstDDR = theGpsSequence->at(0).getLinkDDR();
	for(map<patArc*, patReal>::iterator arcDDR = firstDDR->begin();
			arcDDR!=firstDDR->end();
			++arcDDR){
			origSet.insert(baseNetwork->getNodeFromUserId(arcDDR->first->upNodeId));
}
}
void patTripGraph::genOrigSet_Ids(vector<patULong> origIds){

	for(vector<patULong>::iterator idIter = origIds.begin();
			idIter!=origIds.end();
			++idIter){
			patNode* orig = baseNetwork->getNodeFromUserId(*idIter);
			if (orig !=NULL){
				origSet.insert(orig);
			
			}
	}
}
set<patPathJ> patTripGraph::genPaths(vector<patGpsPoint>* theGpsSequence,
	map<patArc*,set<patULong> >* theAssocDomain){
	
	writeGraph();
	assocDomain=theAssocDomain;
	gpsSequence=theGpsSequence;
	calLengthCeil();
	map<patArc*,patReal>* initLinkDDR = gpsSequence->at(0).getLinkDDR();
	//genOrigSet_SortDDR(theGpsSequence,1);
	/*
	vector<patULong> origIds;
	origIds.push_back(patULong(1));
	origIds.push_back(patULong(2));
	origIds.push_back(patULong(258755905));
	origIds.push_back(patULong(258755907));
		origIds.push_back(patULong(256188131));
		origIds.push_back(patULong(268646683));
		origIds.push_back(patULong(282017112));
				origIds.push_back(patULong(252684258));
*/

	
	
	//genOrigSet_Ids(origIds);
	genOrigSet_All(theGpsSequence);
	//genOrigSet_Bound(theGpsSequence,20,150);
	//DEBUG_MESSAGE("origin nodes:"<<origSet.size());
	for(map<patArc*,patReal>::iterator iter1 = initLinkDDR->begin();
				iter1!=initLinkDDR->end();
				++iter1){
		for(set<patNode*>::iterator iter2 = origSet.begin();
						iter2!=origSet.end();
						++iter2){
						
			if(iter1->first->upNodeId == (*iter2)->userId){
				//DEBUG_MESSAGE("generate path from"<<*baseNetwork->getNodeFromUserId(iter1->first->upNodeId));
				list<patArc*> pathTemp;
				set<patULong> possibleDomain;
				possibleDomain.insert(0);
				connectArcs(iter1->first,&pathTemp,possibleDomain);
			}
		
		}
		
	}
	
	return pathSet;
}


patBoolean patTripGraph::isVisited(patULong nodeUserId,list<patArc*>* pathTemp){
	if(pathTemp->empty()==true){
		return patFALSE;
	}
	patArc* backArc = pathTemp->back();
	pathTemp->pop_back();
	
	patBoolean rtnValue = patFALSE;
	for(list<patArc*>::iterator arcIter = pathTemp->begin();
						arcIter!=pathTemp->end();
						++arcIter){
		
		if(nodeUserId == (*arcIter)->upNodeId ){
			rtnValue = patTRUE;
			break;
		}
	
	}
	pathTemp->push_back(backArc);
	return rtnValue;
}

patReal patTripGraph::calAverageSpeed(){
	patReal avegSpeed = 0.0;
	patReal count = 0;
	for(patULong i = 0; i<gpsSequence->size();++i){
		patReal as = gpsSequence->at(i).getSpeedMS();
		if(as>=1.0){
		
		avegSpeed += gpsSequence->at(i).getSpeedMS();
		count++;
		}
	}
	
	avegSpeed /= count;
	//DEBUG_MESSAGE("average speed"<<avegSpeed/3.6);
	return avegSpeed;
}

patReal patTripGraph::calLengthCeil(){
	lengthCeil = patNBParameters::the()->pathLengthCeil * calAverageSpeed() * (gpsSequence->back().getTimeStamp()-gpsSequence->front().getTimeStamp());
}

void patTripGraph::connectArcs(patArc* theArc,list<patArc*>* pathTemp,set<patULong> possibleDomain) {
	pathTemp->push_back(theArc);
	//DEBUG_MESSAGE("arc"<<*theArc);
	//detect loop
	if(isVisited(theArc->downNodeId,pathTemp)==patTRUE){
			 //DEBUG_MESSAGE("a loop"<<pathTemp);					
			return;
	}
	patPathJ thePath(*pathTemp);
	if(thePath.computePathLength() > lengthCeil){
	
		//DEBUG_MESSAGE("path too long"<<thePath.computePathLength()<<","<<lengthCeil);
		return ;
	}
	//if in the domain of last gps point record a path		
	map<patArc*,patReal>* backGpsDDR = gpsSequence->back().getLinkDDR();
	if(backGpsDDR->find(theArc)!=backGpsDDR->end()){
		//DEBUG_MESSAGE("record a path");
		recordPath(pathTemp);
	}
	
	//if no succesor, return 
	else if(successor.find(theArc)==successor.end()){
		//DEBUG_MESSAGE("no successor at:"<<*theArc);
		return;

	}
	
	//set the next possible domain
	set<patULong> nextPossibleDomain;
	//if(assocDomain->find(theArc)==assocDomain->end()){
		//DEBUG_MESSAGE("no gps correlated");
		nextPossibleDomain = possibleDomain;
	//}
	
	//DEBUG_MESSAGE("determin the next possible domain");

		for(set<patULong>::iterator iter = (*assocDomain)[theArc].begin();
					iter!=(*assocDomain)[theArc].end();
					iter++
		){
			if(possibleDomain.find(*iter)!=possibleDomain.end()){
				nextPossibleDomain.insert(*iter);
				nextPossibleDomain.insert((*iter)+1);
				
			}
		}
	
	 
	
	//if there is overlap between possible domain and current domain
	//go to next arc
	if(nextPossibleDomain.empty()==false){
		//domain for next arc can be one gps point after
		//DEBUG_MESSAGE("domain +1");
		
		//loop through successors
		//DEBUG_MESSAGE("loop through succesor");
		for(set<patArc*>::iterator arcIter = successor[theArc].begin();
			arcIter != successor[theArc].end();
			arcIter++){
			

			//pathTemp->push_back(const_cast<patArc*>(*arcIter));
			connectArcs(const_cast<patArc*>(*arcIter),pathTemp,nextPossibleDomain);
			pathTemp->pop_back();
		}
	}
	else{
		DEBUG_MESSAGE("possible domain is empty");
	}
} 

set<patULong> calNextPossibleDomain(set<patULong> prevDomain,set<patULong> currDomain){
	

}
void patTripGraph::recordPath(list<patArc*>* pathTemp){
	patPathJ thePath(*pathTemp);
	if(pathSet.find(thePath)!=pathSet.end()){
		DEBUG_MESSAGE("path exists");
		return ;
	}
	if(thePath.isValidPath(gpsSequence)==patTRUE){
	pathSet.insert(thePath);
	
	//DEBUG_MESSAGE("A path recorded."<<thePath);
	//DEBUG_MESSAGE("A path recorded.");
	//DEBUG_MESSAGE("current path size:"<<pathSet.size());
	}
	else{
		//DEBUG_MESSAGE("path not valid");
	}
}


void patTripGraph::writeGraph(){
	ofstream outfile("result/output.out");
	for(map<patArc*,set<patArc*> >::iterator iter1=successor.begin();
			iter1!=successor.end();
			++iter1){
		outfile<<*(iter1->first)<<":"<<endl;
		for(set<patArc*> ::iterator iter2=iter1->second.begin();
			iter2!=iter1->second.end();
			++iter2){	
			outfile<<"\t"<<*(*iter2)<<endl;
		}
	}
}


set<patPathJ> patTripGraph::getPathSet(){
	return pathSet;
}


set<patPathJ> patTripGraph::selectPaths(vector<patGpsPoint>* theGpsSequence,set<patArcTransition>* arcTranSet){

	map<patReal,set<patPathJ*> > ddrPathSet;
	map<patReal,set<patPathJ*> > ddrPathSet_avg;
	map<patReal,set<patPathJ*> > lengthPathSet;
	map<patReal,set<patPathJ*> > distancePathSet;
		vector<patGpsPoint*> gpsPointerSequence;
	for(vector<patGpsPoint>::iterator gIter = theGpsSequence->begin();
				gIter!=theGpsSequence->end();
				++gIter){
		gpsPointerSequence.push_back(const_cast<patGpsPoint*>(&(*gIter)));
	}

	for(set<patPathJ>::iterator pathIter= pathSet.begin();
				pathIter!=pathSet.end();
				++pathIter){
		
		patPathJ* currPath = const_cast<patPathJ*>(&(*pathIter));
		//DEBUG_MESSAGE("read a path");
		//DEBUG_MESSAGE("transition set"<<arcTranSet->size());
		patPathProbaAlgoV4 probaRaw(currPath,&gpsPointerSequence, baseNetwork, arcTranSet,theGpsSequence);
 
		patReal ddrValue = probaRaw.run_raw();
		
		patReal pathLength = currPath->computePathLength();
		patReal ddrValue_avg = ddrValue/pathLength;
		
		patReal distance = probaRaw.run_dist();
		
//		DEBUG_MESSAGE("path value:"<<ddrValue);
		if(ddrPathSet.find(ddrValue)==ddrPathSet.end()){
			ddrPathSet[ddrValue]=set<patPathJ*>();
		}
		
		ddrPathSet[ddrValue].insert(currPath);
		
		if(ddrPathSet_avg.find(ddrValue_avg)==ddrPathSet_avg.end()){
			ddrPathSet_avg[ddrValue_avg]=set<patPathJ*>();
		}
		
		ddrPathSet_avg[ddrValue_avg].insert(currPath);
		
		if(lengthPathSet.find(pathLength)==lengthPathSet.end()){
			lengthPathSet[pathLength]=set<patPathJ*>();
		}
		
		lengthPathSet[pathLength].insert(currPath);
		
		if(distancePathSet.find(distance)==distancePathSet.end()){
			distancePathSet[distance]=set<patPathJ*>();
		}
		
		distancePathSet[distance].insert(currPath);
		
	}
	
	set<patPathJ*> rtnSet;
	if(ddrPathSet.empty()==true){
		return set<patPathJ>();
	}
	map<patReal,set<patPathJ*> >::iterator ddrPathIterBack= ddrPathSet.end();
	map<patReal,set<patPathJ*> >::iterator ddrPathIterBack_avg= ddrPathSet_avg.end();
	map<patReal,set<patPathJ*> >::iterator ddrPathIterFront= ddrPathSet.begin();
	
	map<patReal,set<patPathJ*> >::iterator distanceIterFront= distancePathSet.begin();
	
	//map<patReal,set<patPathJ> >::iterator ddrPathIterFront_avg= ddrPathSet_avg.begin();
	map<patReal,set<patPathJ*> >::iterator lengthPathIterFront= lengthPathSet.begin();

	ddrPathIterBack--;
	ddrPathIterBack_avg--;
	patULong best = patNBParameters::the()->selectBestPaths;
	patULong worst = patNBParameters::the()->selectWorstPaths;
	patULong shortest = patNBParameters::the()->selectShortestPaths;
	
	
	DEBUG_MESSAGE("best: "<<best<<", worst: "<<worst<<", shortest: "<<shortest);
	for(patULong i =0; i<worst;++i){
	
	if(ddrPathIterFront!=ddrPathSet.end()){
			rtnSet.insert(ddrPathIterFront->second.begin(),ddrPathIterFront->second.end());
			ddrPathIterFront++;
		}
		/*
		if(ddrPathIterFront_avg!=ddrPathSet_avg.end()){
			rtnSet.insert(ddrPathIterFront_avg->second.begin(),ddrPathIterFront_avg->second.end());
			ddrPathIterFront_avg++;

		}
		*/
	}
	
	for(patULong i=0;i<best;++i){
		
		rtnSet.insert(ddrPathIterBack->second.begin(),ddrPathIterBack->second.end());
			
		if(ddrPathIterBack!=ddrPathSet.begin()){
			ddrPathIterBack--;
		}
		
		rtnSet.insert(ddrPathIterBack_avg->second.begin(),ddrPathIterBack_avg->second.end());
			
		if(ddrPathIterBack_avg!=ddrPathSet_avg.begin()){
			ddrPathIterBack_avg--;

		}
		
		if(distanceIterFront!=distancePathSet.end()){
			rtnSet.insert(distanceIterFront->second.begin(),distanceIterFront->second.end());
			distanceIterFront++;

		}
		
	}
	
	for(patULong i=0;i<shortest;++i){
		if(lengthPathIterFront!=lengthPathSet.end()){
			rtnSet.insert(lengthPathIterFront->second.begin(),lengthPathIterFront->second.end());
			lengthPathIterFront++;

		}
	}
	DEBUG_MESSAGE("path size"<<rtnSet.size());
	for(set<patPathJ>::iterator pIter = pathSet.begin();
			pIter!=pathSet.end();
			){
			
		if(rtnSet.find(const_cast<patPathJ*>(&(*pIter))) == rtnSet.end()){
			pathSet.erase(pIter++);
		}
		else{
			pIter++;
		}
	}
	return pathSet;
}

/*
set<patPathJ> patTripGraph::genPaths(vector<patGpsPoint>* theGpsSequence,
	map<patArc*,set<patULong> >* theAssocDomain){
	
	writeGraph();
	assocDomain=theAssocDomain;
	gpsSequence=theGpsSequence;
	map<patArc*,patReal>* initLinkDDR = gpsSequence->at(0).getLinkDDR();
	//genOrigSet_SortDDR(theGpsSequence);
		genOrigSet_All(theGpsSequence);

	DEBUG_MESSAGE("origin nodes:"<<origSet.size());
	for(map<patArc*,patReal>::iterator iter1 = initLinkDDR->begin();
				iter1!=initLinkDDR->end();
				++iter1){
		for(set<patNode*>::iterator iter2 = origSet.begin();
						iter2!=origSet.end();
						++iter2){
						
			if(iter1->first->upNodeId == (*iter2)->userId){
				DEBUG_MESSAGE("generate path from"<<*baseNetwork->getNodeFromUserId(iter1->first->upNodeId));
				list<patArc*> pathTemp;
				set<patULong> possibleDomain;
				possibleDomain.insert(0);
				connectArcs(iter1->first,&pathTemp,possibleDomain);
			}
		
		}
		
	}
	
	return pathSet;
}

void patTripGraph::connectArcs(patArc* theArc,list<patArc*>* pathTemp,set<patULong> possibleDomain) {
	//DEBUG_MESSAGE("arc"<<*theArc);
	//detect loop
	if(isVisited(theArc->downNodeId,pathTemp)==patTRUE){
			//DEBUG_MESSAGE("a loop");					
			return;
	}
	//if in the domain of last gps point record a path		
	map<patArc*,patReal>* backGpsDDR = gpsSequence->back().getLinkDDR();
	if(backGpsDDR->find(theArc)!=backGpsDDR->end()){
		//DEBUG_MESSAGE("record a path");
		recordPath(pathTemp);
	}
	
	//if no succesor, return 
	else if(successor.find(theArc)==successor.end()){
		//DEBUG_MESSAGE("no successor");
		return;

	}
	
	//set the next possible domain
	set<patULong> nextPossibleDomain;
	if(assocDomain->find(theArc)==assocDomain->end()){
		//DEBUG_MESSAGE("no gps correlated");
		nextPossibleDomain = possibleDomain;
	}
	else{
		//DEBUG_MESSAGE("determin the next possible domain");

		for(set<patULong>::iterator iter = (*assocDomain)[theArc].begin();
					iter!=(*assocDomain)[theArc].end();
					iter++
		){
			if(possibleDomain.find(*iter)!=possibleDomain.end()){
				nextPossibleDomain.insert(*iter);
				nextPossibleDomain.insert((*iter)+1);
				
			}
		}
	
	}
	
	//if there is overlap between possible domain and current domain
	//go to next arc
	if(nextPossibleDomain.empty()==false){
		//domain for next arc can be one gps point after
		//DEBUG_MESSAGE("domain +1");
		
		//loop through successors
		//DEBUG_MESSAGE("loop through succesor");
		for(set<patArc*>::iterator arcIter = successor[theArc].begin();
			arcIter != successor[theArc].end();
			arcIter++){
			

			pathTemp->push_back(const_cast<patArc*>(*arcIter));
			connectArcs(const_cast<patArc*>(*arcIter),pathTemp,nextPossibleDomain);
			pathTemp->pop_back();
		}
	}
	else{
		//DEBUG_MESSAGE("possible domain is empty");
	}
} 

void patTripGraph::recordPath(list<patArc*>* pathTemp){
	patPathJ thePath(*pathTemp);
	if(pathSet.find(thePath)!=pathSet.end()){
		DEBUG_MESSAGE("path exists");
	}
	pathSet.insert(thePath);
	
	//DEBUG_MESSAGE("A path recorded."<<thePath);
	//DEBUG_MESSAGE("current path size:"<<(int)pathSet.size());
}
*/