    #include "patUniform.h"
#include "patParameters.h"
#include "patNBParameters.h"
#include "patDisplay.h"
#include "patConst.h"
#include "patPathSampling.h"
#include "patSample.h"
#include "patPathJ.h"
#include "patOdJ.h"
#include "patShortestPathAlgoDest.h"
#include "patErrOutOfRange.h"
#include "patErrMiscError.h"
#include "patErrNullPointer.h"
#include "patPower.h"
#include "patDiscreteDistribution.h"

patPathSampling::patPathSampling( patNetwork* baseNetwork,
        vector< list < pair<patArc*, patULong> > >* theadjList,
		patPathJ* aPath):
			shortestPathCost(patMaxReal),
			destTree(),
		kumaA( patNBParameters::the()->kumaA),
		kumaB( patNBParameters::the()->kumaB),
        adjList(theadjList),
		//randomNumbersGenerator(patULong(13124))
	randomNumbersGenerator(patParameters::the()->getgevSeed())
{
	theNetwork=baseNetwork;
	thePath=aPath;
	
	theOd=thePath->getOd();
}



void patPathSampling::run(patULong algoName,		patError*& err){
	switch(algoName){
		case 1: randomWalk(err); break;
		default: break;
	}
	
	// destTree.destroyTree();
}

/*
To be improved:
only labels of tree node(shortest path cost) are useful.
To save memory, only store those labels < patMaxReal as shortestPathCost list
*/

/*
 generate a shortest path tree to destination
 */
void patPathSampling::getShortestPathTreeDest(patError*& err){
							
	//DEBUG_MESSAGE("generate shortest path tree"<<theNetwork->adjacencyLists->size()<<","<<theNetwork->minimumLabelForShortestPath);
	patShortestPathAlgoDest destAlgo(theOd->getDestination()->internalId,
								adjList,
								theNetwork->minimumLabelForShortestPath);
	destAlgo.computeShortestPathTree(err);
	if (err != NULL) {
		WARNING(err->describe()) ;
		return ;
	}
	 destTree = destAlgo.getTree();
	/*
		patULong i=0;
	
		for (i=0;i<destTree.labels.size();++i){
			DEBUG_MESSAGE("from "<<i<<":"<<destTree.labels[i]);
		
		}
	*/					
}


/*
 get the shortest path cost from a node "aId" to destination
 */
patReal patPathSampling::getShortestPathCostFrom(const patULong aId,patError*& err){
	//DEBUG_MESSAGE("get shortest path from"<<aId<<" to "<<dest->internalId);
	
	if (aId==theOd->getDestination()->internalId){
		return 0.0;
	}
	if(aId>=destTree.labels.size()){
		stringstream str ;
		str << "node id exceeds size of network";
		err = new patErrMiscError(str.str()) ;
		WARNING(err->describe()) ;
		return patMaxReal ;
	}
	patReal cost=destTree.labels[aId];
	//DEBUG_MESSAGE(cost);
	if( cost<100000.0){
	
		//DEBUG_MESSAGE("return a cost"<<cost); 	
		return cost;
	}
	else{
		return patMaxReal ;
		stringstream str ;
		DEBUG_MESSAGE("Fail when get shortest path from "<<aId<<"to destination");
		str << "Fail when get shortest path from "<<theNetwork->internalNodes[aId]<<"to destination";
		err = new patErrMiscError(str.str()) ;
		WARNING(err->describe()) ;
		return patMaxReal ;
	}
}

