/*
 * MHPath.h
 *
 *  Created on: Apr 20, 2012
 *      Author: jchen
 */

#ifndef MHPATH_H_
#define MHPATH_H_
#include <map>
#include "MHAlgorithm.h"
#include "MHPoints.h"
#include "patRouter.h"
#include "patMultimodalPath.h"
#include "patNode.h"
#include <map>
#include "patShortestPathTreeGeneral.h"
using namespace std;

class MHPath: public patMultiModalPath {
private:

	patShortestPathTreeGeneral  modifiedFwdCost();
	patShortestPathTreeGeneral  modifiedBwdCost();
	patMultiModalPath fwdRoute(const patNode* origin, const patNode* destination) ;
	patMultiModalPath bwdRoute(const patNode* origin, const patNode* destination) ;


	patMultiModalPath getForwardShortestPath(const patNode* A,
			const patNode* B) const;
	patMultiModalPath getBackwardShortestPath(const patNode* A,
			const patNode* B) const;
public:
	MHPath();
	MHPath(const patMultiModalPath& path, const MHPoints& points, const patRouter* router);
	virtual ~MHPath();
	const patNode* getNodeA() const;
	const patNode* getNodeB() const;
	const patNode* getNodeC() const;


	int getA() const;
	int getB() const;
	int getC() const;

	unsigned long pointCombinationSize() const;
	map<const patNode*, double> getInsertProbs(const double detour_cost_scale) ;
	void setPoints(MHPoints points);

	bool equalsSubPath(patMultiModalPath& b_path, int start, int end) const;
	bool isSpliceable();

	void setRouter(const patRouter* router);

	void insertDetour(const patNode* nodeB);

protected:
	const patRouter* m_router;
	double m_cost;
	MHPoints m_points;
	short m_spliceable;

	patShortestPathTreeGeneral m_modified_fwd_cost;
	patShortestPathTreeGeneral m_modified_bwd_cost;

}
;

#endif /* MHPATH_H_ */
