//----------------------------------------------------------------
// File: patNetwork.cc
// Author: Michel Bierlaire
// Creation: Thu Oct 30 11:06:00 2008
//----------------------------------------------------------------

#include <fstream>
#include "patNetwork.h"
#include "patMath.h"
#include "patConst.h"
#include "patDisplay.h"
#include "patErrMiscError.h"
#include "patErrNullPointer.h"
#include "patGeoCoordinates.h"
#include "patShortestPathAlgoDest.h"
  
patNetwork::patNetwork(patString theName) : 
  name(theName), 
  adjacencyLists_back(NULL),
  adjacencyLists(NULL) {

}

void patNetwork::setMapBounds(patReal minLat, 
			      patReal maxLat, 
			      patReal minLon, 
			      patReal maxLon) {
  minLatitude = minLat ;
  maxLatitude = maxLat ;
  minLongitude = minLon ;
  maxLongitude = maxLon ;
}


patBoolean patNetwork::addNode(const patNode& theNode) {
  map<patULong, patNode>::iterator found = theNodes.find(theNode.userId) ;
  if (found != theNodes.end()) {
    // The node Id already exists
    return patFALSE ;
  } 
  else {
    theNodes.insert(pair<patULong, patNode>(theNode.userId,theNode)) ; ; 
    //    DEBUG_MESSAGE("Node '" << theNode.name << "' added") ;  
    return patTRUE ;
  }
}

patBoolean patNetwork::addArcWithIds(patULong theId, 
				     patULong aNodeId, 
				     patULong bNodeId, 
				     patString theName,
					 struct arc_attributes theAttr,
				     patError*& err) {
  patNode* aNode = getNodeFromUserId(aNodeId) ;
  if (aNode == NULL) {
    err = new patErrNullPointer("patNode") ;
    WARNING(err->describe()) ;
    return patFALSE ;
  }
  patNode* bNode = getNodeFromUserId(bNodeId) ;
  if (bNode == NULL) {
    err = new patErrNullPointer("patNode") ;
    WARNING(err->describe()) ;
    return patFALSE ;
  }
  patBoolean result = addArc(theId,aNode,bNode,theName,theAttr,err) ;
  if (err != NULL) {
    WARNING(err->describe()) ;
    return patFALSE ;
  }
  return result ;
}

patBoolean patNetwork::addArc(patULong theId, 
			      patNode* aNode, 
			      patNode* bNode, 
			      patString theName,
				  struct arc_attributes theAttr,
			      patError*& err) {

  if (aNode == NULL || bNode == NULL) {
    return patFALSE ;
  }
  map<patULong, patArc>::iterator found = theArcs.find(theId) ;
  if (found == theArcs.end()) {
    aNode->addSuccessor(bNode->userId) ;
    bNode->addPredecessor(aNode->userId) ;
    addNode(*aNode) ;
    addNode(*bNode) ;
    patArc newArc(theId,aNode,bNode,theName, theAttr, err) ;
    if (err != NULL) {
      WARNING(err->describe()) ;
      return patFALSE ;
    }
    newArc.polyline.push_back(aNode->geoCoord) ;
    newArc.polyline.push_back(bNode->geoCoord) ;
    theArcs.insert(pair<patULong, patArc>(theId,newArc)) ;
    pair<patULong, patULong> pairOfNodeIds(aNode->userId, bNode->userId) ;
    pair<pair<patULong, patULong>, patULong > elementToInsert(pairOfNodeIds,theId) ;
    listOfArcsPerPairsOfNodes.insert(elementToInsert) ;

    return patTRUE ;
  }
  else {
    return patFALSE ;
  }
}


patNode* patNetwork::getNodeFromUserId(patULong id) {
  map<patULong, patNode>::iterator found = theNodes.find(id) ;
  if (found == theNodes.end()) {
    return NULL ;
  }
  else {
    return &(found->second) ;
  }
}

patArc* patNetwork::getArcFromUserId(patULong id)  {
  map<patULong, patArc>::iterator found = theArcs.find(id) ;
  if (found == theArcs.end()) {
    return NULL ;
  }
  else {
    return &(found->second) ;
  }
}

patArc* patNetwork::getArcFromNodesUserId(patULong aNodeId, patULong bNodeId)  {

  pair<patULong, patULong> elementToFind(aNodeId,bNodeId) ;
  map<pair<patULong, patULong>, patULong >::const_iterator found =
    listOfArcsPerPairsOfNodes.find(elementToFind) ;
  if (found == listOfArcsPerPairsOfNodes.end()) {
    return NULL ;
  }
  else {
    return getArcFromUserId(found->second) ;
  }
}

