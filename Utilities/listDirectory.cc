/*
 * listDirectory.cc
 *
 *  Created on: Nov 14, 2011
 *      Author: jchen
 */

#include "listDirectory.h"
#include<iostream>
#include<sstream>
#include<fstream>
#include "patDisplay.h"
listDirectory::listDirectory() {
	// TODO Auto-generated constructor stub

}

listDirectory::~listDirectory() {
	// TODO Auto-generated destructor stub
}

list<string> listDirectory::getListOfFiles(string directory_name,
		patError* err) {
	DIR * dip;
	struct dirent *dit;

	DEBUG_MESSAGE("Try to open directory " << directory_name);
	if ((dip = opendir(directory_name)) == NULL) {
		stringstream str;
		str << "Directory " << directory_name
				<< " doesn't exist or no permission to read.";
		err = new patErrMiscError(str.str());
		WARNING(err->describe());
		return list<string>();
	}
	DEBUG_MESSAGE("Direcotry " << directory_name << " is now open");
	list < string > dirContent;
	unsigned char isFile = 0x8;
	string esp("~");
	string don("#");
	string csvext("csv");

	while ((dit = readdir(dip)) != NULL) {
		if (dit->d_type == isFile) {
			string fileName(dit->d_name);
			if (fileName.find(csvext) != string::npos
					&& fileName.find(esp) == string::npos
					&& fileName.find(don) == string::npos) {
				DEBUG_MESSAGE("Found file:" << fileName);
				dirContent.push_back(fileName);
			}

		}
	}

	closedir(dip);
	return dirContent;
}
