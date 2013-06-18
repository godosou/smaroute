#ifndef patPathJ_h
#define patPathJ_h

#include <list>
#include "patType.h"
#include "patString.h"
#include "patArc.h"
#include <iostream>
#include "patOdJ.h"

#include "patTransportMode.h"
#include "patNetworkElements.h"
#include "patArcSequence.h"

struct path_attributes {
	unsigned long leftTurn;
	unsigned long rightTurn;
	unsigned long straightTurn;
	unsigned long uTurn;
	double length;
	unsigned long nbrOfTrafficSignals;
};

class patGpsPoint;
class patPathJ: public patArcSequence {
public:
	/**
	 *Constructor
	 */
	patPathJ();
	patPathJ(list<patNode*> listOfNodes, patNetwork* theNetwork,
			patError*& err);
	patPathJ(list<patArc*> theListOfArcs);
	unsigned long nbrOfArcs();
	friend ostream& operator<<(ostream& str, const patPathJ& x);

	/**
	 compare if two paths are the same
	 */
	friend bool operator==(const patPathJ& path1, const patPathJ& path2);
	friend bool operator!=(const patPathJ& path1, const patPathJ& path2);

	friend bool operator<(const patPathJ& path1, const patPathJ& path2);
	//bool operator==(const patPathJ& aPath) const;
	//bool operator!=(const patPathJ& aPath) const;

	/**
	 *Add an arc to the front of listOfArcs
	 *@param theArc arc to be added.
	 *@param t_m the transport mode.
	 */
	void addArcToBack(patArc* theArc, TransportMode t_m);

	/**
	 *Generate Od of the path according to arc sequence
	 *@return Od of the path
	 */
	patOd generateOd(patNetwork* theNetwork, patError*& err);

	patOd generateOd();

	/**
	 *Assign an Od to the path
	 *@param theOd pointer of od to be assigned
	 */
	void assignOd(patOd* theOd);

	/**
	 *get gamma length defined by ramming's path size method
	 *@return gamma length
	 */
	double getPathLengthG(double tmpLength);

	/**
	 *Compute path size
	 */
	void computePathSize(patError*& err);

	/**
	 *Get path size, if not exists, calculate it
	 *@return path size
	 */
	double getPathSize(patError*& err);

	/**
	 * assign an id to path
	 *@param theId id to be assigned
	 */
	void assignId(const unsigned long theId);
	/**
	 * get od of the path
	 *@return pointer of the Od
	 */
	patOd* getOd();
	patString genDescription();
	unsigned long getId();
	int size();
	unsigned long getLeftTurns();
	unsigned long getRightTurns();
	unsigned long getStraightTurns();
	unsigned long getUTurns();
	unsigned long getTrafficSignals();
	void computeTrafficSignals(patNetwork* thNetwork);
	void calTurns(patNetwork* theNetwork);

	bool isValidPath(vector<patGpsPoint>* gpsSequence);
	double computePointDDRRaw(const map<patArc*, double>* currLinkDDR) const;
	bool append(list<patArc*>* newSeg);
	/**
	 * Append a new segment to the current path. The connection node should be consistent.
	 * @param newSeg, the pointer to the downstream path.
	 */
	bool append(patPathJ* newSeg);
	unsigned long endNodeUserId();
	/**
	 * Join a new path bPath. The first arc of bPath and the last arc of the current path is the same.
	 * @param bPath: the path to be joined
	 */
	bool join(patPathJ bPath);

	unsigned long getSubPath(patPathJ* newPath, patNode* startNode,
			patNode* endNode);
	bool containLoop();

	list<pair<patArc*, TransportMode> > getSeg(patArc* aArc, patArc* bArc);

	double getPerPrimaryLinkRd();
	double getPerTrunkLinkRd();
	double getPerMotorwayLinkRd();
	double getPerBridleRd();
	double getPerResidentialRd();
	double getPerUnclassifiedRd();
	double getPerTertiaryRd();
	double getPerSecondaryRd();
	double getPerPrimaryRd();
	double getPerTrunkRd();
	double getPerMotorwayRd();
	bool empty();

	/**
	 * Multi-modal path definition
	 */
	patPathJ(list<pair<patArc*, TransportMode> > theListOfArcs);
	/**
	 * Get multimodal path segs;
	 * @param aArc Begin arc.
	 * @param bArc End arc.
	 * @return list of arcs with modes;
	 */
	list<pair<patArc*, TransportMode> > getMultimodalSeg(patArc* aArc,
			patArc* bArc);

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
	vector<double> getIntermediateUnimodalLengths();

	/**
	 * Get modes of each unimodal segment;
	 */
	vector<TransportMode> getUnimodalModes();
	/**
	 * Read a path from shape files .shp and .dbf.
	 */
	bool readShpFile(string file_path, patNetworkElements* network,
			patError*& err);
	bool getArcsFromOSMIds(patNetworkElements* network, patError*& err);

	/**
	 * Export the path to shape files.
	 * @param network The network.
	 * @param file_path The path of th export files
	 * @param err error pointer.
	 * @return true if successful, false otherwise.
	 */
	bool exportShpFiles(string file_path, patNetworkElements* network,
			patError*& err);

protected:
	list<TransportMode> m_modes;
	vector<short> m_change_points;
	patOd* m_od;
	patString m_name;
	unsigned long m_id;
//	patNetwork* theNetwork;
	struct path_attributes m_attributes;

	/**
	 List of the internal id of the links
	 */

};
#endif 
