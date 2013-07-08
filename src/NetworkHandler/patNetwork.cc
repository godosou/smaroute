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

void patNetwork::setMapBounds(double minLat,
			      double maxLat,
			      double minLon,
			      double maxLon) {
  minLatitude = minLat ;
  maxLatitude = maxLat ;
  minLongitude = minLon ;
  maxLongitude = maxLon ;
}


bool patNetwork::addNode(const patNode& theNode) {
  map<unsigned long, patNode>::iterator found = theNodes.find(theNode.userId) ;
  if (found != theNodes.end()) {
    // The node Id already exists
    return false ;
  }
  else {
    theNodes.insert(pair<unsigned long, patNode>(theNode.userId,theNode)) ; ;
    //    DEBUG_MESSAGE("Node '" << theNode.name << "' added") ;
    return true ;
  }
}

bool patNetwork::addArcWithIds(unsigned long theId,
				     unsigned long aNodeId,
				     unsigned long bNodeId,
				     patString theName,
					 struct arc_attributes theAttr,
				     patError*& err) {
  patNode* aNode = getNodeFromUserId(aNodeId) ;
  if (aNode == NULL) {
    err = new patErrNullPointer("patNode") ;
    WARNING(err->describe()) ;
    return false ;
  }
  patNode* bNode = getNodeFromUserId(bNodeId) ;
  if (bNode == NULL) {
    err = new patErrNullPointer("patNode") ;
    WARNING(err->describe()) ;
    return false ;
  }
  bool result = addArc(theId,aNode,bNode,theName,theAttr,err) ;
  if (err != NULL) {
    WARNING(err->describe()) ;
    return false ;
  }
  return result ;
}

bool patNetwork::addArc(unsigned long theId,
			      patNode* aNode,
			      patNode* bNode,
			      patString theName,
				  struct arc_attributes theAttr,
			      patError*& err) {

  if (aNode == NULL || bNode == NULL) {
    return false ;
  }
  map<unsigned long, patArc>::iterator found = theArcs.find(theId) ;
  if (found == theArcs.end()) {
    aNode->addSuccessor(bNode->userId) ;
    bNode->addPredecessor(aNode->userId) ;
    addNode(*aNode) ;
    addNode(*bNode) ;
    patArc newArc(theId,aNode,bNode,theName, theAttr, err) ;
    if (err != NULL) {
      WARNING(err->describe()) ;
      return false ;
    }
    newArc.m_polyline.push_back(aNode->geoCoord) ;
    newArc.m_polyline.push_back(bNode->geoCoord) ;
    theArcs.insert(pair<unsigned long, patArc>(theId,newArc)) ;
    pair<unsigned long, unsigned long> pairOfNodeIds(aNode->userId, bNode->userId) ;
    pair<pair<unsigned long, unsigned long>, unsigned long > elementToInsert(pairOfNodeIds,theId) ;
    listOfArcsPerPairsOfNodes.insert(elementToInsert) ;

    return true ;
  }
  else {
    return false ;
  }
}


patNode* patNetwork::getNodeFromUserId(unsigned long id) {
  map<unsigned long, patNode>::iterator found = theNodes.find(id) ;
  if (found == theNodes.end()) {
    return NULL ;
  }
  else {
    return &(found->second) ;
  }
}

patArc* patNetwork::getArcFromUserId(unsigned long id)  {
  map<unsigned long, patArc>::iterator found = theArcs.find(id) ;
  if (found == theArcs.end()) {
    return NULL ;
  }
  else {
    return &(found->second) ;
  }
}

patArc* patNetwork::getArcFromNodesUserId(unsigned long aNodeId, unsigned long bNodeId)  {

  pair<unsigned long, unsigned long> elementToFind(aNodeId,bNodeId) ;
  map<pair<unsigned long, unsigned long>, unsigned long >::const_iterator found =
    listOfArcsPerPairsOfNodes.find(elementToFind) ;
  if (found == listOfArcsPerPairsOfNodes.end()) {
    return NULL ;
  }
  else {
    return getArcFromUserId(found->second) ;
  }
}

short patNetwork::isArcOneWay(unsigned long aNodeId, unsigned long bNodeId) const {

  short result(0) ;
  pair<unsigned long, unsigned long> elementToFind(aNodeId,bNodeId) ;
  map<pair<unsigned long, unsigned long>, unsigned long >::const_iterator found =
    listOfArcsPerPairsOfNodes.find(elementToFind) ;
  if (found != listOfArcsPerPairsOfNodes.end()) {
    ++result ;
  }

  elementToFind = pair<unsigned long, unsigned long>(bNodeId,aNodeId) ;
  found = listOfArcsPerPairsOfNodes.find(elementToFind) ;
  if (found != listOfArcsPerPairsOfNodes.end()) {
    ++result ;
  }
  return result ;

}



