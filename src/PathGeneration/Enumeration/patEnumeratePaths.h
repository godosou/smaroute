/*
 * patEnumeratePaths.h
 *
 *  Created on: May 25, 2012
 *      Author: jchen
 */

#ifndef PATENUMERATEPATHS_H_
#define PATENUMERATEPATHS_H_
#include "patPathGenerator.h"
#include "patNetworkBase.h"
#include "patNode.h"
#include "patPathWriter.h"
#include "patMultiModalPath.h"
class patEnumeratePaths : public patPathGenerator {
public:
	patEnumeratePaths();
	void setNetwork(const patNetworkBase* network_base);
	void run(const patNode* origin, const patNode* destination);
	void setPathWriter(patPathWriter* path_writer);
	void depth_first_enumerate(
			const unordered_map<const patNode*, set<const patRoadBase*> >* outgoing_incidents
			,const patNode* up_node, const patNode* end_node
			, patMultiModalPath& tmp_path,
			set<patMultiModalPath>& found_path) const;
	patEnumeratePaths* clone() const{
		return new patEnumeratePaths(*this);
	}
	double calculatePathLogWeight(const patMultiModalPath& ) const{
		return 0.0;
	}
	virtual ~patEnumeratePaths();
private:
	const patNetworkBase* m_network;
	patPathWriter* m_path_writer;
};

#endif /* PATENUMERATEPATHS_H_ */
