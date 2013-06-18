/*
 * patMapMatchingBed.h
 *
 *  Created on: Aug 18, 2012
 *      Author: jchen
 */

#ifndef PATMAPMATCHINGBED_H_
#define PATMAPMATCHINGBED_H_
#include<vector>
#include <string>
#include "patGeoBoundingBox.h"
#include "patNetworkEnvironment.h"
class patMapMatchingBed {
public:
	patMapMatchingBed(const std::vector<std::string>& gps_files);

	/**
	 * Initiate network environment
	 */
	void initiateNetworks();
	void getBoundingBox()  ;
	/**
	 * Generate shortest path for each gps file.
	 */
	void genShortestPath();
	virtual ~patMapMatchingBed();
private:
	std::vector<std::string> m_gps_files;
	patGeoBoundingBox m_bb;
	std::string m_output_folder;
	patNetworkEnvironment* m_network_environment;
};

#endif /* PATMAPMATCHINGBED_H_ */
