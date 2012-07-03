//----------------------------------------------------------------
// File: patNode.h
// Author: Michel Bierlaire
// Creation: Thu Oct 30 09:33:09 2008
//----------------------------------------------------------------

#ifndef patNode_h
#define patNode_h

#include <set>
#include "patCoordinates.h"
#include "patType.h"
#include "patString.h"
//#include "patArc.h"
#include <tr1/unordered_map>
using namespace std::tr1;
#include "kml/dom.h"

using kmldom::PlacemarkPtr;
using namespace std;
struct node_attributes {
	patString type;
	bool traffic_signal;
};
class patArc;
#include "patIterator.h"

class patOd;
class patNode {

	friend class patDisconnectedNode;
	friend class patNetwork;
	friend class patArc;
	friend class patPathSampling; //add by Jingmin

	friend class patGpsPoint; //add by Jingmin
	friend class patShortestPathTreeRange; //add by Jingmin
	friend class patPathDDR; //add by Jingmin
	friend class patReadSampleFromCsv; //add by Jingmin
	friend class patTripParser; //add by Jingmin
	friend class patOd; //add by Jingmin
	friend class patMultiModalPath; //add by Jingmin
	friend class patReadSample; //add by Jingmin
	friend class patSample; //add by Jingmin
	friend class patObservation; //add by Jingmin
	friend class patTraveler; //add by Jingmin
	friend class patTripGraph; //add by Jingmin
	friend class patPathProbaAlgoV2; //add by Jingmin
	friend class patArcTransition; //added by Jingmin
	friend class patWriteBiogemeInput; //added by Jingmin
	friend class patSimulator; //added by Jingmin
	friend class patMapMatching; //added by Jingmin
	friend class patMapMatchingV2; //added by Jingmin
	friend class patMapMatchingRoute; //added by Jingmin
	friend class patStreetSegment; //added by Jingmin
	friend class patNetworkElements; //added by Jingmin

	friend ostream& operator<<(ostream& str, const patNode& x);
	friend bool operator<(const patOd& od1, const patOd& od2);

public:
	/**
	 @param theId User ID of the node
	 @param theName Name of the node
	 @param lat Latitude of the node
	 @param lon Longitude of the node
	 */

	patNode(unsigned long theId, patString theName, double lat, double lon,
			struct node_attributes theAttr);
	patNode(unsigned long theId, double lat, double lon);

	patString getName() const;
	const patArc* getOutgoingArc(unsigned long down_node_id) const;

	patIterator<unsigned long>* getSuccessors();

	unsigned long getUserId() const;
	/**
	 @param aSucc user ID of the successor to add
	 */
	void addSuccessor(unsigned long aSucc);
	/**
	 @param aPred user ID of the predecessor to add
	 */
	void addPredecessor(unsigned long aPred);

	/**
	 */
	bool disconnected() const;

	patCoordinates getGeoCoord() const;
	double getLatitude() const;
	double getLongitude() const;
	void setName(string the_name) ;
	void setTags(unordered_map<string, string>& tags);
	/**
	 */
	bool isCentroid;
	PlacemarkPtr getKML() const;
	string getTag(string tag_key) const;
	string getTagString() const;
 void setTag(string key, string value);
 unordered_map<string, string > getTags() const;
	double calHeading(const patNode* b_node) const;

	bool hasTrafficSignal() const;
protected:
	unordered_map<string, string > m_tags;
	unsigned long userId;
	unsigned long internalId;
	patString name;
	patCoordinates geoCoord;
	set<unsigned long> userPredecessors;
	set<unsigned long> userSuccessors;
	map<unsigned long, patArc*> outgoingArcs;
	map<unsigned long, patArc*> incomingArcs;
	struct node_attributes attributes;
};

#endif
