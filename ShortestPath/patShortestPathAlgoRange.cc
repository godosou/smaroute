

#include "patDisplay.h"
#include "patShortestPathAlgoRange.h"
#include "patGpsPoint.h"
#include "patShortestPathTreeRange.h"
#include "patNetwork.h"
#include "patArc.h"
#include <vector>
#include <list>
#include <set>


patShortestPathAlgoRange::patShortestPathAlgoRange(patGpsPoint* gpsPoint, vector< list < pair<patArc*, patULong> > >* adjList, patNetwork* theNetwork, bool aMinimumLabel
        ) : theTree(adjList->size()) {

    minimumLabel = aMinimumLabel;
    baseNetwork = theNetwork;
    adjacencyLists = adjList;
    theGpsPoint = gpsPoint;

}

patBoolean patShortestPathAlgoRange::computeShortestPathTree(set<patULong> theRoot,
        patReal ceil, set<patArc*>* ddrArcs) {

    for (set<patULong>::iterator iter = theRoot.begin();
            iter != theRoot.end();
            ++iter) {

        listOfNodes.push_back(*iter);
        theTree.labels[*iter] = 0.0;
        theTree.root.insert(*iter);
    }

    while (!listOfNodes.empty()) {

        patULong nodeToProcess = *listOfNodes.begin();
        //DEBUG_MESSAGE("deal with"<<nodeToProcess<<":"<<(*adjacencyLists)[nodeToProcess].size());
        listOfNodes.pop_front();
        if (theTree.labels[nodeToProcess] > ceil) {
            //DEBUG_MESSAGE("exceed length limit");
            break;
        }
        for (list < pair<patArc*, patULong> >::iterator downNode =
                (*adjacencyLists)[nodeToProcess].begin();
                downNode != (*adjacencyLists)[nodeToProcess].end();
                ++downNode) {

            patULong downNodeId = downNode->second;
            patArc* incomingArc = downNode->first;

            patReal linkCost = incomingArc->length;
            patReal downNodeLabel = theTree.labels[downNodeId];
            //DEBUG_MESSAGE("node"<<downNodeId<<":"<<theTree.labels[nodeToProcess]<<","<<linkCost<<";"<<downNodeLabel);
            //DEBUG_MESSAGE("incomingArc"<<*incomingArc);

            if (theGpsPoint != NULL) {
                if (true) {
                    if (theGpsPoint->linkDDR.find(incomingArc) == theGpsPoint->linkDDR.end()) {
                        if (theGpsPoint->detLinkDDR(incomingArc, baseNetwork) == patTRUE) {
                            ddrArcs->insert(incomingArc);
                        }
                    } else {
                        ddrArcs->insert(incomingArc);
                    }
                }
            }
            if (downNodeLabel > theTree.labels[nodeToProcess] + linkCost) {
                theTree.labels[downNodeId] = theTree.labels[nodeToProcess] + linkCost;
                if (theTree.labels[downNodeId] < minimumLabel) {
                    WARNING("NEGATIVE CYCLE DETECTED");
                    return patFALSE;
                }
                theTree.predecessor[downNodeId] = pair<patULong, patArc*>(nodeToProcess, incomingArc);

                theTree.successor[nodeToProcess].push_back(pair<patULong, patArc*>(downNodeId, incomingArc));

                // Add the node following Bertsekas (1993)
                if (listOfNodes.empty()) {
                    //DEBUG_MESSAGE("add node to list"<<downNodeId);
                    listOfNodes.push_back(downNodeId);
                } else {
                    patReal topLabel = theTree.labels[*(listOfNodes.begin())];
                    if (downNodeLabel <= topLabel) {
                        listOfNodes.push_front(downNodeId);
                    } else {
                        listOfNodes.push_back(downNodeId);
                    }
                }
            }
        }
    }
    //DEBUG_MESSAGE(theTree.predecessor[3893]);
    return patTRUE;
}

patShortestPathTreeRange patShortestPathAlgoRange::getTree() {
    return theTree;
}
