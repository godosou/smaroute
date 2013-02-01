/*
 * patComputeBoundingBox.cc
 *
 *  Created on: Aug 21, 2012
 *      Author: jchen
 */

#include "patComputeBoundingBox.h"
using namespace std;
#include "patGpsSequence.h"
patComputeBoundingBox::patComputeBoundingBox() {
	// TODO Auto-generated constructor stub

}

patComputeBoundingBox::~patComputeBoundingBox() {
	// TODO Auto-generated destructor stub
}

patGeoBoundingBox patComputeBoundingBox::computeByGPSFiles(const std::vector<std::string>& files){
	patGeoBoundingBox bb;
	for (unsigned i = 0; i < files.size(); ++i) {
		string file_path = files.at(i);
		try {
			patGpsSequence gps_seqeunce(file_path);
			bb.extend(gps_seqeunce.computeBoundingBox(0.05));

		} catch (RuntimeException& e) {
			cout << e.what() << endl;
		}
	}
	cout << bb;

	return bb;
}