patReal patPathSampling::computePathProbaForRandomWalk(patPathJ* thePath,patError*& err){
	patReal logPathProba = 0.0;
	
	patReal dest_count=0.0;
	for(list<patArc*>::iterator arcIter = thePath->getArcList()->begin();
		arcIter != thePath->getArcList()->end();
		++arcIter){
			
		patNode* tmpNode = theNetwork->getNodeFromUserId((*arcIter)->upNodeId);
		patReal theShortestPathCost=patMaxReal;
		if (tmpNode == theOd->getDestination()){
			
			dest_count+=1.0;
			//DEBUG_MESSAGE("dest"<<dest_count);
			logPathProba+=log(pow(0.5,dest_count));
			for (set<patULong>::iterator iter =  tmpNode->userSuccessors.begin();
				iter!=tmpNode->userSuccessors.end();
				++iter){
					patArc* tmpArc = theNetwork->getArcFromNodesUserId(tmpNode->userId,*iter);
					if ( tmpArc==NULL||tmpArc->name=="M1" || tmpArc->attributes.priority<4 || tmpArc->attributes.priority>14){
						continue;
					}
					patReal spCostDN = getShortestPathCostFrom(theNetwork->getInternalNodeIdFromUserId(tmpArc->downNodeId),err);
					if (err != NULL || spCostDN==patMaxReal) {
						DEBUG_MESSAGE("not valid outgoing link");
						continue;
					}
					else{
						if (spCostDN+tmpArc->getLength()<theShortestPathCost){
							theShortestPathCost=spCostDN+tmpArc->getLength();
						}
					}

			}
			
		}
		else{
			 theShortestPathCost= getShortestPathCostFrom(theNetwork->getNodeFromUserId((*arcIter)->upNodeId)->internalId,err);
		}
		if (err != NULL) {
			WARNING(err->describe()) ;
			return 0.0 ;
		}
		
		//Loop on the outgoing arcs, and compute the cost of each shortest path.
		vector<patReal>proba;
		vector<patArc*> theArcs;
		patReal total(0.0);
		
		for (set<patULong>::iterator iter =  tmpNode->userSuccessors.begin();
			iter!=tmpNode->userSuccessors.end();
			++iter){
			
			patArc* tmpArc = theNetwork->getArcFromNodesUserId(tmpNode->userId,*iter);
			if ((*tmpArc).name=="M1" || (*tmpArc).attributes.priority<4 || (*tmpArc).attributes.priority>14) {
				continue;
			}
			//theOd.orig = theNetwork->getNodeFromUserId(tmpArc->downNodeId);
			patReal spCost = getShortestPathCostFrom(theNetwork->getNodeFromUserId(tmpArc->downNodeId)->internalId,err);
			
			if (err!=NULL || spCost==patMaxReal) {
				DEBUG_MESSAGE("no shortest path to dest");
				continue;
			}
			patReal theRatio = theShortestPathCost/(spCost+tmpArc->getLength());
		//	DEBUG_MESSAGE(theRatio<<"="<<theShortestPathCost<<"/("<<spCost<<"+"<<tmpArc->getLength()<<")");
			if (theRatio == 0.0){
				stringstream str ;
				str << "Ratio should be greater than zeros, and is " << theRatio ;
				err = new patErrMiscError(str.str()) ;
				WARNING(err->describe()) ;
				return 0.0 ;
			}
			if(theRatio >1.0){
				stringstream str ;
				str << "Ratio should be lower than 1, and is " << theRatio ;
				err = new patErrMiscError(str.str()) ;
				WARNING(err->describe()) ;
				return 0.0 ;
			}
			
			patReal kuma = patPower(theRatio*10.0,kumaA);
			//DEBUG_MESSAGE(theRatio<<","<<kuma);
			if(!finite(kuma) || isnan(kuma)){
				WARNING("kuma is not valid"<<kuma);
				//DEBUG_MESSAGE(theRatio);
				kuma = 0.0;
			}			
			
			total+=kuma;
			
			proba.push_back(kuma);
			theArcs.push_back(tmpArc);
		}
		for (vector<patReal>::iterator i = proba.begin() ;
				i != proba.end() ;
				++i) {
			*i /= total ;
		}
	
		
		
		for(int i = 0; i<proba.size();++i){
			if(theArcs[i]==(*arcIter)){
		//		DEBUG_MESSAGE(i<<":"<<proba[i]);
				logPathProba+= log(proba[i]);
			}
		}
		//DEBUG_MESSAGE("log proba"<<logPathProba);
	}
	logPathProba+=log(1-pow(0.5,dest_count+1.0));
	return logPathProba;

}

