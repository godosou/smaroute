#ifndef patOdJ_h
#define patOdJ_h

#include "patType.h"
#include "patNode.h"
#include <iostream>
#include <set>

class patPathJ;
class patOdJ {

public:
	patOdJ();

	/**
	 * Constructor
	 * @param o origin node
	 * @param d destination node
	 */
	patOdJ(patNode* o, patNode* d);

	/**
	 * Find a path in the od.
	 * @param aPath, the path to be found.
	 * @return a pointer to the path; NULL if not found
	 */
	patPathJ* findPath(patPathJ aPath);

	/**
	 * Add a path to the OD.
	 */
	patPathJ* addPath(patPathJ& thePath);

	/**
	 * Get all paths.
	 * @return the pointer to the set of paths set<patPathJ>*.
	 */
	set<patPathJ>* getAllPaths();

	/**
	 * Get the number of paths.
	 */
	patULong getPathNumber();


	friend ostream& operator<<(ostream& str, const patOdJ& x);
	friend patBoolean operator<(const patOdJ& od1, const patOdJ& od2);
	friend patBoolean operator==(const patOdJ& od1, const patOdJ& od2);

	/**
	 * Get origin node patNode*.
	 */
	patNode* getOrigin();

	/**
	 * Get destination node patNode*.
	 */
	patNode* getDestination();
protected:
	patNode* orig;
	patNode* dest;
	set<patPathJ> pathSet;
};
#endif
