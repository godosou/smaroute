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


map<double, set<unsigned long> > patTripGraph::sortOrig(vector<patGpsPoint>* theGpsSequence){
map<patArc*, double>* firstDDR = theGpsSequence->at(0).getLinkDDR();
	map<unsigned long, double> origDDRSum;
	for(map<patArc*, double>::iterator arcDDR = firstDDR->begin();
			arcDDR!=firstDDR->end();
			++arcDDR){
		
		if(origDDRSum.find(arcDDR->first->upNodeId)==origDDRSum.end()){
			origDDRSum[arcDDR->first->upNodeId]=0.0;
		}
		origDDRSum[arcDDR->first->upNodeId]+=arcDDR->second;
	}
	
	map<double,set<unsigned long> > ddrOrig;
	for(map<unsigned long,double>::iterator origIter=origDDRSum.begin();
			origIter!=origDDRSum.end();
			++origIter){
			
		if(ddrOrig.find(origIter->second)==ddrOrig.end()){
			ddrOrig[origIter->second]= set<unsigned long>();
		}
		
		ddrOrig[origIter->second].insert(origIter->first);
	
	}
	
	return ddrOrig;
}
map<double, set<unsigned long> > patTripGraph::sortDest(vector<patGpsPoint>* theGpsSequence){
	
	map<patArc*, double>* lastDDR = theGpsSequence->back().getLinkDDR();
	map<unsigned long, double> destDDRSum;
	for(map<patArc*, double>::iterator arcDDR = lastDDR->begin();
			arcDDR!=lastDDR->end();
			++arcDDR){
		
		if(destDDRSum.find(arcDDR->first->m_down_node_id)==destDDRSum.end()){
			destDDRSum[arcDDR->first->m_down_node_id]=0.0;
		}
		destDDRSum[arcDDR->first->m_down_node_id]+=arcDDR->second;
	}
	
	map<double,set<unsigned long> > ddrDest;
	for(map<unsigned long,double>::iterator destIter=destDDRSum.begin();
			destIter!=destDDRSum.end();
			++destIter){
			
		if(ddrDest.find(destIter->second)==ddrDest.end()){
			ddrDest[destIter->second]= set<unsigned long>();
		}
		
		ddrDest[destIter->second].insert(destIter->first);
	
	}
	
	return ddrDest;

}
void patTripGraph::genOrigSet_SortDDR(vector<patGpsPoint>* theGpsSequence,unsigned long k){
	
	map<double,set<unsigned long> > ddrOrig = sortOrig(theGpsSequence);
	map<double,set<unsigned long> >::iterator origIter = ddrOrig.end();
	
	
	for(int i=0;i<k;++i){
		origIter--;
			for(set<unsigned long>::iterator nodeIter = origIter->second.begin();
			nodeIter != origIter->second.end();
			++nodeIter){
				origSet.insert(baseNetwork->getNodeFromUserId(*nodeIter));
			}
		if(origIter==ddrOrig.begin()){
			break;
		}
	}
	
}
void patTripGraph::genOrigSet_Bound(vector<patGpsPoint>* theGpsSequence,double lowerBound, double upperBound){
	map<double,set<unsigned long> > ddrOrig = sortOrig( theGpsSequence);
	for(map<double,set<unsigned long> >::iterator origIter = ddrOrig.begin();
				origIter != ddrOrig.end();
				++origIter){
				
		if((origIter->first>=lowerBound && origIter->first <=upperBound)||
			(origIter->first>=39 && origIter->first <=40)
		){
			for(set<unsigned long>::iterator nodeIter = origIter->second.begin();
			nodeIter != origIter->second.end();
			++nodeIter){
				origSet.insert(baseNetwork->getNodeFromUserId(*nodeIter));
			}
		}
	}

}
void patTripGraph::genOrigSet_All(vector<patGpsPoint>* theGpsSequence){
		map<patArc*, double>* firstDDR = theGpsSequence->at(0).getLinkDDR();
	for(map<patArc*, double>::iterator arcDDR = firstDDR->begin();
			arcDDR!=firstDDR->end();
			++arcDDR){
			origSet.insert(baseNetwork->getNodeFromUserId(arcDDR->first->upNodeId));
}
}
void patTripGraph::genOrigSet_Ids(vector<unsigned long> origIds){

	for(vector<unsigned long>::iterator idIter = origIds.begin();
			idIter!=origIds.end();
			++idIter){
			patNode* orig = baseNetwork->getNodeFromUserId(*idIter);
			if (orig !=NULL){
				origSet.insert(orig);
			
			}
	}
}
set<patPathJ> patTripGraph::genPaths(vector<patGpsPoint>* theGpsSequence,
	map<patArc*,set<unsigned long> >* theAssocDomain){
	
	writeGraph();
	assocDomain=theAssocDomain;
	gpsSequence=theGpsSequence;
	calLengthCeil();
	map<patArc*,double>* initLinkDDR = gpsSequence->at(0).getLinkDDR();
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
	for(map<patArc*,double>::iterator iter1 = initLinkDDR->begin();
				iter1!=initLinkDDR->end();
				++iter1){
		for(set<patNode*>::iterator iter2 = origSet.begin();
						iter2!=origSet.end();
						++iter2){
						
			if(iter1->first->upNodeId == (*iter2)->userId){
				//DEBUG_MESSAGE("generate path from"<<*baseNetwork->getNodeFromUserId(iter1->first->upNodeId));
				list<patArc*> pathTemp;
				set<unsigned long> possibleDomain;
				possibleDomain.insert(0);
				connectArcs(iter1->first,&pathTemp,possibleDomain);
			}
		
		}
		
	}
	
	return pathSet;
}


