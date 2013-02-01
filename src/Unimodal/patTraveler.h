#ifndef patTraveler_h
#define patTraveler_h

#include "patType.h"

#include <map>
#include <set>
class patObservation;
class patTraveler{
 public:
	patTraveler(const patULong& theId);
	patTraveler(const patULong& theId,
				const patString& theName);
				
	friend patBoolean operator==(const patTraveler& aTraveler, const patTraveler& bTraveler) ;
friend patBoolean operator<(const patTraveler& aTraveler, const patTraveler& bTraveler) ;

	void setAttributes(const patString& key, const patReal& value);
	patReal getAttributes(const patString& key);
	patObservation* addObservation(patObservation& theObservation);
	patULong getNumberOfObservations();
	patULong getId();
	set<patObservation>* getAllObservations();
	protected:
	patULong id;
	patString name;
	map<patString, patReal> attributes;
	set<patObservation> observationSet;
};
#endif
