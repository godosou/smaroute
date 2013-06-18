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
//#include "patPathJ.h"
class patNetwork;
struct arc_attributes {
	patString type;
	patReal heading;
	unsigned long priority;
};
class patPathJ;
class patOdJ;
class patNetwork;
class patArc : public patRoadBase{
	friend class patPathSampling; //add by Jingmin

	friend class patNetwork;
	friend class patNode;
	friend class patGpsPoint; //add by Jingmin

	friend class patPathDevelop; //add by Jingmin

	friend class patShortestPathAlgoRange; //add by Jingmin
	friend class patShortestPathTreeRange; //add by Jingmin
	friend class patOdJ; //add by Jingmin
	friend class patPathJ; //add by Jingmin
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
	friend bool operator<(const patPathJ& aPath, const patPathJ& bPath); //add by Jingmin
	friend ostream& operator<<(ostream& str, patMapMatchingRoute& x);

	friend bool operator==(const patPathJ& aPath, const patPathJ& bPath); //add by Jingmin
	friend ostream& operator<<(ostream& str, const patPathJ& x); ///add by Jingmin
	friend class patPath;
	friend class patLinkSubpath;
	friend ostream& operator<<(ostream& str, const patArc& x);

public:
	patArc(unsigned long theId, patNode* theUpNode, patNode* theDownNode);

	patArc(unsigned long theId, patNode* theUpNode, patNode* theDownNode,
			patString theName, patError*& err);
	patArc(unsigned long theId, patNode* theUpNode, patNode* theDownNode,
			patString theName, struct arc_attributes theAttr, patError*& err);

	unsigned long computeTurn(patArc* downArc, patNetwork* theNetwork);
	/**
	 @return length of the arc in meters
	 */
	patReal getLength() const;

	/**
	 @return patTRUE is the arc "follower" is actually consecutive to
	 the current arc
	 */
	patBoolean followedBy(patArc* follower, patError*& err);

	patReal generalizedCost;
	void calPriority();
	const patNode* getUpNode() ;
	const patNode* getDownNode() ;
	patReal calHeading(patNetwork* theNetwork, patError*& err);
	patReal calHeading();
	 list<patArc*> getArcList() const;

protected:
	patReal frozenGeneralizedCost;
	unsigned long userId;
	unsigned long internalId;
	unsigned long upNodeId;
	unsigned long downNodeId;
	patNode* upNode;
	patNode* downNode;
	patString name;
	unsigned long way_id;
	struct arc_attributes attributes;
	list<patGeoCoordinates> polyline;
};

#endif
