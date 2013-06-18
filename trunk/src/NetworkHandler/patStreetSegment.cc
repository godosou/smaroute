/*
 * File:   patStreetSegment.cc
 * Author: jchen
 *
 * Created on April 7, 2011, 11:44 AM
 */
#include "patStreetSegment.h"
#include "patErrNullPointer.h"
#include "patDisplay.h"
#include "patErrMiscError.h"

/**
 * Initiate with an empty street
 * @param aNetwork
 */
patStreetSegment::patStreetSegment(patNetwork* aNetwork) :
Network(aNetwork) {
}

/**
 * Initiate with a list of arcs.
 * @param aNetwork
 * @param anArcList
 */
patStreetSegment::patStreetSegment(patNetwork* aNetwork, list<patArc*> anArcList) :
Network(aNetwork),
ArcList(anArcList) {

}

patStreetSegment::patStreetSegment(patNetwork* aNetwork, patArc* anArc, patError*& err) :
Network(aNetwork) {
    generateStreetFromArc(anArc, err);
}

bool operator==(const patStreetSegment& street1, const patStreetSegment& street2) {
    const list<patArc*>* arcList1 = street1.getArcList();
    const list<patArc*>* arcList2 = street2.getArcList();


    if (arcList1->size() != arcList2->size()) {
        return false;
    }

    unsigned long segs = arcList1->size();
    list<patArc*>::const_iterator iter1 = arcList1->begin();
    list<patArc*>::const_iterator iter2 = arcList2->begin();

    while (segs != 0) {
        if (*iter1 != *iter2) {
            return false;
        }
        ++iter1;
        ++iter2;
        --segs;
    }
    return true;


}

patNode* patStreetSegment::getStartNode(patError*& err) {
    patNode* StartNode = Network->getNodeFromUserId(ArcList.front()->m_up_node_id);
    if (StartNode == NULL) {

        stringstream str;
        str << "Start Node " << ArcList.front()->m_up_node_id << " does not exist";
        err = new patErrNullPointer(str.str());
        WARNING(err->describe());
        return NULL;

    }
    return StartNode;
}

patNode* patStreetSegment::getEndNode(patError*& err) {
    patNode* EndNode = Network->getNodeFromUserId(ArcList.back()->m_down_node_id);
    if (EndNode == NULL) {
        stringstream str;
        str << "End Node " << ArcList.back()->m_down_node_id << " does not exist";
        err = new patErrNullPointer(str.str());
        WARNING(err->describe());
        return NULL;

    }
    return EndNode;
}

patArc* patStreetSegment::getStartArc() {
    return ArcList.front();
}

patArc* patStreetSegment::getEndArc() {
    return ArcList.back();
}

/**
 * Append an arc to the back of the street segment.
 * 1. Check the consistency.
 * 2. Append to the arc list.
 * @param anArc
 * @return
 */
bool patStreetSegment::addArcToBack(patArc* anArc) {
    if (anArc->m_up_node_id == ArcList.back()->m_down_node_id) {
        ArcList.push_back(anArc);
        return true;
    } else {
        return false;
    }
}

/**
 * Add an arc to the front of the street segment.
 * 1. Check the consistency.
 * 2. Add to the arc list.
 * @param anArc
 * @return
 */
bool patStreetSegment::addArcToFront(patArc* anArc) {
    if (anArc->m_down_node_id == ArcList.front()->m_up_node_id) {
        ArcList.push_front(anArc);
        return true;
    } else {
        return false;
    }
}

/**
 * Delete the first arc.
 *
 * @return patTRUE if the segment is not empty
 */
bool patStreetSegment::deleteArcFromFront() {
    if (ArcList.size() == 0) {
        return false;
    } else {
        ArcList.pop_front();
        return true;
    }
}

/**
 * Delete the last arc.
 * @return patTRUE if the segment is not empty
 */
bool patStreetSegment::deleteArcFromBack() {
    if (ArcList.size() == 0) {
        return false;
    } else {
        ArcList.pop_back();
        return true;
    }
}

bool patStreetSegment::checkDownwardSingleWay(patNode* aNode, patNode* prevNode) {
    set<unsigned long> NodeSet;
    for (set<unsigned long>::iterator iter = aNode->userSuccessors.begin();
            iter != aNode->userSuccessors.end();
            ++iter) {
        if ((*iter) != prevNode->userId) {
            NodeSet.insert(*iter);
        }
    }
    if (NodeSet.size() == 1) {
        return true;
    } else {
        return false;
    }
}

