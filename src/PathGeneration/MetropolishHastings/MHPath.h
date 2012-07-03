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
using namespace std::tr1;

class MHPath: public patMultiModalPath {
private:
	patMultiModalPath newSpliceSegmentAB(const patNode* insertNode, bool& correct) ;
	patMultiModalPath newSpliceSegmentBC(const patNode* insertNode, bool& correct) ;
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
	void setPoints(MHPoints points);
	bool equalsSubPath(patMultiModalPath& b_path, int start, int end) const;
	bool isSpliceable();
	void setRouter(const patRouter* router);
	bool insertDetour(const patNode* nodeB);

protected:
	const patRouter* m_router;
	double m_cost;
	MHPoints m_points;
	short m_spliceable;
}
;

#endif /* MHPATH_H_ */
