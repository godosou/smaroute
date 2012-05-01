#include <iostream>
#include "patDisplay.h"
#include "patOdJ.h"
#include "patMultiModalPath.h"
patOdJ::patOdJ() {

}
patOdJ::patOdJ(const patNode* o, const patNode* d) :
		orig(o), dest(d) {

}

ostream& operator<<(ostream& str, const patOdJ& x) {
	str << "[" << *(x.orig) << "-" << *(x.dest) << "]";
	return str;
}

/*
 operator for comparing paths
 */
bool operator<(const patOdJ& od1, const patOdJ& od2) {
	if (od1.orig->userId < od2.orig->userId) {
		return true;
	}
	if (od1.orig->userId > od2.orig->userId) {
		return false;
	}
	return (od1.dest->userId < od2.dest->userId);
}

/*
 operation "=" for comparing paths
 */
bool operator==(const patOdJ& od1, const patOdJ& od2) {
	if (od1.orig == od2.orig && od2.dest == od2.dest) {
		return true;
	}
	return false;
}

/*
 find a path 
 */

const patNode* patOdJ::getOrigin() {
	return orig;
}
const patNode* patOdJ::getDestination() {
	return dest;
}
