#ifndef patTravelerSet_h
#define patTravelerSet_h

class patTravelerSet{
 public:
	patTraveler* addTraveler(const patTraveler& theTraveler);
	patTraveler* addTraveler(const patULong& theTravelerId);
	patTraveler* findTraveler(const patULong& theTravelerId);
	set<patTraveler>* getAllTravelers();
	patULong getNumberofObservations();
 protected:
	set<patTraveler> travelerSet;
	
};

#endif
