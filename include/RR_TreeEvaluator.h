/*
 * TreeEvaluator.h
 *
 *  Created on: Nov 14, 2012
 *      Author: ctusche
 */

#ifndef TREEEVALUATOR_H_
#define TREEEVALUATOR_H_

#include <stdlib.h>
//#include "RR_Node.h"
#include "RR_TreeAnnotation.h"

struct CorrSums {

	float linXY;
	float linY;
	float squX;
	float squY;

	float meanX;
	float meanY;

};

class TreeEvaluator {

private:

	bool verbose;

	void correlate(FloatTreeAnnotation* i1, FloatTreeAnnotation* i2, CorrSums &cs,
			bool includeLeafs);

public:

	TreeEvaluator(bool verbose=false);

	float correlate(FloatTreeAnnotation* firstIndex, FloatTreeAnnotation* secondIndex,
			bool includeLeafs);

	float MSE(FloatTreeAnnotation* i1, FloatTreeAnnotation* i2, bool includeLeafs);

	float MPE(FloatTreeAnnotation* i1, FloatTreeAnnotation* i2);

	float getMean(FloatTreeAnnotation* index, bool includeLeafs);

	void getSum(FloatTreeAnnotation* index, float &total, size_t &count,
			bool includeLeafs);

};

#endif /* TREEEVALUATOR_H_ */
