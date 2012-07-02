/*
 * PathGenerator.h
 *
 *  Created on: Feb 29, 2012
 *      Author: jchen
 */

#ifndef PATPATHGENERATOR_H_
#define PATPATHGENERATOR_H_
#include "patNetworkBase.h"
#include "patNode.h"
#include "patPathWriter.h"
class patPathGenerator {
public:
	//patPathGenerator();

	/**
	 * Sets the network. It is called after <code>configure</code> (inherited
	 * from <code>Configurable</code>) and before <code>setPathWriter</code>.
	 *
	 * @param network
	 *            the network
	 */
	virtual void setNetwork(const patNetworkBase* network) =0;
	virtual void setPathWriter(patPathWriter* path_writer) =0;

	virtual patPathGenerator* clone() const = 0;
	virtual double calculatePathLogWeight(const patMultiModalPath& path) const=0;
	/**
	 * Sets the path writer. It is called after <code>setNetwork</code> and
	 * (once) before all calls to <code>run</code>.
	 * <p>
	 * The implementing class is expected to call
	 * {@link bioroute.PathWriter#writePath(java.util.List, java.util.Map)} once
	 * for every generated path. All other functions of <code>PathWriter</code>
	 * are called by <code>BiorouteRunner</code>.
	 *
	 * @param writer
	 *            the path writer
	 */

	/**
	 * Generates paths for the indicated OD pair. Is called once for every
	 * origin/destination pair in the XML configuration file.
	 *
	 * @param origin
	 *            the origin
	 * @param destination
	 *            the destination
	 */
	virtual void run(const patNode* origin, const patNode* destination)=0;
	//virtual ~patPathGenerator();
};

#endif /* PATHGENERATOR_H_ */
