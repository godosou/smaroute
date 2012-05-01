/*
 * Exception
 *
 *  Created on: Apr 20, 2012
 *      Author: jchen
 */

#ifndef PATEXCEPTION_H_
#define PATEXCEPTION_H_

#include <iostream>
#include "patDisplay.h"
#include <exception>
using  namespace std;
class IllegalArgumentException: public exception{
public:
	IllegalArgumentException(const char*  a):m_a(a){};
	virtual const char* what( ) const throw(){
		return m_a;
	}
protected:
	const char*  m_a;
};

class RuntimeException: public exception{
public:
	RuntimeException(const char*  a):m_a(a){};
	virtual const char* what( ) const throw(){
		return m_a;
	}
protected:
	const char*  m_a;
};

#endif /* EXCEPTION_ */