bool patTripGraph::isVisited(unsigned long nodeUserId,list<patArc*>* pathTemp){
	if(pathTemp->empty()==true){
		return false;
	}
	patArc* backArc = pathTemp->back();
	pathTemp->pop_back();
	
	bool rtnValue = false;
	for(list<patArc*>::iterator arcIter = pathTemp->begin();
						arcIter!=pathTemp->end();
						++arcIter){
		
		if(nodeUserId == (*arcIter)->upNodeId ){
			rtnValue = true;
			break;
		}
	
	}
	pathTemp->push_back(backArc);
	return rtnValue;
}

double patTripGraph::calAverageSpeed(){
	double avegSpeed = 0.0;
	double count = 0;
	for(unsigned long i = 0; i<gpsSequence->size();++i){
		double as = gpsSequence->at(i).getSpeedMS();
		if(as>=1.0){
		
		avegSpeed += gpsSequence->at(i).getSpeedMS();
		count++;
		}
	}
	
	avegSpeed /= count;
	//DEBUG_MESSAGE("average speed"<<avegSpeed/3.6);
	return avegSpeed;
}

double patTripGraph::calLengthCeil(){
	lengthCeil = patNBParameters::the()->pathLengthCeil * calAverageSpeed() * (gpsSequence->back().getTimeStamp()-gpsSequence->front().getTimeStamp());
}

void patTripGraph::connectArcs(patArc* theArc,list<patArc*>* pathTemp,set<unsigned long> possibleDomain) {
	pathTemp->push_back(theArc);
	//DEBUG_MESSAGE("arc"<<*theArc);
	//detect loop
	if(isVisited(theArc->m_down_node_id,pathTemp)==true){
			 //DEBUG_MESSAGE("a loop"<<pathTemp);					
			return;
	}
	patPathJ thePath(*pathTemp);
	if(thePath.computeLength() > lengthCeil){
	
		//DEBUG_MESSAGE("path too long"<<thePath.computePathLength()<<","<<lengthCeil);
		return ;
	}
	//if in the domain of last gps point record a path		
	map<patArc*,double>* backGpsDDR = gpsSequence->back().getLinkDDR();
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
	set<unsigned long> nextPossibleDomain;
	//if(assocDomain->find(theArc)==assocDomain->end()){
		//DEBUG_MESSAGE("no gps correlated");
		nextPossibleDomain = possibleDomain;
	//}
	
	//DEBUG_MESSAGE("determin the next possible domain");

		for(set<unsigned long>::iterator iter = (*assocDomain)[theArc].begin();
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

set<unsigned long> calNextPossibleDomain(set<unsigned long> prevDomain,set<unsigned long> currDomain){
	

}
void patTripGraph::recordPath(list<patArc*>* pathTemp){
	patPathJ thePath(*pathTemp);
	if(pathSet.find(thePath)!=pathSet.end()){
		DEBUG_MESSAGE("path exists");
		return ;
	}
	if(thePath.isValid(gpsSequence)==true){
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

	map<double,set<patPathJ*> > ddrPathSet;
	map<double,set<patPathJ*> > ddrPathSet_avg;
	map<double,set<patPathJ*> > lengthPathSet;
	map<double,set<patPathJ*> > distancePathSet;
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
 
		double ddrValue = probaRaw.run_raw();
		
		double pathLength = currPath->computeLength();
		double ddrValue_avg = ddrValue/pathLength;
		
		double distance = probaRaw.run_dist();
		
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
	map<double,set<patPathJ*> >::iterator ddrPathIterBack= ddrPathSet.end();
	map<double,set<patPathJ*> >::iterator ddrPathIterBack_avg= ddrPathSet_avg.end();
	map<double,set<patPathJ*> >::iterator ddrPathIterFront= ddrPathSet.begin();
	
	map<double,set<patPathJ*> >::iterator distanceIterFront= distancePathSet.begin();
	
	//map<patReal,set<patPathJ> >::iterator ddrPathIterFront_avg= ddrPathSet_avg.begin();
	map<double,set<patPathJ*> >::iterator lengthPathIterFront= lengthPathSet.begin();

	ddrPathIterBack--;
	ddrPathIterBack_avg--;
	unsigned long best = patNBParameters::the()->selectBestPaths;
	unsigned long worst = patNBParameters::the()->selectWorstPaths;
	unsigned long shortest = patNBParameters::the()->selectShortestPaths;
	
	
	DEBUG_MESSAGE("best: "<<best<<", worst: "<<worst<<", shortest: "<<shortest);
	for(unsigned long i =0; i<worst;++i){
	
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
	
	for(unsigned long i=0;i<best;++i){
		
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
	
	for(unsigned long i=0;i<shortest;++i){
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