patPathJ* patPathSampling::newPath(patPathJ aPath){
	 
//	aPath.assignOd(theOd);
	patPathJ* pathPointer = theOd->addPath(aPath);
	if(pathPointer==NULL){
		WARNING("path is not added");
	}
	choiceSet.insert(pathPointer);
	if(countReplicates.find(pathPointer)==countReplicates.end()){
		countReplicates[pathPointer]=1;
	}
	else{
		countReplicates[pathPointer]+=1;
	}
	
	return pathPointer;
	
}

void patPathSampling::newSampledPath(patPathJ aPath, patReal proba){
	patPathJ* newPathPt = newPath(aPath);
	rwLogProba[newPathPt]=proba;	
}

void patPathSampling::addPath(patPathJ aPath,patError*& err){
	patReal sampleProba=computePathProbaForRandomWalk(&aPath,err);
	if(err!=NULL || sampleProba==0.0){
		stringstream str ;
		str << " Error in calculating sampling probability or the probability is zero";
		err = new patErrMiscError(str.str()) ;
		WARNING(err->describe()) ;
		return;		
	}
	else{
		patPathJ* newPathPt = newPath(aPath);
		addedPath.insert(newPathPt);
		rwLogProba[newPathPt]=sampleProba;
	}
}

pair<patArc*,patReal> patPathSampling::getNextArcInRandomWalk(const patNode* fromNode,patUnixUniform* randomNumbersGenerator,patError*& err){
	//DEBUG_MESSAGE("get the next arc from"<<*fromNode);
	patReal theShortestPathCost=patMaxReal;
	if(fromNode!=theOd->getDestination()){
		 theShortestPathCost= getShortestPathCostFrom(fromNode->internalId,err);
	}
	else{
		for (set<patULong>::iterator iter =  fromNode->userSuccessors.begin();
			iter!=fromNode->userSuccessors.end();
			++iter){
				patArc* tmpArc = theNetwork->getArcFromNodesUserId(fromNode->userId,*iter);
				if ( tmpArc==NULL||tmpArc->name=="M1" || tmpArc->attributes.priority<4 || tmpArc->attributes.priority>14){
					continue;
				}
				patReal spCostDN = getShortestPathCostFrom(theNetwork->getInternalNodeIdFromUserId(tmpArc->downNodeId),err);
				if (err != NULL || spCostDN==patMaxReal) {
					DEBUG_MESSAGE("not valid outgoing link");
					continue;
				}
				else{
					if (spCostDN+tmpArc->getLength()<theShortestPathCost){
						theShortestPathCost=spCostDN+tmpArc->getLength();
					}
				}
			
		}
	}
	if (err != NULL) {
		//DEBUG_MESSAGE("from node internalId"<<fromNode->internalId);
		WARNING(err->describe()) ;
		return pair<patArc*,patReal>(NULL,0.0) ;
	}
		
	//Loop on the outgoing arcs, and compute the cost of each shortest path.
	vector<patReal>proba;
	vector<patArc*> theArcs;
	patReal total(0.0);
		
	for (set<patULong>::iterator iter =  fromNode->userSuccessors.begin();
		iter!=fromNode->userSuccessors.end();
		++iter){
		patArc* tmpArc = theNetwork->getArcFromNodesUserId(fromNode->userId,*iter);
		if ( tmpArc==NULL||tmpArc->name=="M1" || tmpArc->attributes.priority<4 || tmpArc->attributes.priority>14){
			continue;
		}
		//theOd.orig = theNetwork->getNodeFromUserId(tmpArc->downNodeId);
		patReal spCost = getShortestPathCostFrom(theNetwork->getInternalNodeIdFromUserId(tmpArc->downNodeId),err);
		if (err != NULL || spCost==patMaxReal) {
			DEBUG_MESSAGE("not valid outgoing link");
			continue;
		}
			
		patReal theRatio = theShortestPathCost/(spCost+tmpArc->generalizedCost);
		if(theRatio >1.0){
			stringstream str ;
			str << "Ratio should be larger than 1, and is " << theRatio <<*tmpArc;
			str << "shortest cost:"<<theShortestPathCost<<"sp cost"<<spCost<<"arc cost:"<<tmpArc->generalizedCost;
			err = new patErrMiscError(str.str()) ;
			WARNING(err->describe()) ;
		return pair<patArc*,patReal>(NULL,0.0) ;
		}
			
		patReal kuma = patPower(theRatio*10.0,kumaA);
		if(!finite(kuma)){
			kuma = 0.0;
		}			
			
		total+=kuma;
		proba.push_back(kuma);
		//DEBUG_MESSAGE("proba"<<kuma<<","<<kumaA<<","<<kumaB);
		theArcs.push_back(tmpArc);
		
		}
	for (vector<patReal>::iterator i = proba.begin() ;
			i != proba.end() ;
			++i) {
			
		*i /= total ;
		//DEBUG_MESSAGE("proba"<<*i);
		
	}
	if(proba.size()==0|| total==0.0){
		DEBUG_MESSAGE("error ");
		return pair<patArc*,patReal>(NULL,0);
	}	
			
	patULong selectId = patDiscreteDistribution(&proba,randomNumbersGenerator)();
	
	return pair<patArc*,patReal>(theArcs[selectId],proba[selectId]);
}

