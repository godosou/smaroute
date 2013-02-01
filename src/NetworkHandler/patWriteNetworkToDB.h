/*
 * patWriteNetworkToDB.h
 *
 *  Created on: Aug 19, 2012
 *      Author: jchen
 */

#ifndef PATWRITENETWORKTODB_H_
#define PATWRITENETWORKTODB_H_
#include "patNetworkBase.h"
#include <string>

class patWriteNetworkToDB {
public:
	patWriteNetworkToDB();
	static void write(const patNetworkBase* network, const std::string& db_name) ;
	virtual ~patWriteNetworkToDB();
};

#endif /* PATWRITENETWORKTODB_H_ */
