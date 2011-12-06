//----------------------------------------------------------------
// File: patArc.h
// Author: Michel Bierlaire
// Creation: Thu Oct 30 09:40:25 2008
//----------------------------------------------------------------

#ifndef patArc_h
#define patArc_h

#include <iostream>
#include <map>
#include <list>
#include "patGeoCoordinates.h"
#include "patError.h"
#include "patType.h"
#include "patString.h"
#include "patNode.h"
#include "patRoadBase.h"
#include "kml/dom.h"
using kmldom::PlacemarkPtr;
//#include "patMultiModalPath.h"
struct arc_attributes {
	patString type;
	double heading;
	unsigned long priority;
};
class patMultiModalPath;
class patOdJ;
class patArc: public patRoadBase {
	friend class patPathSampling; //add by Jingmin

	friend class patNode;
	friend class patGpsPoint; //add by Jingmin

	friend class patPathDevelop; //add by Jingmin

	friend class patShortestPathAlgoRange; //add by Jingmin
	friend class patShortestPathTreeRange; //add by Jingmin
	friend class patOdJ; //add by Jingmin
	friend class patMultiModalPath; //add by Jingmin
	friend class patPathDDR; //add by Jingmin
	friend class patReadSample; //add by Jingmin
	friend class patSample; //add by Jingmin
	friend class patObservation; //add by Jingmin
	friend class patTraveler; //add by Jingmin
	friend class patTripParser; //add by Jingmin
	friend class patTripGraph; //add by Jingmin
	friend class patPathProbaAlgoV2; //add by Jingmin
	friend class patPathProbaAlgoV4; //add by Jingmin
	friend class patArcTransition; //added by Jingmin
	friend class patWriteKML; //added by Jingmin
	friend class patWriteBiogemeInput; //added by Jingmin
	friend class patSimulator; //added by Jingmin
	friend class patMapMatching; //added by Jingmin
	friend class patMapMatchingV2; //added by Jingmin
	friend class patMapMatchingRoute; //added by Jingmin
	friend class patStreetSegment; //added by Jingmin
	friend class patReadPathFromKML; //added by Jingmin
	friend class patNetworkElements; //add by Jingmin
	friend bool operator<(const patMultiModalPath& aPath, const patMultiModalPath& bPath); //add by Jingmin
	friend ostream& operator<<(ostream& str, patMapMatchingRoute& x);

	friend bool operator==(const patMultiModalPath& aPath, const patMultiModalPath& bPath); //add by Jingmin
	friend ostream& operator<<(ostream& str, const patMultiModalPath& x); ///add by Jingmin
	friend class patPath;
	friend class patLinkSubpath;
	friend ostream& operator<<(ostream& str, const patArc& x);

public:
	patArc(unsigned long theId, patNode* theUpNode, patNode* theDownNode);

	patArc(unsigned long theId, patNode* theUpNode, patNode* theDownNode,
			patString theName, patError*& err);
	patArc(unsigned long theId, patNode* theUpNode, patNode* theDownNode,
			patString theName, struct arc_attributes theAttr, patError*& err);

	/**
	 @return length of the arc in meters
	 */
	double getLength() const;

	 double computeLength() ;
	/**
	 @return patTRUE is the arc "follower" is actually consecutive to
	 the current arc
	 */
	bool followedBy(patArc* follower, patError*& err);

	double generalizedCost;
	void calPriority();
	const patNode* getUpNode() const;
	const patNode* getDownNode() const;
	double calHeading();
	 list<const patArc*> getArcList() const;
	unsigned long getUserId() const;
	double getHeading() const;
	patString getName() const;
	 int size() const;
	 bool isValid() const;
	 PlacemarkPtr getArcKML(string mode) const;

protected:
	double frozenGeneralizedCost;
	unsigned long m_user_id;
	unsigned long m_internal_id;
	unsigned long m_up_node_id;
	unsigned long m_down_node_id;
	patNode* m_up_node;
	patNode* m_down_node;
	patString m_name;
	unsigned long m_way_id;
	struct arc_attributes m_attributes;
	list<patGeoCoordinates> m_polyline;
};

#endif