short patNetwork::isArcOneWay(patULong aNodeId, patULong bNodeId) const {
  
  short result(0) ;
  pair<patULong, patULong> elementToFind(aNodeId,bNodeId) ;
  map<pair<patULong, patULong>, patULong >::const_iterator found =
    listOfArcsPerPairsOfNodes.find(elementToFind) ;
  if (found != listOfArcsPerPairsOfNodes.end()) {
    ++result ;
  }

  elementToFind = pair<patULong, patULong>(bNodeId,aNodeId) ;
  found = listOfArcsPerPairsOfNodes.find(elementToFind) ;
  if (found != listOfArcsPerPairsOfNodes.end()) {
    ++result ;
  }
  return result ;
  
}



void patNetwork::setArcLength(patULong arcId, patReal l, patError*& err) {
  patArc* theArc = getArcFromUserId(arcId) ;
  if (theArc == NULL) {
    stringstream str ;
    str << "Arc " << arcId << " not found" ;
    err = new patErrMiscError(str.str()) ;
    WARNING(err->describe()) ;
    return ;
  }
  theArc->setLength(l) ;
}

void patNetwork::computeArcLength(patULong arcId, patError*& err) {
  patArc* theArc = getArcFromUserId(arcId) ;
  if (theArc == NULL) {
    stringstream str ;
    str << "Arc " << arcId << " not found" ;
    err = new patErrMiscError(str.str()) ;
    WARNING(err->describe()) ;
    return ;
  }
  patNode* upNode = getNodeFromUserId(theArc->upNodeId) ;
  if (upNode == NULL) {
    stringstream str ;
    str << "Node " << theArc->upNodeId << " not found" ;
    err = new patErrMiscError(str.str()) ;
    WARNING(err->describe()) ;
    return ;
  }
  patNode* downNode = getNodeFromUserId(theArc->downNodeId) ;
  if (downNode == NULL) {
    stringstream str ;
    str << "Node " << theArc->downNodeId << " not found" ;
    err = new patErrMiscError(str.str()) ;
    WARNING(err->describe()) ;
    return ;
  }
  theArc->setLength(upNode->geoCoord.distanceTo(downNode->geoCoord)) ;
  return ;
}

patULong patNetwork::nbrOfArcs() const {
  return theArcs.size() ;
}

patULong patNetwork::nbrOfNodes() const {
  return theNodes.size() ;
}

