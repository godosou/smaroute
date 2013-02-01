#include "patShortestPathTreeRange.h"
#include "patErrOutOfRange.h"
#include <set>
#include "patType.h"
#include "patNetwork.h"
#include "patArc.h"
#include "patNode.h"
#include "patDisplay.h"
#include "patGpsPoint.h"
patShortestPathTreeRange::patShortestPathTreeRange( unsigned long nNodes) :

    labels(nNodes,patMaxReal),
    successor(nNodes,list<pair<unsigned long,patArc*> >(NULL)),
    predecessor(nNodes,pair<unsigned long,patArc*>(patBadId,NULL)) {
  
} ;
patShortestPathTreeRange::patShortestPathTreeRange( ) 
{
}   

list<unsigned long > 	 patShortestPathTreeRange::getShortestPathTo(unsigned long dest,patNetwork* theNetwork,map<patArc*,double>* theLinkDDR) {
  list<unsigned long > theList ;
  //DEBUG_MESSAGE("short"<<dest<<",pred"<<predecessor.size()<<",labes"<<labels.size()<<",su"<<successor.size());
  if (dest >= predecessor.size()) {
    /*err = new patErrOutOfRange<patULong>(dest,0,predecessor.size()-1) ;
    WARNING(err->describe()) ;
    */
    return list<unsigned long>() ;
  }
  double DDRValue = 0.0;
	unsigned long currentNode = dest ;
	map<patArc*,double>::iterator DDRFind;
	while(	currentNode != patBadId){
	//DEBUG_MESSAGE(currentNode);
		//DEBUG_MESSAGE(predecessor[3957]);
		theList.push_front(currentNode);
		set<unsigned long> nodePredecessor = theNetwork->internalNodes[currentNode]->userPredecessors;
		unsigned long currentNodeUserId = theNetwork->internalNodes[currentNode]->userId;
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

list<patArc* > 	 patShortestPathTreeRange::getShortestPathToV2(unsigned long dest) {
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
  double DDRValue = 0.0;
	unsigned long currentNode = dest ;
	map<patArc*,double>::iterator DDRFind;
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

double patShortestPathTreeRange::getShortestPathCost(unsigned long dest){
	list<patArc* > 	 l = getShortestPathToV2(dest);
	double c = 0.0;
	DEBUG_MESSAGE("arcs"<<l.size());
	for (list<patArc*>::iterator i = l.begin();
			i!=l.end();
			++i){
		c+= (*i)->generalizedCost;	
	}
	return c;
}
