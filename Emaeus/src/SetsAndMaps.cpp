/*
 * SetsAndMaps.cpp
 *
 *  Created on: Sep 29, 2011
 *      Author: ctusche
 */


#include "SetsAndMaps.h"



#include <cfloat>
BasicStats getBasicStats(std::vector<float> v) {

	BasicStats ms;

	float totalSum = 0.0f;
	float min= FLT_MAX;
	float max= -FLT_MAX;

	for (size_t i=0; i<v.size(); i++) {
		totalSum += v[i];
		if (v[i]>max) max = v[i];
		if (v[i]<min) min = v[i];
	}

	ms.mean = totalSum / float(v.size());

	totalSum = 0.0f;
	for (size_t i=0; i<v.size(); i++) {
		totalSum += (v[i]-ms.mean)*(v[i]-ms.mean);
	}
	ms.std = sqrt(totalSum/float(v.size()));
	ms.max = max;
	ms.min = min;

	return ms;
}

BasicStats getAbsMeanAndStd(std::vector<float> v) {

	BasicStats ms;
	float min= FLT_MAX;
	float max= -FLT_MAX;

	float totalSum = 0.0f;
	for (size_t i=0; i<v.size(); i++) {
		totalSum += abs(v[i]);
		if (v[i]>max) max = v[i];
		if (v[i]<min) min = v[i];
	}

	ms.mean = totalSum / float(v.size());

	totalSum = 0.0f;
	for (size_t i=0; i<v.size(); i++) {
		totalSum += (abs(v[i])-ms.mean)*(abs(v[i])-ms.mean);
	}
	ms.std = sqrt(totalSum/float(v.size()));
	ms.max = max;
	ms.min = min;
	return ms;
}