void patNetwork::writeKML(patString fileName, patError*& err) {
  ofstream kml(fileName.c_str()) ;
  kml.precision(15);
  kml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl ;
  kml << "      <kml xmlns=\"http://earth.google.com/kml/2.0\">" << endl ;
  kml << "      <Document>" << endl ;
  patString theName(name) ;
  replaceAll(&theName,patString("&"),patString("and")) ;
  kml << "            <name>" << theName <<"</name>" << endl ;
  kml << "            <description>File created by bioroute (Michel Bierlaire, EPFL)</description>" << endl ;

  for (map<patULong, patNode>::iterator nIter = theNodes.begin() ;
       nIter != theNodes.end() ;
       ++nIter) {
  
    kml << "            <Placemark>" << endl ;
    if (nIter->second.isCentroid) {
      kml << "	<visibility>1</visibility>" << endl ;
    }
    else {
      kml << "	<visibility>0</visibility>" << endl ;
    }
  patString theName(nIter->second.name) ;
  replaceAll(&theName,patString("&"),patString("and")) ;
    kml << "                  <name>" << theName << "</name>" << endl ;
    kml << "                  <description>Id "<< nIter->second.userId <<"</description>" << endl ;
    kml << "                  <Point>" << endl ;
    kml << "                        <coordinates>" 
	<< nIter->second.geoCoord.getKML() << ", 0</coordinates>" << endl ;
    kml << "                  </Point>" << endl ;
    kml << "            </Placemark>" << endl ;
  }
  
  
  for (map<patULong, patArc>::iterator aIter = theArcs.begin() ;
       aIter != theArcs.end() ;
       ++aIter) {
    kml << "            <Placemark>" << endl ;
    patString theName(aIter->second.name) ;
    replaceAll(&theName,patString("&"),patString("and")) ;
    kml << "                  <name>"<< theName <<"</name>" << endl ;
    kml << "                  <description>Arc "<< aIter->second.userId 
	<< " from node "
	<< aIter->second.upNodeId 
	<< " to node "
	<< aIter->second.downNodeId
	<< "</description>" << endl ;
    kml << " " << endl ;
    kml << "                  <Style>" << endl ;
    kml << "                        <LineStyle>" << endl ;
    kml << "                              <color>ff00ff00</color>" << endl ;
    kml << "                              <width>2</width>" << endl ;
    kml << "                        </LineStyle>" << endl ;
    kml << "                  </Style>" << endl ;
    kml << " " << endl ;
    kml << "                  <LineString>" << endl ;
    kml << "                        <coordinates>" << endl ;
    
    list<patGeoCoordinates>::iterator gIter(aIter->second.polyline.begin()) ;
    list<patGeoCoordinates>::iterator hIter(aIter->second.polyline.begin()) ;
    ++hIter ;
    for ( ; hIter != aIter->second.polyline.end() ; ++gIter, ++hIter) {
      
      patReal a1 = gIter->longitudeInDegrees ;
      patReal a2 = gIter->latitudeInDegrees ;
      patReal b1 = hIter->longitudeInDegrees ;
      patReal b2 = hIter->latitudeInDegrees ;
      
      // If the arc is two way, we need to shift it a little bit
      
      //     patReal x1 ;
      //     patReal x2 ;
      //     patReal y1 ;
      //     patReal y2 ;
      
      //     if (patAbs(b1-a1) <= 1.0e-6) {
      //       x1 = a1 + shift ;
      //       x2 = a2 ;
      //       y1 = b1 + shift ;
      //       y2 = b2 ;
      //     }
      //     else {
      //       patReal ratio = (b2-a2)/(b1-a1) ;
      //       x1 = a1 - shift * ratio ;
      //       x2 = a2 + shift ;
      
      //     }
      kml << a1 <<"," << a2 << ",0 "  
	  << b1 <<"," << b2 << ",0" << endl ;
    }
    kml << "</coordinates>	" << endl ;
    kml << "                  </LineString>" << endl ;
    kml << "            </Placemark>" << endl ;
  }
  kml << " " << endl ;
  kml << "      </Document>" << endl ;
  kml << "      </kml>" << endl ;


  kml.close() ;
}


void patNetwork::removeUselessNodes(patError*& err) {


  DEBUG_MESSAGE(theNodes.size() << " nodes") ;
  patBoolean erase(patTRUE) ;

  while (erase) {
    erase = patFALSE ;
    for (map<patULong, patNode>::iterator i = theNodes.begin() ;
	 i != theNodes.end() ;
	 ++i) {
      if (i->second.disconnected()) {
	theNodes.erase(i->first) ;
	erase = patTRUE ;
	break ;
      }
    }
  }

  DEBUG_MESSAGE("After removing disconnected nodes: " << theNodes.size() << " nodes") ;

  erase = patTRUE ;

  while (erase) {
    for (map<patULong, patNode>::iterator i = theNodes.begin() ;
	 i != theNodes.end() ;
	 ++i) {
      erase = removeUselessNode(i->first,err) ;
      if (err != NULL) {
	WARNING(err->describe()) ;
	return ;
      }
      if (erase) {
	break ;
      }
    }
  }

  DEBUG_MESSAGE("After removing intermediary nodes: " << theNodes.size() << " nodes") ;

}
patBoolean patNetwork::removeUselessNode(patULong id, patError*& err) {
  patNode* theNode = getNodeFromUserId(id) ;
  if (theNode == NULL) {
    return patFALSE ;
  }

  if (theNode->userPredecessors.size() != 1) {
    return patFALSE ;
  }
  patULong thePred = *(theNode->userPredecessors.begin()) ;
  patNode* thePredPtr = getNodeFromUserId(thePred) ;
  if (thePredPtr == NULL) {
    err = new patErrNullPointer("patNode") ;
    WARNING(err->describe()) ;
    return patFALSE ;
  }
  if (theNode->userSuccessors.size() != 1) {
    return patFALSE ;
  }
  patULong theSucc = *(theNode->userSuccessors.begin()) ;

  patNode* theSuccPtr = getNodeFromUserId(theSucc) ;
  if (theSuccPtr == NULL) {
    err = new patErrNullPointer("patNode") ;
    WARNING(err->describe()) ;
    return patFALSE ;
  }
  theSuccPtr->userPredecessors.erase(id) ;
  theSuccPtr->userPredecessors.insert(thePred) ;
  thePredPtr->userSuccessors.erase(id) ;
  thePredPtr->userSuccessors.insert(theSucc) ; 

  patArc* arc1 = getArcFromNodesUserId(thePred,theNode->userId) ;
  if (arc1 == NULL) {
    WARNING("arc " << thePred << "->" << theNode->userId) ;
    err = new patErrNullPointer("patArc") ;
    WARNING(err->describe()) ;
    return patFALSE ;
  }
  patArc* arc2 = getArcFromNodesUserId(theNode->userId,theSucc) ;
  if (arc2 == NULL) {
    WARNING("arc " << theNode->userId << "->" << theSucc) ;
    err = new patErrNullPointer("patArc") ;
    WARNING(err->describe()) ;
    return patFALSE ;
  }

  patULong newId = patMin(arc1->userId,arc2->userId) ;
  patString newName ;
  if (arc1->name != arc2->name) {
    newName = arc1->name + " - " + arc2->name ;
  }
  else {
    newName = arc1->name ;
  }
  patArc newArc(newId,thePredPtr,theSuccPtr,newName,err) ;
  if (err != NULL) {
    WARNING(err->describe()) ;
    return patFALSE ;
  }
  newArc.generalizedCost = arc1->generalizedCost + arc2->generalizedCost ;
  newArc.length = arc1->length + arc2->length ;
  newArc.polyline.push_back(thePredPtr->geoCoord) ;
  newArc.polyline.push_back(theNode->geoCoord) ;
  newArc.polyline.push_back(theSuccPtr->geoCoord) ;

  theArcs.erase(arc1->userId) ;  
  theArcs.erase(arc2->userId) ;  
  theArcs.insert(pair<patULong, patArc>(newArc.userId,newArc)) ;

  pair<patULong, patULong> toremove1(thePred,id) ;
  listOfArcsPerPairsOfNodes.erase(toremove1) ;
  pair<patULong, patULong> toremove2(id,theSucc) ;
  listOfArcsPerPairsOfNodes.erase(toremove2) ;
  pair<patULong, patULong> toadd(thePred,theSucc) ;
  pair< pair<patULong, patULong>, patULong > toinsert(toadd,newArc.userId) ;
  listOfArcsPerPairsOfNodes.insert(toinsert) ;


  theNodes.erase(id) ;

  return patTRUE ;
		 
}