void patNetwork::setArcLength(unsigned long arcId, double l, patError*& err) {
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

void patNetwork::computeArcLength(unsigned long arcId, patError*& err) {
  patArc* theArc = getArcFromUserId(arcId) ;
  if (theArc == NULL) {
    stringstream str ;
    str << "Arc " << arcId << " not found" ;
    err = new patErrMiscError(str.str()) ;
    WARNING(err->describe()) ;
    return ;
  }
  patNode* upNode = getNodeFromUserId(theArc->m_up_node_id) ;
  if (upNode == NULL) {
    stringstream str ;
    str << "Node " << theArc->m_up_node_id << " not found" ;
    err = new patErrMiscError(str.str()) ;
    WARNING(err->describe()) ;
    return ;
  }
  patNode* downNode = getNodeFromUserId(theArc->m_down_node_id) ;
  if (downNode == NULL) {
    stringstream str ;
    str << "Node " << theArc->m_down_node_id << " not found" ;
    err = new patErrMiscError(str.str()) ;
    WARNING(err->describe()) ;
    return ;
  }
  theArc->setLength(upNode->geoCoord.distanceTo(downNode->geoCoord)) ;
  return ;
}

unsigned long patNetwork::nbrOfArcs() const {
  return theArcs.size() ;
}

unsigned long patNetwork::nbrOfNodes() const {
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

  for (map<unsigned long, patNode>::iterator nIter = theNodes.begin() ;
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


  for (map<unsigned long, patArc>::iterator aIter = theArcs.begin() ;
       aIter != theArcs.end() ;
       ++aIter) {
    kml << "            <Placemark>" << endl ;
    patString theName(aIter->second.m_name) ;
    replaceAll(&theName,patString("&"),patString("and")) ;
    kml << "                  <name>"<< theName <<"</name>" << endl ;
    kml << "                  <description>Arc "<< aIter->second.m_user_id
	<< " from node "
	<< aIter->second.m_up_node_id
	<< " to node "
	<< aIter->second.m_down_node_id
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

    list<patGeoCoordinates>::iterator gIter(aIter->second.m_polyline.begin()) ;
    list<patGeoCoordinates>::iterator hIter(aIter->second.m_polyline.begin()) ;
    ++hIter ;
    for ( ; hIter != aIter->second.m_polyline.end() ; ++gIter, ++hIter) {

      double a1 = gIter->longitudeInDegrees ;
      double a2 = gIter->latitudeInDegrees ;
      double b1 = hIter->longitudeInDegrees ;
      double b2 = hIter->latitudeInDegrees ;

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
  bool erase(true) ;

  while (erase) {
    erase = false ;
    for (map<unsigned long, patNode>::iterator i = theNodes.begin() ;
	 i != theNodes.end() ;
	 ++i) {
      if (i->second.disconnected()) {
	theNodes.erase(i->first) ;
	erase = true ;
	break ;
      }
    }
  }

  DEBUG_MESSAGE("After removing disconnected nodes: " << theNodes.size() << " nodes") ;

  erase = true ;

  while (erase) {
    for (map<unsigned long, patNode>::iterator i = theNodes.begin() ;
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
bool patNetwork::removeUselessNode(unsigned long id, patError*& err) {
  patNode* theNode = getNodeFromUserId(id) ;
  if (theNode == NULL) {
    return false ;
  }

  if (theNode->userPredecessors.size() != 1) {
    return false ;
  }
  unsigned long thePred = *(theNode->userPredecessors.begin()) ;
  patNode* thePredPtr = getNodeFromUserId(thePred) ;
  if (thePredPtr == NULL) {
    err = new patErrNullPointer("patNode") ;
    WARNING(err->describe()) ;
    return false ;
  }
  if (theNode->userSuccessors.size() != 1) {
    return false ;
  }
  unsigned long theSucc = *(theNode->userSuccessors.begin()) ;

  patNode* theSuccPtr = getNodeFromUserId(theSucc) ;
  if (theSuccPtr == NULL) {
    err = new patErrNullPointer("patNode") ;
    WARNING(err->describe()) ;
    return false ;
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
    return false ;
  }
  patArc* arc2 = getArcFromNodesUserId(theNode->userId,theSucc) ;
  if (arc2 == NULL) {
    WARNING("arc " << theNode->userId << "->" << theSucc) ;
    err = new patErrNullPointer("patArc") ;
    WARNING(err->describe()) ;
    return false ;
  }

  unsigned long newId = patMin(arc1->m_user_id,arc2->m_user_id) ;
  patString newName ;
  if (arc1->m_name != arc2->m_name) {
    newName = arc1->m_name + " - " + arc2->m_name ;
  }
  else {
    newName = arc1->m_name ;
  }
  patArc newArc(newId,thePredPtr,theSuccPtr,newName,err) ;
  if (err != NULL) {
    WARNING(err->describe()) ;
    return false ;
  }
  newArc.generalizedCost = arc1->generalizedCost + arc2->generalizedCost ;
  newArc.length = arc1->length + arc2->length ;
  newArc.m_polyline.push_back(thePredPtr->geoCoord) ;
  newArc.m_polyline.push_back(theNode->geoCoord) ;
  newArc.m_polyline.push_back(theSuccPtr->geoCoord) ;

  theArcs.erase(arc1->m_user_id) ;
  theArcs.erase(arc2->m_user_id) ;
  theArcs.insert(pair<unsigned long, patArc>(newArc.m_user_id,newArc)) ;

  pair<unsigned long, unsigned long> toremove1(thePred,id) ;
  listOfArcsPerPairsOfNodes.erase(toremove1) ;
  pair<unsigned long, unsigned long> toremove2(id,theSucc) ;
  listOfArcsPerPairsOfNodes.erase(toremove2) ;
  pair<unsigned long, unsigned long> toadd(thePred,theSucc) ;
  pair< pair<unsigned long, unsigned long>, unsigned long > toinsert(toadd,newArc.m_user_id) ;
  listOfArcsPerPairsOfNodes.insert(toinsert) ;


  theNodes.erase(id) ;

  return true ;

}

void patNetwork::addOd(unsigned long o, unsigned long d) {
  theOds.insert(patOd(o,d)) ;
}

void patNetwork::registerOds(patError*& err) {
  DEBUG_MESSAGE("Register " << theOds.size() << " OD pairs") ;
  vector<unsigned long> unknownNodes ;
  for (set<patOd>::iterator i = theOds.begin() ;
       i != theOds.end() ;
       ++i) {
    DEBUG_MESSAGE("Register " << *i) ;
    patNode* orig = getNodeFromUserId(i->orig) ;
    if (orig == NULL) {
      unknownNodes.push_back(i->orig) ;
    }
    else {
      orig->isCentroid = true ;
    }
    patNode* dest = getNodeFromUserId(i->dest) ;
    if (dest == NULL) {
      unknownNodes.push_back(i->dest) ;
    }
    else {
      dest->isCentroid = true ;
    }
  }

  if (!unknownNodes.empty()) {
    stringstream str ;
    str << "The following centroids are unknown nodes: " ;
    for (vector<unsigned long>::iterator j = unknownNodes.begin() ;
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


unsigned long patNetwork::getInternalNodeIdFromUserId(unsigned long userId) const {
  map<unsigned long, patNode>::const_iterator found = theNodes.find(userId) ;
  if (found == theNodes.end()) {
    return patBadId ;
  }
  else {
    return found->second.internalId ;
  }
}


unsigned long patNetwork::getInternalArcIdFromUserId(unsigned long userId) const {
  map<unsigned long, patArc>::const_iterator found = theArcs.find(userId) ;
  if (found == theArcs.end()) {
    return patBadId ;
  }
  else {
    return found->second.internalId ;
  }

}

void patNetwork::computeInternalIds(patError*& err) {
  unsigned long arcInternalId(0) ;
  for (map<unsigned long, patArc>::iterator i = theArcs.begin() ;
       i != theArcs.end() ;
       ++i) {
    i->second.internalId = arcInternalId ;
    internalArcs.push_back(&(i->second)) ;
    ++arcInternalId ;
  }

  unsigned long nodeInternalId(0) ;
  for (map<unsigned long, patNode>::iterator i = theNodes.begin() ;
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
    adjacencyLists = new vector< list <  pair<double,unsigned long> > > ;
  }
  else {
    adjacencyLists->erase(adjacencyLists->begin(),adjacencyLists->end()) ;
  }
  for (unsigned long n = 0 ; n < internalNodes.size() ; ++n) {
    list <  pair<double,unsigned long> >  currentList ;
    patNode* theNode = internalNodes[n] ;
    for (set<unsigned long>::iterator succ = theNode->userPredecessors.begin() ;
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
	  if(theArc->m_name!="M1" && theArc->m_attributes.priority>=4 && theArc->m_attributes.priority<=14 ){
      unsigned long internalIdSucc = getInternalNodeIdFromUserId(*succ) ;
/*	  if (theArc->generalizedCost<1.0){
		DEBUG_MESSAGE("arc"<<*theArc<<theArc->generalizedCost);
	  }
	  */

      currentList.push_back(pair<double,unsigned long>(theArc->generalizedCost,
						   internalIdSucc)) ;
						   }
    }
    adjacencyLists->push_back(currentList) ;
  }
}

void patNetwork::computeMinimalLabel(patError*& err) {
  minimumLabelForShortestPath = DBL_MAX ;
  for (map<unsigned long, patArc>::iterator i = theArcs.begin() ;
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
