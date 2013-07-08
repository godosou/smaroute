#ifndef patOd_h
#define patOd_h

#include "patType.h"
#include "patNode.h"
#include <iostream>
#include <set>


#include "kml/dom.h"
using kmldom::KmlFactory;
using kmldom::FolderPtr;
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

	void setOrigin(const patNode* o);
	void setDestination(const patNode* d);
	FolderPtr getKML() const;
protected:
	const patNode* orig;
	const patNode* dest;
};
#endif
