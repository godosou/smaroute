#ifndef patOdJ_h
#define patOdJ_h

#include "patType.h"
#include "patNode.h"
#include <iostream>
#include <set>

class patOd {

public:
	patOd();

	/**
	 * Constructor
	 * @param o origin node
	 * @param d destination node
	 */
	patOd(const patNode* o, const patNode* d);


	friend ostream& operator<<(ostream& str, const patOd& x);
	friend bool operator<(const patOd& od1, const patOd& od2);
	friend bool operator==(const patOd& od1, const patOd& od2);

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
