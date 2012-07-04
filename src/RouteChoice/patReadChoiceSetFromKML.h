/*
 * patReadChoiceSetFromKML.h
 *
 *  Created on: May 3, 2012
 *      Author: jchen
 */

#ifndef PATREADCHOICESETFROMKML_H_
#define PATREADCHOICESETFROMKML_H_
#include "patObservation.h"
class patChoiceSet;
class patRandomNumber;
class patReadChoiceSetFromKML {
public:
	patReadChoiceSetFromKML(const patNetworkElements* a_network);
	map<patOd, patChoiceSet> read(string file_name,

	const patRandomNumber& rnd);
	virtual ~patReadChoiceSetFromKML();
private:
	const patNetworkElements* m_network;

};

#endif /* PATREADCHOICESETFROMKML_H_ */
