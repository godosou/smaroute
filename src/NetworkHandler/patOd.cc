#include <iostream>
#include "patDisplay.h"
#include "patOd.h"
#include "patMultiModalPath.h"
#include <sstream>
patOd::patOd() {

}
patOd::patOd(const patNode* o, const patNode* d) :
		orig(o), dest(d) {

}

ostream& operator<<(ostream& str, const patOd& x) {
	str << "[" << *(x.orig) << "-" << *(x.dest) << "]";
	return str;
}

/*
 operator for comparing paths
 */
bool operator<(const patOd& od1, const patOd& od2) {
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
bool operator==(const patOd& od1, const patOd& od2) {
	if (od1.orig == od2.orig && od2.dest == od2.dest) {
		return true;
	}
	return false;
}

/*
 find a path
 */

const patNode* patOd::getOrigin() {
	return orig;
}
const patNode* patOd::getDestination() {
	return dest;
}

FolderPtr patOd::getKML() const {
	// Create <coordinates>.

	KmlFactory* factory = KmlFactory::GetFactory();

	FolderPtr kml_folder = factory->CreateFolder();

	stringstream desc;
	desc<<orig->getUserId();
	desc<<"-";
	desc<<dest->getUserId();
	kml_folder->set_name(desc.str());
	return kml_folder;
}

void patOd::setOrigin(const patNode* o){
	orig=o;
}
void patOd::setDestination(const patNode* d){
	dest = d;
}
