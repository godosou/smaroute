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
 vector< list <  pair<patArc*,unsigned long> > >*  adjList_arc;//added by Jingmin
 
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
  void setMapBounds(double minLat, 
		    double maxLat, 
		    double minLon, 
		    double maxLon) ;
  

  /**
     @param theNode node to be added 
     @return TRUE if the node has been
     added, FALSE otherwise. In particular, if the node already exists,
     it is not added.
   */
  bool addNode(const patNode& theNode) ;

  /**
     @param theId  Unique arc identifier
     @param aNode Upstream node of the arc. If it does not exist in
     the network, it will be added.
     @param bNode Downstream node of the arc. If it does not exist in
     the network, it will be added.
     @param theName name of the arc
     @return TRUE is the arc has been added, FALSE otherwise.  
  */
  bool addArc(unsigned long theId, 
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
  bool addArcWithIds(unsigned long theId, 
			   unsigned long aNodeId,
			   unsigned long bNodeId, 
			   patString theName,
			   struct arc_attributes  theAttr,
			   patError*& err) ;
  
  /**
     @param id  User ID
     @return Pointer to the node if it exists. NULL otherwise.
   */
  patNode* getNodeFromUserId(unsigned long id) ;
  /**
     @param id  User ID
     @return Pointer to the arc if it exists. NULL otherwise.
   */
  patArc* getArcFromUserId(unsigned long id) ;

  /**
     @param aNodeId User ID of the upstream node
     @param bNodeId User ID of the downstream node
     @return Pointer to the arc if it exists. NULL otherwise.
   */
  patArc* getArcFromNodesUserId(unsigned long aNodeId, unsigned long bNodeId)  ;

  /**
     @param aNodeId User ID of a node
     @param bNodeId User ID of a node
     @return Number of arcs adjacent to both nodes. Typically 0 (no arc linking the nodes), 1 (one way), 2 (two ways). 
   */
  short isArcOneWay(unsigned long aNodeId, unsigned long bNodeId) const ;

  /**
   */
  void setArcLength(unsigned long arcId, double l, patError*& err) ;

  /**
   */
  void computeArcLength(unsigned long arcId, patError*& err) ;

  /**
   */
  unsigned long nbrOfArcs() const ;

  /**
   */
  unsigned long nbrOfNodes() const ;

  /**
   */
  void writeKML(patString fileName, patError*& err) ;


  /**
   */
  void addOd(unsigned long o, unsigned long d) ;

  /**
     @return patBadId if the node userId does not exists
   */
  unsigned long getInternalNodeIdFromUserId(unsigned long userId) const ;

  /**
     @return patBadId if the arc userId does not exists
   */
  unsigned long getInternalArcIdFromUserId(unsigned long userId) const ;

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
  bool removeUselessNode(unsigned long id, patError*& err) ;

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
  map<unsigned long, patArc> theArcs ;
  map<unsigned long, patNode> theNodes ;
  vector<patArc*> internalArcs ;
  vector<patNode*> internalNodes ;
  set<patOd> theOds ;
  map<pair<unsigned long, unsigned long>, unsigned long > listOfArcsPerPairsOfNodes ;
  double minLatitude ;
  double maxLatitude ;
  double minLongitude ;
  double maxLongitude ;
  double minimumLabelForShortestPath ;
  vector< list <  pair<double,unsigned long> > >* adjacencyLists ;
  vector< list <  pair<double,unsigned long> > >* adjacencyLists_back ;

};
#endif