void patPathSampling::getOnePathWithRandomWalk(patUnixUniform* randomNumbersGenerator,patError*& err1){
	patPathJ thePath;
	patNode* currentNode = theOd->getOrigin();
	patReal logProba = 0.0;
	patReal dest_count=0.0;
	while(true){
//		DEBUG_MESSAGE(logProba);
		if (currentNode==theOd->getDestination()){
			dest_count+=1.0;
			vector<patReal>proba;
			vector<patArc*>arcVector;
			proba.push_back(pow(0.5,dest_count));
			proba.push_back(1.0-pow(0.5,dest_count));
			patULong selectId = patDiscreteDistribution(&proba,randomNumbersGenerator)();
			logProba+=log(proba[selectId]);
			if(selectId==proba.size()-1){
				break;
			}
		}
			patError* err(NULL) ;
			
			 pair<patArc*, patReal> result = getNextArcInRandomWalk(currentNode, randomNumbersGenerator, err);
			
			 if (err != NULL||result.first==NULL) { 
				stringstream str ;
				str << "sampling algorithm can't proceeds";
				err1 = new patErrMiscError(str.str()) ;
				WARNING(err->describe()) ;
				return ;
			 } 
//			DEBUG_MESSAGE("add a arc"<<*(result.first)<<","<<result.second); 
			
			thePath.addArcToBack(result.first); 
			logProba += log(result.second);
			
			 currentNode =theNetwork->getNodeFromUserId(result.first->downNodeId); 
			if (currentNode == NULL) {
				 err1 = new patErrNullPointer("patNode") ;
				 WARNING(err->describe()) ; 
				return  ;
            }
			
		
	}
	if(isfinite(logProba)){
		newSampledPath(thePath, logProba);
	}
	else{
		stringstream str ;
		str << "sampling algorithm is not valid:"<<exp(logProba);
		err1 = new patErrMiscError(str.str()) ;
		WARNING(err1->describe()) ;
		return ;
		
	}
	DEBUG_MESSAGE("generat a path with random walk."<<logProba);
	

}

void patPathSampling::getMultiplePathsWithRandomWalk(patULong nbrOfPaths,
								patULong nbrOfTrials,
								patUnixUniform* randomNumbersGenerator,
								patError*& err)
{	
	for(patULong iter = 0;
		iter <nbrOfTrials && choiceSet.size()<nbrOfPaths;
		++iter	){
		patError* err(NULL) ;
		getOnePathWithRandomWalk(randomNumbersGenerator,err);
		while(err!=NULL){
			err=NULL;
			getOnePathWithRandomWalk(randomNumbersGenerator,err);
		}
	}
	DEBUG_MESSAGE("After randowm walk paths: " << choiceSet.size()) ;
}

