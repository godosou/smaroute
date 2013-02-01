//-*-c++-*------------------------------------------------------------
//
// File name : patConst.h
// Author :    \URL[Michel Bierlaire]{http://rosowww.epfl.ch/mbi}
// Date :      Fri Nov 27 15:30:42 1998
//
//--------------------------------------------------------------------

#ifndef patConst_h
#define patConst_h

#ifndef NULL
#define NULL 0L
#endif

#include <errno.h>
#ifdef SGI_COMPILER
#include <float.h>
#define patSTRINGERASE erase
#else
#include <cfloat>
#define patSTRINGERASE erase
#endif

#include "patType.h"

/**
 */
const patBoolean patFALSE = (0 != 0) ;
/**
 */
const patBoolean patTRUE  = (0 == 0) ;

/**
 */
const unsigned long patBadId = unsigned(-1) ;

/**
 */
#define PURE_VIRTUAL 0
/**
 */
#define DELETE_PTR(ptr) { if (ptr != NULL) { delete (ptr) ; ptr = NULL ; } }

#include <math.h>
/**
 */
#define patNAN 999999

// Does not link with infnan.
//#define patNAN infnan(0)

/**
 */
const patReal pi = acos(-1.0) ;

#ifndef name2 
/**
 */
# define name2(X,Y) X##Y
#endif

/**
@doc  Provides the logarithm of the largest real value. It is the largest argument that can be provided to an exp statement.   Implemented as a singleton to avoid recomputation of the log each time it
     is invoked
@author \URL[Michel Bierlaire]{http://rosowww.epfl.ch/mbi}
*/
   
  class patLogMaxReal {
  public:
    /**
     */
    static patReal the() {
      static patLogMaxReal* me = NULL ;
      if (me == NULL) {
        me = new patLogMaxReal() ;
      }
      return (me->val) ;
    } ;
  private :
    patLogMaxReal() : val(log(patMaxReal)) {
    } ;
  private:
    patReal val ;
  };

#endif /* patConst_h */


