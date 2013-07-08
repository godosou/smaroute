/*
 * patRandomDraw.h
 *
 *  Created on: Mar 22, 2012
 *      Author: jchen
 */

#ifndef PATRANDOMDRAWLOG_H_
#define PATRANDOMDRAWLOG_H_

class patRandomDrawLog {
public:
	patRandomDraw();

	set<int> drawByCount(unsigned long count);

	virtual ~patRandomDraw();
};

#endif /* PATRANDOMDRAWLOG_H_ */
