/*
 * MHPath.h
 *
 *  Created on: Apr 20, 2012
 *      Author: jchen
 */

#ifndef MHPATH_H_
#define MHPATH_H_
#include "MHAlgorithm.h"
#include "MHPoints.h"
#include "patRouter.h"
#include "patMultiModalPath.h"
#include "patNode.h"
#include <deque>
using namespace std;
using namespace std::tr1;

class MHPath: public patMultiModalPath {
private:
	patMultiModalPath newSpliceSegmentAB(const patNode* insertNode,
			bool& correct);
	patMultiModalPath newSpliceSegmentBC(const patNode* insertNode,
			bool& correct);
public:
	MHPath();
	MHPath(const patMultiModalPath& path, const MHPoints& points,
			const patRouter* router);
	virtual ~MHPath();
	const patNode* getNodeA() const;
	const patNode* getNodeB() const;
	const patNode* getNodeC() const;

	bool checkSpliceableAB(const patNode* insertNode) const;
	bool checkSpliceableBC(const patNode* insertNode) const;
	deque<const patNode*> getOrderedNodes(int start, int end,
			Direction direct) const;
	int getA() const;
	int getB() const;
	int getC() const;

	unsigned long pointCombinationSize() const;
	void setPoints(MHPoints points);
	bool equalsSubPath(patMultiModalPath& b_path, int start, int end) const;
	bool isSpliceable();
	void setRouter(const patRouter* router);
	bool insertDetour(const patNode* nodeB,
			const unordered_map<const patNode*, double>& proposalProbabilities);

	void update(
			const unordered_map<const patNode*, double>& proposalProbabilities);
	MHPoints drawPoints(const patRandomNumber* rnd);

protected:
	const patRouter* m_router;
	vector<unsigned> m_valide_nodes;
	vector<const patRoadBase*> m_roads;
	double m_cost;
	MHPoints m_points;
	short m_spliceable;
}
;

#endif /* MHPATH_H_ */
