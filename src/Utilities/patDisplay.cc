//-*-c++-*------------------------------------------------------------
//
// File name : patDisplay.cc
// Author :    \URL[Michel Bierlaire]{http://rosowww.epfl.ch/mbi}
// Date :      Mon Jun  2 22:59:15 2003
//
//--------------------------------------------------------------------

#include <cassert>
#include "patDisplay.h"
#include "patFileNames.h"
#include "patAbsTime.h"
#include "patVersion.h"
 #include <iostream>
using namespace std;
patDisplay::patDisplay() : screenImportance(patImportance::patDEBUG),
			   logImportance(patImportance::patDEBUG),
			   logFile(patFileNames::the()->getLogFile().c_str()) {

  patAbsTime now ;
  now.setTimeOfDay() ;
  logFile << "This file has automatically been generated." << endl ;
  logFile << now.getTimeString(patTsfFULL) << endl ;
  logFile << patVersion::the()->getCopyright() << endl ;
  logFile << endl ;
  logFile << patVersion::the()->getVersionInfoDate() << endl ;
  logFile << patVersion::the()->getVersionInfoAuthor() << endl ;
  logFile << endl ;
  logFile << flush ;

}

patDisplay::~patDisplay() {
   logFile.close() ;
}

patDisplay* patDisplay::the() {
  static patDisplay* singleInstance = NULL ;
  if (singleInstance == NULL) {
    singleInstance = new patDisplay ;
    assert(singleInstance != NULL) ;
  }
  return singleInstance ;
}

void patDisplay::addMessage(const patImportance& aType,
			    const patString& text,
			    const patString& fileName,
			    const patString& lineNumber) {
  patMessage theMessage ;
  theMessage.theImportance = aType ;
  theMessage.text = text ;
  theMessage.fileName = fileName ;
  theMessage.lineNumber = lineNumber ;

  patAbsTime now ;
  now.setTimeOfDay() ;
  theMessage.theTime = now ;
  messages.push_back(theMessage) ;
  if (aType <= screenImportance) {
    if (screenImportance < patImportance::patDEBUG) {
      cout << theMessage.shortVersion() << endl << flush  ;
    }
    else {
      cout << theMessage.fullVersion() << endl << flush  ;
    }
  }
  if (aType <= logImportance) {
    if (logImportance < patImportance::patDEBUG) {
      logFile << theMessage.shortVersion() << endl << flush  ;
    }
    else {
      logFile << theMessage.fullVersion() << endl << flush  ;
    }
  }
}

void patDisplay::setScreenImportanceLevel(const patImportance& aType) {
  screenImportance = aType ;
}

void patDisplay::setLogImportanceLevel(const patImportance& aType) {
  logImportance = aType ;
}

void patDisplay::initProgressReport(const patString message,
			unsigned long upperBound) {


}

patBoolean patDisplay::updateProgressReport(unsigned long currentValue) {
  return true ;
}

void patDisplay::terminateProgressReport() {

}
