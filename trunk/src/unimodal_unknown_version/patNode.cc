//----------------------------------------------------------------
// File: patNode.cc
// Author: Michel Bierlaire
// Creation: Thu Oct 30 10:51:19 2008
//----------------------------------------------------------------

#include "patConst.h"
#include "patNode.h"
#include "patGeoCoordinates.h" 
#include "patDisplay.h"

#include "patStlSetIterator.h"

patNode::patNode(unsigned long theId, 
		 patString theName, 
		 double lat, 
		 double lon,
		 struct node_attributes theAttr
		 ) :
  userId(theId),
  internalId(patBadId),
  name(theName),
  geoCoord(lat,lon),
  attributes(theAttr),
  isCentroid(false) { 
}
patNode::patNode(unsigned long theId,double lat, double lon) :
	  userId(theId),
	  internalId(patBadId),
	  geoCoord(lat,lon),
	  isCentroid(false) {


}

void patNode::addSuccessor(unsigned long aSucc) {
  userSuccessors.insert(aSucc) ;
}

void patNode::addPredecessor(unsigned long aPred) {
  userPredecessors.insert(aPred) ;
}

ostream& operator<<(ostream& str, const patNode& x) {

  str << "Node " << x.userId << " [" << x.name << "] " 
      << x.geoCoord ;
  if (!x.userPredecessors.empty()) {
    str << " P(" ;
    for (set<unsigned long>::iterator i = x.userPredecessors.begin() ;
	 i != x.userPredecessors.end() ;
	 ++i) {
      if (i != x.userPredecessors.begin()) {
	str << "," ;
      }
      str << *i ;
    }
    str << ")" ;
  }
  if (!x.userSuccessors.empty()) {
    str << " P(" ;
    for (set<unsigned long>::iterator i = x.userSuccessors.begin() ;
	 i != x.userSuccessors.end() ;
	 ++i) {
      if (i != x.userSuccessors.begin()) {
	str << "," ;
      }
      str << *i ;
    }
    str << ")" ;
  }
}

bool  patNode::disconnected() const {
  if (userSuccessors.size() > 0) {
    return false ;
  }
  if (userPredecessors.size() > 0) {
    return false ;
  }
  return true ;
}


patString patNode::getName() const {
  return name ;
}

patIterator<unsigned long>* patNode::getSuccessors() {
  patIterator<unsigned long>* ptr = new patStlSetIterator<unsigned long>(userSuccessors) ;
  return ptr ;
}

unsigned long patNode::getUserId() const {
  return userId ;
}
patArc* patNode::getOutgoingArc(unsigned long down_node_id){
	map<unsigned long, patArc*>::iterator find = outgoingArcs.find(down_node_id);
	if (find==outgoingArcs.end()){
		return NULL;
	}
	else{
		return find->second;
	}
}

patGeoCoordinates patNode::getGeoCoord(){
	return geoCoord;
}
