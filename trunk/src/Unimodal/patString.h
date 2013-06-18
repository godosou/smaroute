//-*-c++-*------------------------------------------------------------
//
// File name : patString.h
// Author :    \URL[Michel Bierlaire]{http://rosowww.epfl.ch/mbi}
// Date :      Fri Jun 18 13:40:01 1999
//
//--------------------------------------------------------------------

#ifndef patString_h
#define patString_h

using namespace std ;

#include <string>

typedef string patString ;

// #include "patMyString.h"
// typedef patMyString patString ;

patString noSpace(const patString& aString) ;

/**
   Replace all occurences of "chain" in "theString" with "with"
 */
patString* replaceAll(patString* theString, patString chain, patString with) ;

/**
   Ceate a new string of size n, where blanks are added before or after depending on the requested justification (1=left, 0=right).
If n is too snall, the string is returned as such without warning.
 */

patString fillWithBlanks(const patString& theStr, 
			 unsigned long n, 
			 short justifyLeft) ;

#endif //patString_h



