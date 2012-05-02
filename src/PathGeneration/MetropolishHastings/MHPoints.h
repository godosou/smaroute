/*
 * MHPoints.h
 *
 *  Created on: Apr 20, 2012
 *      Author: jchen
 */

#ifndef MHPOINTS_H_
#define MHPOINTS_H_

#include <iostream>
using namespace std;
class MHPoints{
public:
	MHPoints();
	MHPoints(int A, int B, int C);
	int getA() const;
	int getB() const;
	int getC() const;

	friend ostream& operator<<(ostream& str, const MHPoints& x);
private:
	int m_A;
	int m_B;
	int m_C;

};
#endif /* MHPOINTS_H_ */
