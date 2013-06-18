#ifndef patOdJ_h
#define patOdJ_h

#include "patType.h"
#include "patNode.h"
#include <iostream>
#include <set>

class patOdJ {

public:
	patOdJ();

	/**
	 * Constructor
	 * @param o origin node
	 * @param d destination node
	 */
	patOdJ(const patNode* o, const patNode* d);


	friend ostream& operator<<(ostream& str, const patOdJ& x);
	friend bool operator<(const patOdJ& od1, const patOdJ& od2);
	friend bool operator==(const patOdJ& od1, const patOdJ& od2);

	/**
	 * Get origin node patNode*.
	 */
	const patNode* getOrigin();

	/**
	 * Get destination node patNode*.
	 */
	const patNode* getDestination();
protected:
	const patNode* orig;
	const patNode* dest;
};
#endif
