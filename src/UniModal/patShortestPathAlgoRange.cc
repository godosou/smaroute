

#include "patDisplay.h"
#include "patShortestPathAlgoRange.h"
#include "patGpsPoint.h"
#include "patShortestPathTreeRange.h"
#include "patNetwork.h"
#include "patArc.h"
#include <vector>
#include <list>
#include <set>


patShortestPathAlgoRange::patShortestPathAlgoRange(patGpsPoint* gpsPoint, vector< list < pair<patArc*, unsigned long> > >* adjList, patNetwork* theNetwork, bool aMinimumLabel
        ) : theTree(adjList->size()) {

    minimumLabel = aMinimumLabel;
    baseNetwork = theNetwork;
    adjacencyLists = adjList;
    theGpsPoint = gpsPoint;

}

bool patShortestPathAlgoRange::computeShortestPathTree(set<unsigned long> theRoot,
        double ceil, set<patArc*>* ddrArcs) {

    for (set<unsigned long>::iterator iter = theRoot.begin();
            iter != theRoot.end();
            ++iter) {

        listOfNodes.push_back(*iter);
        theTree.labels[*iter] = 0.0;
        theTree.root.insert(*iter);
    }

    while (!listOfNodes.empty()) {

        unsigned long nodeToProcess = *listOfNodes.begin();
        //DEBUG_MESSAGE("deal with"<<nodeToProcess<<":"<<(*adjacencyLists)[nodeToProcess].size());
        listOfNodes.pop_front();
        if (theTree.labels[nodeToProcess] > ceil) {
            //DEBUG_MESSAGE("exceed length limit");
            break;
        }
        for (list < pair<patArc*, unsigned long> >::iterator downNode =
                (*adjacencyLists)[nodeToProcess].begin();
                downNode != (*adjacencyLists)[nodeToProcess].end();
                ++downNode) {

            unsigned long downNodeId = downNode->second;
            patArc* incomingArc = downNode->first;

            double linkCost = incomingArc->length;
            double downNodeLabel = theTree.labels[downNodeId];
            //DEBUG_MESSAGE("node"<<downNodeId<<":"<<theTree.labels[nodeToProcess]<<","<<linkCost<<";"<<downNodeLabel);
            //DEBUG_MESSAGE("incomingArc"<<*incomingArc);

            if (theGpsPoint != NULL) {
                if (true) {
                    if (theGpsPoint->linkDDR.find(incomingArc) == theGpsPoint->linkDDR.end()) {
                        if (theGpsPoint->detLinkDDR(incomingArc, baseNetwork) == true) {
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
                    return false;
                }
                theTree.predecessor[downNodeId] = pair<unsigned long, patArc*>(nodeToProcess, incomingArc);

                theTree.successor[nodeToProcess].push_back(pair<unsigned long, patArc*>(downNodeId, incomingArc));

                // Add the node following Bertsekas (1993)
                if (listOfNodes.empty()) {
                    //DEBUG_MESSAGE("add node to list"<<downNodeId);
                    listOfNodes.push_back(downNodeId);
                } else {
                    double topLabel = theTree.labels[*(listOfNodes.begin())];
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
    return true;
}

patShortestPathTreeRange patShortestPathAlgoRange::getTree() {
    return theTree;
}
