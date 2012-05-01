/*
 * patArcSequence.h
 *
 *  Created on: Nov 15, 2011
 *      Author: jchen
 */

#ifndef PATARCSEQUENCE_H_
#define PATARCSEQUENCE_H_
#include "patArc.h"
#include "patNode.h"
#include "patRoadBase.h"
#include <list>
#include "kml/dom.h"
using kmldom::FolderPtr;
class patArcSequence: public patRoadBase {
	friend ostream& operator<<(ostream& str, const patArcSequence& x);
public:
	patArcSequence();

	bool isError();

	/**
	 * return the pointer to the up node
	 */
	const patNode* getUpNode() const;

	/**
	 * return the pointer to the down node
	 */
	const patNode* getDownNode() const;

	/**
	 * Get the length of the segment.
	 */
	double getLength() const;
	/**
	 * Calculate the length of the segment.
	 */
	double computeLength();
	const patNode* containLoop() const;
	double getNodeLoopCount() const;
	bool empty() const;
	/**
	 * Get the list of arcs;
	 */
	virtual vector<const patArc*> getArcList() const;
	bool isValid() const;
	/**
	 *Add an arc to the front of listOfArcs
	 *@param theArc arc to be added
	 */
	virtual bool addArcToFront(const patArc* arc);

	/**
	 *Add an arc to the back of listOfArcs
	 *@param theArc arc to be added
	 */
	bool addArcToBack(const patArc* arc);
	/**
	 *Check if an arc is on the path
	 *@param theArc pointer of arc to be checked
	 *@return TRUE if theArc in path, FALSE no
	 */
	bool addArcsToBack(const vector<const patArc*>* arc_list);

	bool addArcsToBack(const patRoadBase* road);
	bool addArcsToFront(const patRoadBase* road);
	bool isLinkInPath(const patArc* theArc) const;
	bool isNodeInPath(const patNode* node) const;

	set<const patArc*> getDistinctArcs();
	int size() const;

	const patArc* getArc(unsigned long iArc) const;
	virtual const patArc* deleteArcFromBack();
	 const patArc* back() const;
	 const patArc* front() const;
	 const patArc* back2() const;

	 virtual FolderPtr getKML() const;

		const patNode* getNode(int index) const ;

protected:
	vector<const patArc*> m_arcs;
	double m_length;
};

#endif /* PATARCSEQUENCE_H_ */
