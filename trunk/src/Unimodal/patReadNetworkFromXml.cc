//----------------------------------------------------------------
// File: patReadNetworkFromXml.cc
// Author: Michel Bierlaire
// Creation: Thu Oct 30 14:02:15 2008
//----------------------------------------------------------------

#include <xmlwrapp/xmlwrapp.h>
#include "patReadNetworkFromXml.h"
#include "patDisplay.h"
#include "patErrMiscError.h"

#include "patNBParameters.h"
#include "patPower.h"
patReadNetworkFromXml::patReadNetworkFromXml(patString fName) : 
  fileName(fName), theNetwork(fName) {

}
patBoolean patReadNetworkFromXml::readFile(patError*& err) {
	patReal networkScaler = 1;
	if(patNBParameters::the()->doSimulation == 1){
	
		networkScaler = patNBParameters::the()->simNetworkScale;
  
	}
	xml::init xmlinit ;  
  
  xml::tree_parser parser(fileName.c_str()); 
  xml::document &doc = parser.get_document(); 
  if (!parser) {
    stringstream str ;
    str << "Error while parsing " << fileName ;
    err = new patErrMiscError(str.str()) ;
    WARNING(err->describe());
    return patFALSE ;
  }


  xml::node theRoot = doc.get_root_node() ;

  DEBUG_MESSAGE("Node: " << theRoot.get_name()) ;
  xml::node::node_type theType = theRoot.get_type() ;
  DEBUG_MESSAGE("Node type: " << theType) ;

  xml::node::iterator rootIter(theRoot.begin()) ;
  xml::node::iterator rootEnd(theRoot.end()) ;

  map<patString,short> theNodeNames ;
  map<xml::node::node_type,short> theNodeTypes ;

  for (; rootIter != rootEnd ; ++rootIter) {
    patString nodeName(rootIter->get_name()) ;
    theNodeNames[nodeName]++ ;
    xml::node::node_type theType = rootIter->get_type() ;
    theNodeTypes[theType]++ ;

    if (nodeName == "bounds") {
      patReal minlat, maxlat, minlon, maxlon ;
      
      xml::attributes attr = rootIter->get_attributes() ;
      xml::attributes::iterator i; 
      for (i = attr.begin() ; i != attr.end() ; ++i) { 
	patString attrName(i->get_name()) ;
	if (attrName == "minlat") {
	  minlat = atof(i->get_value()) ;
	  minlat *= networkScaler;
	} 
	else if (attrName == "maxlat") {
	  maxlat = atof(i->get_value()) ;
	  maxlat *= networkScaler;
	} 
	else if (attrName == "minlon") {
	  minlon = atof(i->get_value()) ;
	  maxlat *= networkScaler;
	} 
	else if (attrName == "maxlon") {
	  maxlon = atof(i->get_value()) ;
	  maxlon *= networkScaler;
	} 
	else {
	  stringstream str ;
	  str << "Unknown attribute of node '" 
	      << nodeName 
	      << "' in file " 
	      << fileName 
	      << ": '" 
	      << attrName 
	      << "'" ;
	  err = new patErrMiscError(str.str()) ;
	  WARNING(err->describe());
	  return patFALSE;
	}
      }
      theNetwork.setMapBounds(minlat, maxlat, minlon, maxlon) ;
    }
    else if (nodeName =="node") {
      patULong id ;
      patReal lat ;
      patReal lon ;
      patString name ;
      xml::attributes attr = rootIter->get_attributes() ;
      xml::attributes::iterator i; 
      for (i = attr.begin() ; i != attr.end() ; ++i) { 
	patString attrName(i->get_name()) ;
	if (attrName == "id") {
	  id = atoi(i->get_value()) ;
	  stringstream str ;
	  str << "Node " << id ;
	  name = patString(str.str()) ;
	} 
	else if (attrName == "lat") {
	  lat = atof(i->get_value()) ;
	  lat *= networkScaler;
	} 
	else if (attrName == "lon") {
	  lon = atof(i->get_value()) ;
	  lon *= networkScaler;
	} 
	else {
	  //	  WARNING("Ignore attribute: " << attrName) ;
	}
	
	
      }
      struct node_attributes theAttr;
      xml::node::iterator iter ;
      for (iter = rootIter->begin() ;
	   iter != rootIter->end() ;
	   ++iter) {
	if (patString(iter->get_name()) == "tag") {
	  xml::attributes attr = iter->get_attributes() ;
	  xml::attributes::iterator i; 
	  for (i = attr.begin() ; i != attr.end() ; ++i) { 
	    if (patString(i->get_value()) == "name") {
	      ++i ;
	      name = patString(i->get_value()) ;
	      //break ;
	    }
		else if(patString(i->get_value()) == "highway"){
			++i;
			theAttr.type = patString(i->get_value());
		}
		else if(patString(i->get_value()) == "railway"){
			++i;
			theAttr.type = "railway";
		}
		
	  }
	}	
      }
      
      patNode newNode(id,name,lat,lon,theAttr) ;
      patBoolean success = theNetwork.addNode(newNode) ;
      if (!success) {
	stringstream str ;
	str << "Node " << newNode << " not added to the network" ;
	err = new patErrMiscError(str.str()) ;
	WARNING(err->describe()) ;
	return patFALSE;
      }
    }
    else if (nodeName == "od") {
      patULong orig ;
      patULong dest ;
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
    }
    else if (nodeName == "relation") {
    }
    else if (nodeName == "text") {

    }
    else if (nodeName == "way") {
      patULong id ;
      patULong upNodeId ;
      patULong downNodeId ;
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
	  struct arc_attributes theAttr;
      vector<patULong> nodes ;
      patBoolean oneWay(patFALSE) ;
      patString streetName ;
      xml::node::iterator linkIter ;
      for (linkIter = rootIter->begin() ; 
	   linkIter != rootIter->end() ; 
	   ++linkIter) {
	patString tagName(linkIter->get_name()) ;
	if (tagName == "nd") {
	  patULong nodeid = atoi(linkIter->get_attributes().begin()->get_value()) ;
	  nodes.push_back(nodeid) ;
	} 
	else if (tagName == "tag") {
	  xml::attributes attr = linkIter->get_attributes() ;
	  xml::attributes::iterator i; 
	  for (i = attr.begin() ; i != attr.end() ; ++i) { 
	    if (patString(i->get_value()) == "name") {
	      ++i ;
	      theName = patString(i->get_value()) ;
	    }
	    else if (patString(i->get_value()) == "oneway") {
	      ++i ;
	      if ( (patString(i->get_value()) == "yes") ||  
		   (patString(i->get_value()) == "true") ) {
		oneWay = patTRUE ;
	      }
	    }
		else if (patString(i->get_value()) == "highway") {
	      ++i ;
	      theAttr.type = patString(i->get_value());
	    }
	  }	  
	}
      }
      if (!nodes.empty()) {
	vector<patULong>::iterator j(nodes.begin()), k(nodes.begin()) ;
	++k ;
	patULong theArcId(id * 100) ;
	for ( ;  k != nodes.end() ; ++j,++k) {
	  patBoolean success = theNetwork.addArcWithIds(theArcId,*j,*k,theName,theAttr,err) ;
	  if (err != NULL) {
	    WARNING(err->describe()) ;
	    return patFALSE ;
	  }
	  theNetwork.computeArcLength(theArcId,err) ;
	  if (err != NULL) {
	    WARNING(err->describe()) ;
	    return patFALSE ;
	  }
	  ++theArcId ;
	  if (!oneWay) {
	    success = theNetwork.addArcWithIds(theArcId,*k,*j,theName,theAttr,err) ;
	    if (err != NULL) {
	      WARNING(err->describe()) ;
	      return patFALSE;
	    }
	    theNetwork.computeArcLength(theArcId,err) ;
	    if (err != NULL) {
	      WARNING(err->describe()) ;
	      return patFALSE  ;
	    }
	    ++theArcId ;
	  }
	}
      }
    }
    else {

    }
  }

  DEBUG_MESSAGE("Node names") ;
  for(map<patString,short>::iterator i = theNodeNames.begin() ;
      i != theNodeNames.end() ;
      ++i) {
    DEBUG_MESSAGE(i->first << " [" << i->second << "]") ;
  }
  DEBUG_MESSAGE("Node types") ;
  for (map<xml::node::node_type,short>::iterator i = theNodeTypes.begin() ;
       i != theNodeTypes.end() ;
       ++i) {
    DEBUG_MESSAGE(i->first << " [" << i->second << "]") ;
  }


  theNetwork.finalizeNetwork(err) ;

  if (err != NULL) {
    WARNING(err->describe());
    return patFALSE ;
  }

  return patTRUE ;
  
}


patNetwork patReadNetworkFromXml::getNetwork() {
  return theNetwork ;
}
