//----------------------------------------------------------------
// File: patNode.h
// Author: Michel Bierlaire
// Creation: Thu Oct 30 09:33:09 2008
//----------------------------------------------------------------

#ifndef patNode_h
#define patNode_h

#include <set>
#include "patGeoCoordinates.h"
#include "patType.h"
#include "patString.h"
struct node_attributes{
	patString type;
};
#include "patIterator.h"

class patOdJ;

class patNode {

  friend class patDisconnectedNode ;
  friend class patNetwork ;
  friend class patArc ;
  friend class patPathSampling ;//add by Jingmin

  friend class patGpsPoint ;//add by Jingmin
  friend class patShortestPathTreeRange ;//add by Jingmin
   friend class patPathDDR;//add by Jingmin
  friend class patReadSampleFromCsv ;//add by Jingmin
friend class patTripParser;//add by Jingmin
  friend class patOdJ;//add by Jingmin
friend class patPathJ;//add by Jingmin
friend class patReadSample;//add by Jingmin
friend class patSample;//add by Jingmin
friend class patObservation;//add by Jingmin
friend class patTraveler;//add by Jingmin
friend class patTripGraph;//add by Jingmin
friend class patPathProbaAlgoV2;//add by Jingmin
friend class patArcTransition;//added by Jingmin
friend class patWriteBiogemeInput;//added by Jingmin
friend class patSimulator;//added by Jingmin
friend class patMapMatching;//added by Jingmin
friend class patMapMatchingV2;//added by Jingmin
friend class patMapMatchingRoute;//added by Jingmin
friend class patStreetSegment;//added by Jingmin
  friend ostream& operator<<(ostream& str, const patNode& x) ;
		friend patBoolean operator<(const patOdJ& od1, const patOdJ& od2) ;

 public:
  /**
     @param theId User ID of the node
     @param theName Name of the node
     @param lat Latitude of the node
     @param lon Longitude of the node
   */
  patNode(patULong theId, patString theName, patReal lat, patReal lon,struct node_attributes theAttr) ;

  patString getName() const ;


  patIterator<patULong>* getSuccessors() ;

  patULong getUserId() const ;
 protected:
  /**
     @param aSucc user ID of the successor to add
   */
  void addSuccessor(patULong aSucc) ;
  /**
     @param aPred user ID of the predecessor to add
   */
  void addPredecessor(patULong aPred) ;

  /**
   */
  patBoolean disconnected() const ;

  /**
   */
  patBoolean isCentroid ;
 protected:
  patULong userId ;
  patULong internalId ;
  patString name ;
  patGeoCoordinates geoCoord ;
  set<patULong> userPredecessors ;
  set<patULong> userSuccessors ;
  struct node_attributes attributes;
} ;


#endif 
