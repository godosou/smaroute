
#ifndef patPathJ_h
#define patPathJ_h

#include <list>
#include "patType.h"
#include "patString.h"
#include "patNetwork.h"
#include "patArc.h"
#include <iostream>
#include "patOdJ.h"


#include "patTransportMode.h"
#include "patNetworkElements.h"
struct path_attributes{
	patULong leftTurn;
	patULong rightTurn;
	patULong straightTurn;
	patULong uTurn;
	patReal length;
	patULong nbrOfTrafficSignals;
};


class patGpsPoint;
class patPathJ {
 public:
 /**
 *Constructor
 */
 patPathJ() ;
 patPathJ(list<patNode* > listOfNodes,patNetwork* theNetwork,patError*& err) ;
 patPathJ(list<patArc*> theListOfArcs);
 patULong nbrOfArcs();
   friend ostream& operator<<(ostream& str, const patPathJ& x) ;
   
   /**
   compare if two paths are the same
   */
  friend bool operator==(const patPathJ& path1, const patPathJ& path2) ;
friend bool operator!=(const patPathJ& path1, const patPathJ& path2) ;

friend bool operator<(const patPathJ& path1, const patPathJ& path2) ;
	//bool operator==(const patPathJ& aPath) const;
	//bool operator!=(const patPathJ& aPath) const;

  /**
  *Add an arc to the front of listOfArcs
  *@param theArc arc to be added
  */
   void addArcToFront(patArc* theArc);
   /**
   *Add an arc to the front of listOfArcs
   *@param theArc arc to be added.
   *@param t_m the transport mode.
   */
   void addArcToBack(patArc* theArc, TransportMode t_m);

  /**
  *Add an arc to the back of listOfArcs
  *@param theArc arc to be added
  */
  void addArcToBack(patArc* theArc);
  
  /**
  *Generate Od of the path according to arc sequence
  *@return Od of the path
  */
  patOdJ generateOd(patNetwork* theNetwork
			,patError*& err);
  
  /**
  *Assign an Od to the path
  *@param theOd pointer of od to be assigned
  */
  void assignOd(patOdJ* theOd);
  
  /**
  *Check if an arc is on the path
  *@param theArc pointer of arc to be checked
  *@return TRUE if theArc in path, FALSE no
  */
  patBoolean isLinkInPath(patArc* theArc) const;
  
set<patArc*> getDistinctArcs();
  /**
  *Compute length of path
  */
  patReal computePathLength();
  
  /**
  *Get length of path. If not exists, calculate it.
  *@return length of path
  */
  patReal getPathLength();
  /**
  *get gamma length defined by ramming's path size method
  *@return gamma length
  */
  patReal getPathLengthG(patReal tmpLength);
  
  /**
  *Compute path size 
  */
  void computePathSize(patError*& err);
  
  /**
  *Get path size, if not exists, calculate it
  *@return path size
  */
  patReal getPathSize(patError*& err);
  
  /**
  * assign an id to path
  *@param theId id to be assigned
  */
  void assignId(const patULong theId);
   /**
  * get od of the path
  *@return pointer of the Od
  */
  patOdJ* getOd();
  patString genDescription();
  patULong getId();
  
patULong getLeftTurns();
patULong getRightTurns();
patULong getStraightTurns();
patULong getUTurns();
patULong getTrafficSignals();
void computeTrafficSignals(patNetwork* thNetwork);
void calTurns(patNetwork* theNetwork);
  /**
  * get list of arc
  *@return listOfArc
  */
  list<patArc*>* getArcList();
  patArc* getArc(patULong iArc);
  
  patArc* deleteArcFromBack();
  patArc* back();
  patArc* front();
  patArc* back2();
  patBoolean isValidPath(vector<patGpsPoint>* gpsSequence);
  bool isValidPath();
patReal computePointDDRRaw(map<patArc*,patReal>* currLinkDDR);
patBoolean append(list<patArc*>* newSeg);
patBoolean append(patPathJ* newSeg);
patULong endNodeUserId();
	patBoolean join(patPathJ bPath);
	
 patULong getSubPath(patPathJ* newPath, patNode* startNode, patNode* endNode);
patBoolean containLoop();

list<pair<patArc*, TransportMode> >   getSeg(patArc* aArc, patArc* bArc);
	
	patReal getPerPrimaryLinkRd();
	patReal getPerTrunkLinkRd();
	patReal getPerMotorwayLinkRd();
	patReal getPerBridleRd();
	patReal getPerResidentialRd();
	patReal getPerUnclassifiedRd();
	patReal getPerTertiaryRd();
	patReal getPerSecondaryRd();
	patReal getPerPrimaryRd();
	patReal getPerTrunkRd();
	patReal getPerMotorwayRd();
	patBoolean empty();

	/**
	 * Multi-modal path definition
	 */
	patPathJ(
			list<pair<patArc*, TransportMode> > theListOfArcs);
	/**
	 * Get multimodal path segs;
	 * @param aArc Begin arc.
	 * @param bArc End arc.
	 * @return list of arcs with modes;
	 */
	list<pair<patArc*, TransportMode> >  getMultimodalSeg(patArc* aArc, patArc* bArc);

	/**
	 * Determine whether the path is unimodal or not.
	 * @return true if it is unimodal; false otherwise;
	 */
	bool isUniModal();

	/**
	 * Get the set of modes.
	 * @return The set of modes.
	 */
	set<TransportMode> getUniqueModes();

	/**
	 * Get the number of modes.
	 * @return The number of modes.
	 */
	short getNbrOfUniqueModes();
	/**
	 *
	 * Determine the change points for multimodal path;
	 */
	void detChangePoints();

	/**
	 * Get the number of change points;
	 * @return The number of change points.
	 */
	short getNbrOfChangePoints();
	void assignModeForNoInformation();
	void setUnimodalTransportMode(TransportMode m);
	  /**
	   * Get unimodal intermediate path segments' length
	   */
	  vector<patReal> getIntermediateUnimodalLengths();

	  /**
	   * Get modes of each unimodal segment;
	   */
	  vector<TransportMode> getUnimodalModes();
	/**
	 * Read a path from shape files .shp and .dbf.
	 */
	bool readShpFile(string file_path, patNetworkElements* network,patError*& err);
	bool getArcsFromOSMIds(patNetworkElements* network, patError*& err);


  protected:
  list<patArc*> listOfArcs ;
	list<TransportMode> modes;
	vector<short> change_points;
  patOdJ* od ;
  patString name;
  patULong Id;
//	patNetwork* theNetwork;
  struct path_attributes  attributes;
  
  /**
     List of the internal id of the links
   */


};
#endif 
