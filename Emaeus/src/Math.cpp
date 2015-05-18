/*
 * Math.cpp
 *
 *  Created on: Sep 29, 2011
 *      Author: ctusche
 */

#include "Math.h"


float getMean(std::vector<float> v) {
	float f = 0;
	for (unsigned int i = 0; i < v.size(); i++) {
		f += v[i];
	}
	return f / float(v.size());
}

bool isNan(float f) {
	return f!=f;
}
