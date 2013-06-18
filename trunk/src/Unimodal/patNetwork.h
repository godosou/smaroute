//----------------------------------------------------------------
// File: patNetwork.h
// Author: Michel Bierlaire
// Creation: Thu Oct 30 09:46:33 2008
//----------------------------------------------------------------

#ifndef patNetwork_h
#define patNetwork_h

#include <map>
#include <set>
#include "patNode.h"
#include "patArc.h"
#include "patPath.h"
#include "patError.h"
#include "patOd.h"


class patNetwork {
friend class patGpsPoint ;//add by Jingmin
friend class patTrip ;//add by Jingmin
friend class patShortestPathAlgoRange ;//add by Jingmin
friend class patShortestPathTreeRange ;//add by Jingmin
friend class patReadSampleFromCsv ;//add by Jingmin
friend class patOdJ;//add by Jingmin
friend class patPathJ;//add by Jingmin
friend class patReadSample;//add by Jingmin
friend class patSample;//add by Jingmin
friend class patObservation;//add by Jingmin
friend class patTraveler;//add by Jingmin
friend class patTripParser;//add by Jingmin
friend class patTripGraph;//add by Jingmin
friend class patMapMatching;//added by Jingmin
friend class patMapMatchingV2;//added by Jingmin

friend class patMapMatchingRoute;//added by Jingmin
friend class patPathSampling ;//add by Jingmin

 public:
 vector< list <  pair<patArc*,patULong> > >*  adjList_arc;//added by Jingmin
 
  /**
   * Constructor
   * @param theName name of the network
   */

  patNetwork(patString theName) ;

  /**
   * @param minLat latitude of the south-most point in the network
   * @param maxLat latitude of the north-most point in the network
   * @param minLon longitude of the west-most point in the network
   * @param maxLon longitude of the east-most point in the network
   */
  void setMapBounds(patReal minLat, 
		    patReal maxLat, 
		    patReal minLon, 
		    patReal maxLon) ;
  

  /**
     @param theNode node to be added 
     @return TRUE if the node has been
     added, FALSE otherwise. In particular, if the node already exists,
     it is not added.
   */
  patBoolean addNode(const patNode& theNode) ;

  /**
     @param theId  Unique arc identifier
     @param aNode Upstream node of the arc. If it does not exist in
     the network, it will be added.
     @param bNode Downstream node of the arc. If it does not exist in
     the network, it will be added.
     @param theName name of the arc
     @return TRUE is the arc has been added, FALSE otherwise.  
  */
  patBoolean addArc(patULong theId, 
		    patNode* aNode,
		    patNode* bNode, 
		    patString theName,
			struct arc_attributes  theAttr,
		    patError*& err) ;

  /**
     @param theId  Unique arc identifier
     @param aNodeId Id of the upstream node of the arc. 
     @param bNodeId ID of the downstream node of the arc.
     @param theName name of the arc
     @return TRUE is the arc has been added, FALSE otherwise.  
  */
  patBoolean addArcWithIds(patULong theId, 
			   patULong aNodeId,
			   patULong bNodeId, 
			   patString theName,
			   struct arc_attributes  theAttr,
			   patError*& err) ;
  
  /**
     @param id  User ID
     @return Pointer to the node if it exists. NULL otherwise.
   */
  patNode* getNodeFromUserId(patULong id) ;
  /**
     @param id  User ID
     @return Pointer to the arc if it exists. NULL otherwise.
   */
  patArc* getArcFromUserId(patULong id) ;

  /**
     @param aNodeId User ID of the upstream node
     @param bNodeId User ID of the downstream node
     @return Pointer to the arc if it exists. NULL otherwise.
   */
  patArc* getArcFromNodesUserId(patULong aNodeId, patULong bNodeId)  ;

  /**
     @param aNodeId User ID of a node
     @param bNodeId User ID of a node
     @return Number of arcs adjacent to both nodes. Typically 0 (no arc linking the nodes), 1 (one way), 2 (two ways). 
   */
  short isArcOneWay(patULong aNodeId, patULong bNodeId) const ;

  /**
   */
  void setArcLength(patULong arcId, patReal l, patError*& err) ;

  /**
   */
  void computeArcLength(patULong arcId, patError*& err) ;

  /**
   */
  patULong nbrOfArcs() const ;

  /**
   */
  patULong nbrOfNodes() const ;

  /**
   */
  void writeKML(patString fileName, patError*& err) ;


  /**
   */
  void addOd(patULong o, patULong d) ;

  /**
     @return patBadId if the node userId does not exists
   */
  patULong getInternalNodeIdFromUserId(patULong userId) const ;

  /**
     @return patBadId if the arc userId does not exists
   */
  patULong getInternalArcIdFromUserId(patULong userId) const ;

  /**
     This function should be cvalled after the network description has been loaded.
   */
  void finalizeNetwork(patError*& err) ;

 protected:

  /**
   */
  void buildAdjacencyLists(patError*& err) ;

  /**
   */
  void computeMinimalLabel(patError*& err) ;

  /**
   */
  void removeUselessNodes(patError*& err) ;
  /**
     @return patTRUE if the node has been removed. The iteratord may
     not be valid anymore.
   */
  patBoolean removeUselessNode(patULong id, patError*& err) ;

  /**
     Internal IDs are consecutive, and correspond to indices in a vector 
   */
  void computeInternalIds(patError*& err) ;
  /**
   */
  void registerOds(patError*& err) ;

  /**
     orig and dest are user IDs
   */
   
void buildAdjacencyLists_back(patError*& err);
  patPath getShortestPath(patOd od, patError*& err) ;
void calArcsHeading(patError*& err);
   protected:
  patString name ;
  map<patULong, patArc> theArcs ;
  map<patULong, patNode> theNodes ;
  vector<patArc*> internalArcs ;
  vector<patNode*> internalNodes ;
  set<patOd> theOds ;
  map<pair<patULong, patULong>, patULong > listOfArcsPerPairsOfNodes ;
  patReal minLatitude ;
  patReal maxLatitude ;
  patReal minLongitude ;
  patReal maxLongitude ;
  patReal minimumLabelForShortestPath ;
  vector< list <  pair<patReal,patULong> > >* adjacencyLists ;
  vector< list <  pair<patReal,patULong> > >* adjacencyLists_back ;

};
#endif
