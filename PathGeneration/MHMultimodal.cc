/*
 * MHMultimodal.cc
 *
 *  Created on: Oct 28, 2011
 *      Author: jchen
 */

#include "MHMultimodal.h"

MHMultimodal::MHMultimodal(patNode* the_origin,
		patNode* the_destination,
		patNetworkEnvironment* the_environment):
origin(the_origin),
destination(the_destination),
environment(the_environment)
{

}

MHMultimodal::~MHMultimodal() {
}

