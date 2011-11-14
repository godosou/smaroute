#include "patShortestPathTreeRange.h"
#include "patErrOutOfRange.h"
#include <set>
#include "patType.h"
#include "patNetwork.h"
#include "patArc.h"
#include "patNode.h"
#include "patDisplay.h"
#include "patGpsPoint.h"
patShortestPathTreeRange::patShortestPathTreeRange( patULong nNodes) :

    labels(nNodes,patMaxReal),
    successor(nNodes,list<pair<patULong,patArc*> >(NULL)),
    predecessor(nNodes,pair<patULong,patArc*>(patBadId,NULL)) {
  
} ;
patShortestPathTreeRange::patShortestPathTreeRange( ) 
{
}   

list<patULong > 	 patShortestPathTreeRange::getShortestPathTo(patULong dest,patNetwork* theNetwork,map<patArc*,patReal>* theLinkDDR) {
  list<patULong > theList ;
  //DEBUG_MESSAGE("short"<<dest<<",pred"<<predecessor.size()<<",labes"<<labels.size()<<",su"<<successor.size());
  if (dest >= predecessor.size()) {
    /*err = new patErrOutOfRange<patULong>(dest,0,predecessor.size()-1) ;
    WARNING(err->describe()) ;
    */
    return list<patULong>() ;
  }
  patReal DDRValue = 0.0;
	patULong currentNode = dest ;
	map<patArc*,patReal>::iterator DDRFind;
	while(	currentNode != patBadId){
	//DEBUG_MESSAGE(currentNode);
		//DEBUG_MESSAGE(predecessor[3957]);
		theList.push_front(currentNode);
		set<patULong> nodePredecessor = theNetwork->internalNodes[currentNode]->userPredecessors;
		patULong currentNodeUserId = theNetwork->internalNodes[currentNode]->userId;
		currentNode = predecessor[currentNode].first;
		//DEBUG_MESSAGE(currentNode);
		/*
		for(set<patULong>::iterator iter = nodePredecessor.begin();iter!=nodePredecessor.end();++iter){
		

			patArc* theArc = theNetwork->getArcFromNodesUserId(*iter,currentNodeUserId);

			DDRFind = theLinkDDR->find(theArc);
			if (DDRFind!=theLinkDDR->end()){
				DDRValue += DDRFind->second;

				currentNode = theNetwork->getInternalNodeIdFromUserId(*iter);	
				DEBUG_MESSAGE(currentNode);			
				continue;			
			}
		}
		
		//DEBUG_MESSAGE(currentNode);
		*/
	}


  return theList;
}

list<patArc* > 	 patShortestPathTreeRange::getShortestPathToV2(patULong dest) {
  list<patArc* > theList ;
  list<patArc* > badList;
  badList.push_back(NULL);
  //DEBUG_MESSAGE("short"<<dest<<",pred"<<predecessor.size()<<",labes"<<labels.size()<<",su"<<successor.size());
  if (dest >= predecessor.size()) {
    /*err = new patErrOutOfRange<patULong>(dest,0,predecessor.size()-1) ;
    WARNING(err->describe()) ;
    */
    return badList;
  }
  patReal DDRValue = 0.0;
	patULong currentNode = dest ;
	map<patArc*,patReal>::iterator DDRFind;
	while(	currentNode != patBadId){
	//DEBUG_MESSAGE(currentNode);
		//DEBUG_MESSAGE(predecessor[3957]);
		if(predecessor[currentNode].first!=patBadId){
			/*
			if(currGpsPoint->inReverseArc(predecessor[currentNode].second,prevGpsPoint, theNetwork) == patTRUE){
				return badList;
			}
			*/
			theList.push_front(predecessor[currentNode].second);
		
		}
		//set<patULong> nodePredecessor = theNetwork->internalNodes[currentNode]->userPredecessors;
		//patULong currentNodeUserId = theNetwork->internalNodes[currentNode]->userId;
		currentNode = predecessor[currentNode].first;
	}


  return theList;
}

patReal patShortestPathTreeRange::getShortestPathCost(patULong dest){
	list<patArc* > 	 l = getShortestPathToV2(dest);
	patReal c = 0.0;
	DEBUG_MESSAGE("arcs"<<l.size());
	for (list<patArc*>::iterator i = l.begin();
			i!=l.end();
			++i){
		c+= (*i)->generalizedCost;	
	}
	return c;
}
