/*
 * listDirectory.cc
 *
 *  Created on: Nov 14, 2011
 *      Author: jchen
 */

#include "patListDirectory.h"
#include<iostream>
#include<sstream>
#include<fstream>
#include "patDisplay.h"
#include <dirent.h>
#include "patErrMiscError.h"
patListDirectory::patListDirectory() {
	//

}

patListDirectory::~patListDirectory() {
	//
}

list<string> patListDirectory::getListOfFiles(string directory_name,
		patError* err) {
	DIR * dip;
	struct dirent *dit;
	const char* dirName = directory_name.c_str();

	DEBUG_MESSAGE("Try to open directory " << dirName);
	if ((dip = opendir(dirName)) == NULL) {
		stringstream str;
		str << "Directory " << dirName
				<< " doesn't exist or no permission to read.";
		err = new patErrMiscError(str.str());
		WARNING(err->describe());
		return list<string>();
	}
	DEBUG_MESSAGE("Direcotry " << dirName << " is now open");
	list < string > dirContent;
	unsigned char isFile = 0x8;
	string esp("~");
	string don("#");
	string csvext("csv");
	string accel("accel");
	string bt("bt");

	while ((dit = readdir(dip)) != NULL) {
		if (dit->d_type == isFile) {
			string fileName(dit->d_name);
			if (fileName.find(csvext) != string::npos
					&& fileName.find(esp) == string::npos
					&& fileName.find(don) == string::npos
					&& fileName.find(accel) == string::npos
					&& fileName.find(bt) == string::npos
					&& fileName.find(".kml") == string::npos) {
				DEBUG_MESSAGE("Found file:" << fileName);
				dirContent.push_back(fileName);
			}

		}
	}

	closedir(dip);
	return dirContent;
}
