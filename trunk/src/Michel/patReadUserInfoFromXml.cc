//----------------------------------------------------------------
// File: patReadUserInfoFromXml.cc
// Author: Michel Bierlaire
// Creation: Sun Jun  7 10:26:11 2009
//----------------------------------------------------------------

#include <xmlwrapp/xmlwrapp.h>
#include "patReadUserInfoFromXml.h"
#include "patDisplay.h"
#include "patErrMiscError.h"
#include "patRandomWalk.h"
#include "patMonteCarlo.h"

patReadUserInfoFromXml::patReadUserInfoFromXml(patString fName) : 
  fileName(fName), 
  theNetwork(fName) {
  
}

bool patReadUserInfoFromXml::readFile(patError*& err) {

  DEBUG_MESSAGE("Read " << fileName) ;
  xml::init xmlinit ;  
  
  xml::tree_parser parser(fileName.c_str()); 
  if (!parser) {
    stringstream str ;
    str << "Error while parsing " << fileName ;
    err = new patErrMiscError(str.str()) ;
    WARNING(err->describe());
    return false ;
  }
  xml::document &doc = parser.get_document(); 

  xml::node theRoot = doc.get_root_node() ;


  xml::node::node_type theType = theRoot.get_type() ;

  xml::node::iterator rootIter(theRoot.begin()) ;
  xml::node::iterator rootEnd(theRoot.end()) ;

  map<patString,short> theNodeNames ;
  map<xml::node::node_type,short> theNodeTypes ;

  for (; rootIter != rootEnd ; ++rootIter) {
    patString nodeName(rootIter->get_name()) ;
    theNodeNames[nodeName]++ ;
    xml::node::node_type theType = rootIter->get_type() ;
    theNodeTypes[theType]++ ;
    if (nodeName == "network") {
      xml::attributes attr = rootIter->get_attributes() ;
      xml::attributes::iterator i; 
      unsigned long id ;
      patString fileName ;
      patString networkName ;
      for (i = attr.begin() ; i != attr.end() ; ++i) { 
	patString attrName(i->get_name()) ;
	if (attrName == "file") {
	  fileName = patString(i->get_value()) ;
	  DEBUG_MESSAGE("File name: " << fileName) ;
	}
	else if (attrName == "name") {
	  networkName = patString(i->get_value()) ; 
	  DEBUG_MESSAGE("Network name: " << networkName) ;
	}
      }
      patReadNetworkFromXml theNetworkParser(fileName,
					     networkName,
					     cleanTheNetwork) ;
      theNetworkParser.readFile(err) ;
      if (err != NULL) {
	WARNING(err->describe()) ;
	return false ;
      }
      theNetwork = theNetworkParser.getNetwork() ;

    }
    else if (nodeName == "doNotCleanTheNetwork") {
      xml::attributes attr = rootIter->get_attributes() ;
      xml::attributes::iterator i; 
      unsigned long id ;
      for (i = attr.begin() ; i != attr.end() ; ++i) { 
	patString attrName(i->get_name()) ;
	if (attrName == "value") {
	  patString theValue(i->get_value()) ;
	  if (theValue == "1" || theValue == "true" || theValue == "TRUE") {
	    cleanTheNetwork = false ;
	  }
	  else {
	    cleanTheNetwork = true ;
	  }
	  break ;
	} 
      }

    }
    else if (nodeName == "od") {
      unsigned long orig ;
      unsigned long dest ;
      xml::attributes attr = rootIter->get_attributes() ;
      xml::attributes::iterator i; 
      for (i = attr.begin() ; i != attr.end() ; ++i) { 
	patString attrName(i->get_name()) ;
	if (attrName == "orig") {
	  orig = atoi(i->get_value()) ;
	}
	if (attrName == "dest") {
	  dest = atoi(i->get_value()) ;
	}
      }
      theNetwork.addOd(orig,dest) ;
      xml::node::iterator odSpecIter ;

      for (odSpecIter = rootIter->begin() ; 
	   odSpecIter != rootIter->end() ; 
	   ++odSpecIter) {
	patString tagName(odSpecIter->get_name()) ;
	if (tagName == "displayShortestPath") {
	  theNetwork.addShortestPathToDisplay(orig,dest) ;
	}
      }

    }

    else if (nodeName == "way") {
      unsigned long id ;
      unsigned long upNodeId ;
      unsigned long downNodeId ;
      patString theName ;
      xml::attributes attr = rootIter->get_attributes() ;
      xml::attributes::iterator i; 
      for (i = attr.begin() ; i != attr.end() ; ++i) { 
	patString attrName(i->get_name()) ;
	if (attrName == "id") {
	  id = atoi(i->get_value()) ;
	  stringstream str ;
	  str << "Link " << id ;
	  theName = patString(str.str()) ;
	  break ;
	} 
      }
      xml::node::iterator linkIter ;

      for (linkIter = rootIter->begin() ; 
	   linkIter != rootIter->end() ; 
	   ++linkIter) {
	patString tagName(linkIter->get_name()) ;
	if (tagName == "bioattr") {
	  //	  DEBUG_MESSAGE("List of attributes");
	  pair<patString,double> oneAttribute ;
	  xml::attributes attr = linkIter->get_attributes() ;
	  xml::attributes::iterator i; 
	  for (i = attr.begin() ; i != attr.end() ; ++i) { 
	    if (patString(i->get_name()) == "name") {
	      oneAttribute.first = patString(i->get_value()) ;
	    } 
	    else if (patString(i->get_name()) == "value") {
	      oneAttribute.second = atof(i->get_value()) ;
	      theNetwork.addLinkAttributes(id,oneAttribute,err) ;
	      if (err != NULL) {
		WARNING(err->describe()) ;
		return false ;
	      }
	    }
	  }	  
	}
      }
    }
    else if (nodeName == "biosubpath") {
      xml::attributes attr = rootIter->get_attributes() ;
      xml::attributes::iterator i; 
      unsigned long id ;
      for (i = attr.begin() ; i != attr.end() ; ++i) { 
	patString attrName(i->get_name()) ;
	if (attrName == "id") {
	  id = atoi(i->get_value()) ;
	  break ;
	} 
      }
      xml::node::iterator spIter ;
      list<unsigned long> subpathIds ;
      
      for (spIter = rootIter->begin() ; 
	   spIter != rootIter->end() ; 
	   ++spIter) {
	patString tagName(spIter->get_name()) ;
	if (tagName == "ls") {
	  unsigned long spid = atoi(spIter->get_attributes().begin()->get_value()) ;
	  subpathIds.push_back(spid) ;
	}
      }
      theNetwork.addSubpath(id,subpathIds) ;
    }
    else if (nodeName == "biopath") {
      xml::attributes attr = rootIter->get_attributes() ;
      xml::attributes::iterator i; 
      unsigned long id ;
      for (i = attr.begin() ; i != attr.end() ; ++i) { 
	patString attrName(i->get_name()) ;
	if (attrName == "id") {
	  id = atoi(i->get_value()) ;
	  break ;
	} 
      }
      xml::node::iterator spIter ;
      list<unsigned long> subpathIds ;
      
      for (spIter = rootIter->begin() ; 
	   spIter != rootIter->end() ; 
	   ++spIter) {
	patString tagName(spIter->get_name()) ;
	if (tagName == "ls") {
	  unsigned long spid = atoi(spIter->get_attributes().begin()->get_value()) ;
	  subpathIds.push_back(spid) ;
	}
      }
      theNetwork.addPath(id,subpathIds) ;
    }
    else if (nodeName == "bioobs") {
      xml::node::iterator iter ;
      for (iter = rootIter->begin() ; 
	   iter != rootIter->end() ; 
	   ++iter) {
	patString tagName(iter->get_name()) ;
	if (tagName == "choice") {
	  
	}      
	else if (tagName == "biochar") {
	  
	}      
      }
    }
    else if (nodeName == "setOfPaths") {
      xml::node::iterator iter ;
      for (iter = rootIter->begin() ; 
	   iter != rootIter->end() ; 
	   ++iter) {
	patString tagName(iter->get_name()) ;
	if (tagName == "userDefined") {
	  patString name ;
	  xml::attributes attr = iter->get_attributes() ;
	  xml::attributes::iterator i; 
	  for (i = attr.begin() ; i != attr.end() ; ++i) { 
	    patString attrName(i->get_name()) ;
	    if (attrName == "name") {
	      name = patString(i->get_value()) ;
	    }
	  }   
	}
	else if (tagName == "randomWalk") {
	  double kumA, kumB ;
	  unsigned long nIter ;
	  patString name ;
	  xml::attributes attr = iter->get_attributes() ;
	  xml::attributes::iterator i; 
	  for (i = attr.begin() ; i != attr.end() ; ++i) { 
	    patString attrName(i->get_name()) ;
	    if (attrName == "kumA") {
	      kumA = atof(i->get_value()) ;
	    }
	    else if (attrName == "kumB") {
	      kumB = atof(i->get_value()) ;
	    }
	    else if (attrName == "nIter") {
	      nIter = atoi(i->get_value()) ;
	    }
	    else if (attrName == "name") {
	      name = patString(i->get_value()) ;
	    }
	  }
	  patRandomWalk* theAlgo = new patRandomWalk() ;
	  theAlgo->setKumaParameters(kumA,kumB) ;
	  theAlgo->setMaximumNumberOfTrials(nIter) ;
	  theNetwork.addPathGenerationAlgo(name,theAlgo,err) ;
	  if (err != NULL) {
	    WARNING(err->describe());
	    return false ;
	  }
	}
	else if (tagName == "monteCarlo") {
	  double stdErr ;
	  unsigned long nIter ;
	  patString name ;
	  xml::attributes attr = iter->get_attributes() ;
	  xml::attributes::iterator i; 
	  for (i = attr.begin() ; i != attr.end() ; ++i) { 
	    patString attrName(i->get_name()) ;
	    if (attrName == "stdErr") {
	      stdErr = atof(i->get_value()) ;
	    }
	    else if (attrName == "nIter") {
	      nIter = atoi(i->get_value()) ;
	    }
	    else if (attrName == "name") {
	      name = patString(i->get_value()) ;
	    }
	  }
	  patMonteCarlo* theAlgo = new patMonteCarlo() ;
	  theAlgo->setStandardError(stdErr) ;
	  theAlgo->setMaximumNumberOfTrials(nIter) ;
	  theNetwork.addPathGenerationAlgo(name,theAlgo,err) ;
	  if (err != NULL) {
	    WARNING(err->describe());
	    return false ;
	  }
	}
      }
    }
    else if (nodeName == "computePathAttributes") {
      xml::node::iterator compPathIter ;

      for (compPathIter = rootIter->begin() ; 
	   compPathIter != rootIter->end() ; 
	   ++compPathIter) {
	patString tagName(compPathIter->get_name()) ;
	if (tagName == "leftTurns") {
	  xml::attributes attr = compPathIter->get_attributes() ;
	  xml::attributes::iterator i; 
	  for (i = attr.begin() ; i != attr.end() ; ++i) { 
	    patString attrName(i->get_name()) ;
	    if (attrName == "minAngle") {
	      patAngle theAngle ;
	      theAngle.setAngleInDegree(atof(i->get_value())) ; 
	      theNetwork.mustComputeLeftTurns(true) ;
	      theNetwork.setThresholdForLeftTurns(theAngle) ;
	      break ;
	    } 
	  }
	}
      }
    }
  }

  if (cleanTheNetwork) {
    DEBUG_MESSAGE("Before cleaning: " << theNetwork.nbrOfArcs() << " arcs and " <<theNetwork.nbrOfNodes() << " nodes") ;
  }

  theNetwork.finalizeNetwork(cleanTheNetwork,err) ;

  if (err != NULL) {
    WARNING(err->describe());
    return false ;
  }

  if (cleanTheNetwork) {
    DEBUG_MESSAGE("After cleaning: " << theNetwork.nbrOfArcs() << " arcs and " <<theNetwork.nbrOfNodes() << " nodes") ;
  }
  return true ;
  

}
  

patNetwork patReadUserInfoFromXml::getNetwork() {
  return theNetwork ;

}
