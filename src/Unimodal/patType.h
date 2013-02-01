//-*-c++-*------------------------------------------------------------
//
// File name : patType.h
// Author :    \URL[Michel Bierlaire]{http://rosowww.epfl.ch/mbi}
// Date :      Fri Dec 18 23:29:08 1998
//
//--------------------------------------------------------------------

#ifndef patType_h
#define patType_h

#ifdef ADOLC
#include <adolc/adouble.h>
#endif

#ifdef SGI_COMPILER
#include <float.h>
#else
#include <cfloat>
#endif

#include <vector>
#include <math.h>
#include "patString.h"


/**
 */
typedef short patBoolean;

/**
 */
typedef double patReal ;

/**
 */
typedef float patPythonReal ;

/**
 */
const patReal patEPSILON = DBL_EPSILON ;

/**
 */
const  patReal patSQRT_EPSILON = 1.49012e-08 ;

/**
 */
const patReal patMaxReal = DBL_MAX ;
/**
 */
const patReal patMinReal = DBL_MIN ;

/**
 */
typedef unsigned long patULong  ;

/**
 */
typedef unsigned long patUnitTime ;

/**
 */
typedef vector<patString> patStringVector ;
#endif