patBoolean patPathSampling::isAddedPath(patPathJ* aPath){
	if(addedPath.find(aPath)!=addedPath.end()){
		return patTRUE;
	}
	else{
		return patFALSE;
	}
}

//extended path size
void patPathSampling::computeEPS( patError*& err){
	DEBUG_MESSAGE("compute extended path size");
	map<patArc*, patReal> EPM; //expansion M for chosen path
	for(set<patPathJ*>::iterator pathIter = choiceSet.begin();
		pathIter!=choiceSet.end();
		++pathIter){
			
		patPathJ* aPath = const_cast<patPathJ*>(*pathIter);
		set<patArc*> arcSet = aPath->getDistinctArcs();
		patReal pL = aPath->getPathLength();
		
		for(set<patArc*>::iterator arcIter = arcSet.begin();
				arcIter!=arcSet.end();
				++arcIter){
			
			if(EPM.find(*arcIter)==EPM.end()){
				EPM[*arcIter] = 0.0;
			}					
			if( isAddedPath(aPath) || exp(getPathRwLogProba(aPath,err)+log(patNBParameters::the()->maxTrialsForRandomWalk)) >= 1.0){
				EPM[*arcIter]+=1.0;
			}
			else{
				EPM[*arcIter]+= 1.0/(exp(getPathRwLogProba(aPath,err)+log(patNBParameters::the()->maxTrialsForRandomWalk)));
				
			}
		}		
	}
	for(set<patPathJ*>::iterator pathIter = choiceSet.begin();
		pathIter!=choiceSet.end();
		++pathIter){
			
		patReal eps = 0.0;
		patPathJ* aPath = const_cast<patPathJ*>(*pathIter);
		list<patArc*>* arcList = aPath->getArcList();
		patReal pL = aPath->getPathLength();
		
		for(list<patArc*>::iterator arcIter = arcList->begin();
				arcIter!=arcList->end();
				++arcIter){
//					DEBUG_MESSAGE((*arcIter)->getLength()<<pL<<EPM[*arcIter]);
			if(isfinite(EPM[*arcIter])){
				eps +=((*arcIter)->getLength())/(pL * EPM[*arcIter]);
			}
//			DEBUG_MESSAGE(eps);
		}
//		DEBUG_MESSAGE(eps);
		choiceSetEPS[aPath]=eps;
	}
}

void patPathSampling::computePathSizes( patError*& err){
	DEBUG_MESSAGE("compute path size");
	patReal pathSizeGamma = patNBParameters::the()->pathSizeGamma;
	
	map<patArc*,patULong > arcOverlap;
	
		for(set<patPathJ*>::iterator pathIter = choiceSet.begin();
			pathIter!=choiceSet.end();
			++pathIter){
			patPathJ* aPath = const_cast<patPathJ*>(*pathIter);
			list<patArc*>* arcList = aPath->getArcList();
			
			patReal pL = aPath->getPathLength();
			
			for(list<patArc*>::iterator arcIter = arcList->begin();
					arcIter!=arcList->end();
					++arcIter){
				if(arcOverlap.find(*arcIter)==arcOverlap.end()){
					arcOverlap[*arcIter] = 0;
				}
				arcOverlap[*arcIter] += 1;
				}
		}
		
		for(set<patPathJ*>::iterator pathIter = choiceSet.begin();
			pathIter!=choiceSet.end();
			++pathIter){
			patReal ps = 0.0;
			patPathJ* aPath = const_cast<patPathJ*>(*pathIter);
			list<patArc*>* arcList = aPath->getArcList();
			patReal pL = aPath->getPathLength();
			
			for(list<patArc*>::iterator arcIter = arcList->begin();
					arcIter!=arcList->end();
					++arcIter){
				
					ps += (*arcIter)->getLength()/(pL*arcOverlap[*arcIter]);
			}
			choiceSetPS[aPath]=ps;
			if(!isfinite(choiceSetPS[aPath])){
				WARNING("wrong ps value"<<ps);
			}
		}
		
}

