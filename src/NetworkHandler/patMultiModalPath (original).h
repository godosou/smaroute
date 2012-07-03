#ifndef PATMULTIMODALPATH_h
#define PATMULTIMODALPATH_h

#include <list>
#include "patType.h"
#include "patString.h"
#include "patArc.h"
#include <iostream>
#include "patOD.h"
#include "patTransportMode.h"
#include "patNetworkElements.h"
#include "patArcSequence.h"
#include "patNetworkEnvironment.h"
#include "kml/dom.h"
using kmldom::FolderPtr;
class patGpsDDR;

struct RoadTravel {
	const patRoadBase* road;
	TransportMode mode;
	double stop_time;
};
struct path_attributes {
	unsigned long leftTurn;
	unsigned long rightTurn;
	unsigned long straightTurn;
	unsigned long uTurn;
	double length;
	unsigned long nbrOfTrafficSignals;
};

class patGpsPoint;
class patMultiModalPath: public patArcSequence {
public:
	/**
	 *Constructor
	 */
	patMultiModalPath();
	patMultiModalPath(list<const patArc*> theListOfArcs, bool& success);
	patMultiModalPath(list<const patRoadBase*> theListOfArcs, bool& success);
	patMultiModalPath(const patMultiModalPath& other);
	/**
	 * Multi-modal path definition
	 */
	patMultiModalPath(list<pair<const patArc*, TransportMode> > theListOfArcs
			,bool& success);

	friend ostream& operator<<(ostream& str, const patMultiModalPath& x);

	/**
	 compare if two paths are the same
	 */
	friend bool operator==(const patMultiModalPath& path1,
			const patMultiModalPath& path2);
	friend bool operator!=(const patMultiModalPath& path1,
			const patMultiModalPath& path2);

	friend bool operator<(const patMultiModalPath& path1,
			const patMultiModalPath& path2);

	/**
	 *Add an arc to the front of listOfArcs
	 *@param theArc arc to be added.
	 *@param t_m the transport mode.
	 */
	bool addRoadTravelToBack(const patRoadBase* road, TransportMode t_m =
			TransportMode(NONE), double stop_time = 0.0);
	bool addRoadTravelToBack(const RoadTravel road);
	void removeRoadTravelToBack();
	bool addRoadTravelToFront(const patRoadBase* road, TransportMode t_m =
			TransportMode(NONE), double stop_time = 0.0);

	/**
	 *Generate Od of the path according to arc sequence
	 *@return Od of the path
	 */

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
	 * assign an id to path
	 *@param theId id to be assigned
	 */
	void assignId(const unsigned long theId);
	int nbrOfArcs() const;
	int nbrOfNodes() const;

	unsigned long getId() const;
	/**
	 * get od of the path
	 *@return pointer of the Od
	 */

	patOd* getOd() const;
	patString genDescription() const;
	bool append(const list<const patArc*>& newSeg);
	/**
	 * Append a new segment to the current path. The connection node should be consistent.
	 * @param newSeg, the pointer to the downstream path.
	 */
	bool append(const patMultiModalPath& newSeg);
	/**
	 * Join a new path bPath. The first arc of bPath and the last arc of the current path is the same.
	 * @param bPath: the path to be joined
	 */
	bool join(patMultiModalPath bPath);

	unsigned long getSubPath(patMultiModalPath* newPath, patNode* startNode,
			patNode* endNode);
	patMultiModalPath getSubPathWithNodesIndecis(int start, int end);
	patMultiModalPath getSeg(const patArc* aArc, const patArc* bArc) const;

	patMultiModalPath pathUntilStop(const patNetworkBase* network) const;

	//bool equalsSubPath(patMultiModalPath& b_path, int start, int end);
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

	vector<int> getChangePoints() const;
	short getNbrOfChangePoints();
	void setUnimodalTransportMode(TransportMode m);
	/**
	 * Get unimodal intermediate path segments' length
	 */
	vector<double> getIntermediateUnimodalLengths() const;

	/**
	 * Get modes of each unimodal segment;
	 */
	vector<TransportMode> getUnimodalModes() const;
	/**
	 * Read a path from shape files .shp and .dbf.
	 */
	bool readShpFile(string file_path, patNetworkElements* network,
			patError*& err);
	bool getArcsFromOSMIds(patNetworkElements* network, patError*& err);
	list<pair<const patArc*, TransportMode> > getArcsWithMode() const;
	/**
	 * Export the path to shape files.
	 * @param network The network.
	 * @param file_path The path of th export files
	 * @param err error pointer.
	 * @return true if successful, false otherwise.
	 */
	bool exportShpFiles(string file_path, patNetworkElements* network,
			patError*& err);
	list<RoadTravel> getRoadTravelList() const;

	list<TransportMode> getModeList() const;
	list<double> getStopTimeList() const;

	double getPerPrimaryLinkRd() const;
	double getPerTrunkLinkRd() const;
	double getPerMotorwayLinkRd() const;
	double getPerBridleRd() const;
	double getPerResidentialRd() const;
	double getPerUnclassifiedRd() const;
	double getPerTertiaryRd() const;
	double getPerSecondaryRd() const;
	double getPerPrimaryRd() const;
	double getPerTrunkRd() const;
	double getPerMotorwayRd() const;

	short getLeftTurns() const;
	short getRightTurns() const;

	short getStraightTurns() const;

	short getTrafficSignals() const;
	short getUTurns() const;
	RoadTravel back_road_travel() const;
	RoadTravel front_road_travel() const;
	const patRoadBase* backRoad() const;
	const patRoadBase* frontRoad() const;
	FolderPtr getKML(int iteration_number) const;

	double getMinimumTravelTime(
			const patNetworkEnvironment* network_environment,
			set<const patArc*>& begin_arcs, set<const patArc*>& end_arcs) const;
	/**
	 *
	 * Length until the last arc in ddr.
	 */
	double getLengthWithConstrainedByDDR(const patGpsDDR* ddr) const;
	void proposeStop(const patNetworkEnvironment* network_environment,
			double speed, double stop_time);

	double getTotalStopTime() const;

	bool isReasonableModeChange() const;
	TransportMode frontMode() const;
	TransportMode backMode() const;

	bool containsRoad(const patArc* arc, TransportMode mode) const;
	bool containsPT() const ;
	void detDistanceToStop(patNetworkEnvironment* network_environment);
	void setDistanceToStop(double d);
	const list<double>* getDistanceToStop() const;
	void clear();
protected:
	list<RoadTravel> m_roads;
	set<TransportMode> m_unique_modes;
	patOd* m_od;
	patString m_name;
	unsigned long m_id;
	struct path_attributes m_attributes;
	list<double> m_distance_to_stop; //For each arc, define a distance to downstream pt stop or trffic signal
};
#endif