void patNetwork::addOd(patULong o, patULong d) {
  theOds.insert(patOd(o,d)) ;
}

void patNetwork::registerOds(patError*& err) {
  DEBUG_MESSAGE("Register " << theOds.size() << " OD pairs") ;
  vector<patULong> unknownNodes ;
  for (set<patOd>::iterator i = theOds.begin() ;
       i != theOds.end() ;
       ++i) {
    DEBUG_MESSAGE("Register " << *i) ;
    patNode* orig = getNodeFromUserId(i->orig) ;
    if (orig == NULL) {
      unknownNodes.push_back(i->orig) ;
    }
    else {
      orig->isCentroid = patTRUE ;
    }
    patNode* dest = getNodeFromUserId(i->dest) ;
    if (dest == NULL) {
      unknownNodes.push_back(i->dest) ;
    }
    else {
      dest->isCentroid = patTRUE ;
    }
  }

  if (!unknownNodes.empty()) {
    stringstream str ;
    str << "The following centroids are unknown nodes: " ;
    for (vector<patULong>::iterator j = unknownNodes.begin() ;
	 j != unknownNodes.end() ;
	 ++j) {
      if (j != unknownNodes.begin()) {
	str << "-" ;
      }
      str << *j ;
    }
    err = new patErrMiscError(str.str()) ;
    WARNING(err->describe()) ;
    return ;
  }
}


patULong patNetwork::getInternalNodeIdFromUserId(patULong userId) const {
  map<patULong, patNode>::const_iterator found = theNodes.find(userId) ;
  if (found == theNodes.end()) {
    return patBadId ;
  }
  else {
    return found->second.internalId ;
  }
}


patULong patNetwork::getInternalArcIdFromUserId(patULong userId) const {
  map<patULong, patArc>::const_iterator found = theArcs.find(userId) ;
  if (found == theArcs.end()) {
    return patBadId ;
  }
  else {
    return found->second.internalId ;
  }

}

void patNetwork::computeInternalIds(patError*& err) {
  patULong arcInternalId(0) ;
  for (map<patULong, patArc>::iterator i = theArcs.begin() ;
       i != theArcs.end() ;
       ++i) {
    i->second.internalId = arcInternalId ;
    internalArcs.push_back(&(i->second)) ;
    ++arcInternalId ;
  }

  patULong nodeInternalId(0) ;
  for (map<patULong, patNode>::iterator i = theNodes.begin() ;
       i != theNodes.end() ;
       ++i) {
    i->second.internalId = nodeInternalId ;
    internalNodes.push_back(&(i->second)) ;
    ++nodeInternalId ;
  }
	
}