bool patStreetSegment::checkUpwardSingleWay(patNode* aNode, patNode* nextNode) {
    set<unsigned long> NodeSet;
    for (set<unsigned long>::iterator iter = aNode->userPredecessors.begin();
            iter != aNode->userPredecessors.end();
            ++iter) {
        if ((*iter) != nextNode->userId) {
            NodeSet.insert(*iter);
        }
    }
    if (NodeSet.size() == 1) {
        return true;
    } else {
        return false;
    }
}

/**
 * Generate a segment from a single arc.
 * The segment exsits and is unique because it is links between two intersections.
 * 1. It searches downwards from the down node until an intersection.
 * 2. It searches upwards from the up node until an intersection.
 * 
 * @param anArc
 */
void patStreetSegment::generateStreetFromArc(patArc* anArc, patError*& err) {
    patNode* UpNode = Network->getNodeFromUserId(anArc->m_up_node_id);
    patNode* DownNode = Network->getNodeFromUserId(anArc->m_down_node_id);
    ArcList.push_back(anArc);
    if (UpNode == NULL) {
        stringstream str;
        str << "Up Node " << anArc->m_up_node_id << " does not exist";
        err = new patErrNullPointer(str.str());
        WARNING(err->describe());
        return;

    }
    if (DownNode == NULL) {
        stringstream str;
        str << "Down Node " << anArc->m_down_node_id << " does not exist";
        err = new patErrNullPointer(str.str());
        WARNING(err->describe());
        return;
    }

    //Downward search
    patNode* DownwardNode = DownNode;
    patNode* PrevNode = UpNode;
    while (checkDownwardSingleWay(DownwardNode, PrevNode)) {

        const unsigned long NextNodeId = *(DownwardNode->userSuccessors.begin());
        if (PrevNode->userId == NextNodeId) {
            break;
        }
        patArc* DownwardArc = Network->getArcFromNodesUserId(DownwardNode->userId, NextNodeId);

        if (DownwardArc == NULL) {
            stringstream str;
            str << "Arc " << DownwardNode->userId << "," << NextNodeId << " does not exist";
            err = new patErrNullPointer(str.str());
            WARNING(err->describe());
            return;
        } else {
            if (DownwardArc->m_name == "M1" ||
                    DownwardArc->m_attributes.priority < 4 ||
                    DownwardArc->m_attributes.priority > 14) {
                break;
            }
            if (addArcToBack(DownwardArc) == false) {
                stringstream str;
                str << "Arc append is not consistent";
                err = new patErrMiscError(str.str());
                WARNING(err->describe());
                return;
            }
        }
        PrevNode = DownwardNode;
        DownwardNode = Network->getNodeFromUserId(NextNodeId);

        if (DownwardNode == NULL) {
            stringstream str;
            str << "Up Node " << NextNodeId << " does not exist";
            err = new patErrNullPointer(str.str());
            //WARNING(err->describe());
            return;

        }
    }

    //Upwardsearch
    patNode* UpwardNode = UpNode;
    PrevNode = DownNode;
    while (checkUpwardSingleWay(UpwardNode, PrevNode)) {
        const unsigned long NextNodeId = *(UpwardNode->userPredecessors.begin());
        if (PrevNode->userId == NextNodeId) {
            break;
        }
        patArc* UpwardArc = Network->getArcFromNodesUserId(NextNodeId, UpwardNode->userId);

        if (UpwardArc == NULL) {
            stringstream str;
            str << "Arc " << UpwardNode->userId << "," << NextNodeId << " does not exist";
            err = new patErrNullPointer(str.str());
            WARNING(err->describe());
            return;
        } else {
            if (UpwardArc->m_name == "M1" ||
                    UpwardArc->m_attributes.priority < 4 ||
                    UpwardArc->m_attributes.priority > 14) {
                break;
            }
            if (addArcToFront(UpwardArc) == false) {
                stringstream str;
                str << "Arc append is not consistent";
                err = new patErrMiscError(str.str());
                //WARNING(err->describe());
                return;
            }
        }
        PrevNode = UpwardNode;
        UpwardNode = Network->getNodeFromUserId(NextNodeId);

        if (UpwardNode == NULL) {
            stringstream str;
            str << "Up Node " << NextNodeId << " does not exist";
            err = new patErrNullPointer(str.str());
            WARNING(err->describe());
            return;

        }
    }


}

const list<patArc*>* patStreetSegment::getArcList() const {
    return &ArcList;
}

unsigned long patStreetSegment::size() {
    return ArcList.size();
}

double patStreetSegment::getLength() {
    double Length = 0;
    for (list<patArc*>::iterator aIter = ArcList.begin();
            aIter != ArcList.end();
            ++aIter) {
        Length += (*aIter)->getLength();
    }
    return Length;

}


