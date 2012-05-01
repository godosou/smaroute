#ifndef patTraveler_h
#define patTraveler_h

#include "patType.h"

#include <map>
#include <set>
class patObservation;
class patTraveler{
 public:
	patTraveler(const unsigned long& theId);
	patTraveler(const unsigned long& theId,
				const patString& theName);
				
	friend bool operator==(const patTraveler& aTraveler, const patTraveler& bTraveler) ;
friend bool operator<(const patTraveler& aTraveler, const patTraveler& bTraveler) ;

	void setAttributes(const patString& key, const double& value);
	double getAttributes(const patString& key);
	patObservation* addObservation(patObservation& theObservation);
	unsigned long getNumberOfObservations();
	unsigned long getId();
	set<patObservation>* getAllObservations();
	protected:
	unsigned long id;
	patString name;
	map<patString, double> attributes;
	set<patObservation> observationSet;
};
#endif