void patNetwork::finalizeNetwork(patError*& err) {
  removeUselessNodes(err) ;
  if (err != NULL) {
    WARNING(err->describe()) ;
    return ;
  }
  registerOds(err) ;
  if (err != NULL) {
    WARNING(err->describe()) ;
    return ;
  }
  computeInternalIds(err) ;
  if (err != NULL) {
    WARNING(err->describe()) ;
    return ;
  }

   buildAdjacencyLists_back(err) ;
  if (err != NULL) {
    WARNING(err->describe()) ;
    return ;
  }
  buildAdjacencyLists(err) ;
  if (err != NULL) {
    WARNING(err->describe()) ;
    return ;
  }
  
  computeMinimalLabel(err);
  if (err != NULL) {
    WARNING(err->describe()) ;
    return ;
  }
  patError* err1;
  calArcsHeading(err1);
  if (err != NULL) {
    WARNING(err->describe()) ;
    return ;
  }
}


void patNetwork::buildAdjacencyLists(patError*& err) {
  if (adjacencyLists == NULL) {
    adjacencyLists = new vector< list <  pair<patReal,patULong> > > ;
  }
  else {
    adjacencyLists->erase(adjacencyLists->begin(),adjacencyLists->end()) ;
  }
  for (patULong n = 0 ; n < internalNodes.size() ; ++n) {
    list <  pair<patReal,patULong> >  currentList ;
    patNode* theNode = internalNodes[n] ;
    for (set<patULong>::iterator succ = theNode->userPredecessors.begin() ;
	 succ != theNode->userPredecessors.end() ;
	 ++succ) {
      patArc* theArc = getArcFromNodesUserId(*succ,theNode->userId) ;
      if (theArc == NULL) {
	stringstream str ;
	str << "Arc " << theNode->userId << "->" << *succ << " does not exist" ;
	err = new patErrMiscError(str.str()) ;
	WARNING(err->describe()) ;
	return ;
      }
	  if(theArc->name!="M1" && theArc->attributes.priority>=4 && theArc->attributes.priority<=14 ){
      patULong internalIdSucc = getInternalNodeIdFromUserId(*succ) ;
/*	  if (theArc->generalizedCost<1.0){
		DEBUG_MESSAGE("arc"<<*theArc<<theArc->generalizedCost);
	  }
	  */
	  
      currentList.push_back(pair<patReal,patULong>(theArc->generalizedCost,
						   internalIdSucc)) ;
						   }
    }
    adjacencyLists->push_back(currentList) ;
  }
}

void patNetwork::computeMinimalLabel(patError*& err) {
  minimumLabelForShortestPath = patMaxReal ;
  for (map<patULong, patArc>::iterator i = theArcs.begin() ;
       i != theArcs.end() ;
       ++i) {
    if (i->second.generalizedCost < minimumLabelForShortestPath) {
      minimumLabelForShortestPath = i->second.generalizedCost ;
    }
  }
}

patPath patNetwork::getShortestPath(patOd od, patError*& err) {
  // Should be optimized and called only when the generalized cost
  // have been modified.
  
  computeMinimalLabel(err) ;

  patNode* origNode = getNodeFromUserId(od.orig) ; 
  if (origNode == NULL) {
    stringstream str ;
    str << "Node " << od.orig << " does not exist" ;
    err = new patErrMiscError(str.str()) ;
    WARNING(err->describe());
    return patPath() ;
  }
  patNode* destNode = getNodeFromUserId(od.dest) ; 
  if (destNode == NULL) {
    stringstream str ;
    str << "Node " << od.dest << " does not exist" ;
    err = new patErrMiscError(str.str()) ;
    WARNING(err->describe());
    return patPath() ;
  }

  patShortestPathAlgoDest(destNode->internalId,
			  adjacencyLists,
			  minimumLabelForShortestPath) ;
}

void patNetwork::calArcsHeading(patError*& err){
	for(vector<patArc*>::iterator arcIter = internalArcs.begin();
				arcIter!=internalArcs.end();
				++arcIter
		){
		
		const_cast<patArc*>(*arcIter)->calHeading(this,err);
		
		const_cast<patArc*>(*arcIter)->calPriority();
	}
}


void patNetwork::buildAdjacencyLists_back(patError*& err) {

}