void patPathSampling::computePathAttributes(patError*& err){
	 
	if(choiceSet.size()==0){
			stringstream str ;
			str << "No path connects OD"<<*theOd;
			err = new patErrMiscError(str.str()) ;
			WARNING(err->describe()) ;
			return ;

		}
	DEBUG_MESSAGE("compute path size "<<choiceSet.size());
	
	for(set<patPathJ*>::iterator pathIter = choiceSet.begin();
			pathIter!=choiceSet.end();
			++pathIter){
		const_cast<patPathJ*>(*pathIter)->calTurns(theNetwork);
		const_cast<patPathJ*>(*pathIter)->computeTrafficSignals(theNetwork);
	}
	computeEPS(err);
	computePathSizes(err);	

}

void patPathSampling::randomWalk(patError*& err){
	patULong maxNumberOfGeneratedPaths = patNBParameters::the()->maxNumberOfGeneratedPaths;
	patULong maxTrialsForRandomWalk = patNBParameters::the()->maxTrialsForRandomWalk;
	ofstream debugFile("RW.debug");
	err=NULL;
	DEBUG_MESSAGE("start random walk for "<<*theOd);
	getShortestPathTreeDest(err);
	if (err != NULL) {
		WARNING(err->describe()) ;
		return ;
	}

	addPath(*thePath,err);

	DEBUG_MESSAGE("Before random walk, there are "<<choiceSet.size()<<" paths");

	getMultiplePathsWithRandomWalk(maxNumberOfGeneratedPaths,
					maxTrialsForRandomWalk,
					&randomNumbersGenerator,err);
	DEBUG_MESSAGE("After random walk: there are " << choiceSet.size() << " paths") ;
	
	//DEBUG_MESSAGE("Generated paths for " << *this) ;
	computePathAttributes(err);
	debugFile.close();
	
	return ;
}

set<patPathJ*>* patPathSampling::getChoiceSet(){
	return &choiceSet;
}
patReal patPathSampling::getPathEPS(patPathJ* aPath,patError*& err){
	if(choiceSetEPS.find(aPath)==choiceSetEPS.end()){
		stringstream str ;
		str << "path is not in the choice set"<<*aPath;
		err = new patErrMiscError(str.str()) ;
		WARNING(err->describe()) ;
		
		return 0.0;
	}
	else{
		return choiceSetEPS[aPath];
	}
}
patReal patPathSampling::getPathPS(patPathJ* aPath,patError*& err){
	
	if(choiceSetPS.find(aPath)==choiceSetPS.end()){
		stringstream str ;
		str << "path is not in the choice set"<<*aPath;
		err = new patErrMiscError(str.str()) ;
		WARNING(err->describe()) ;
		
		return 0.0;
	}
	else{
		return choiceSetPS[aPath];
	}
}

patReal patPathSampling::getPathRwLogProba(patPathJ* aPath,patError*& err){
	
	if(rwLogProba.find(aPath)==rwLogProba.end()){
		stringstream str ;
		str << "path is not in the choice set"<<*aPath;
		err = new patErrMiscError(str.str()) ;
		WARNING(err->describe()) ;
		
		return 0.0;
	}
	else{
		return rwLogProba[aPath];
	}
}
patULong patPathSampling::getPathReplicates(patPathJ* aPath,patError*& err){
	
	if(countReplicates.find(aPath)==countReplicates.end()){
		stringstream str ;
		str << "path is not in the choice set"<<*aPath;
		err = new patErrMiscError(str.str()) ;
		WARNING(err->describe()) ;
		
		return 0;
	}
	else{
		return countReplicates[aPath];
	}
}
