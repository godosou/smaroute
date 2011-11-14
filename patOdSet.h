#ifndef patOdSet_h
#define patOdSet_h

#include "patOdJ.h"
#include "patType.h"
#include "patPathJ.h"
class patOdSet{
	public:
		patOdSet();
set<patOd>* getOdSet();
patOd* addOd(patOd theOd);
patPath* findPath(const patPath& thePath);
patOd* findOd(const patOd& theOd);

	protected:
		set<patOd> odSet;
		
};

#endif
