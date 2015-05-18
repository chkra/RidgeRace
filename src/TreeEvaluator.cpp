/*
 * TreeEvaluator.cpp
 *
 *  Created on: Nov 14, 2012
 *      Author: ctusche
 */

#include "RR_TreeEvaluator.h"
#include <cmath>

void TreeEvaluator::getSum(FloatTreeAnnotation* index, float &total,
		size_t &count, bool includeLeafs) {

	map<RR_Node*, float> fullData = index->getFullData();
	map<RR_Node*, float>::iterator iter;

	for (iter = fullData.begin(); iter != fullData.end(); iter++) {

		if (iter->first->isLeaf() & !includeLeafs) {
			continue;
		}
		// else

		total += iter->second;
		count++;

	}
}

float TreeEvaluator::getMean(FloatTreeAnnotation* index, bool includeLeafs) {

	float total = 0.0f;
	size_t count = 0;

	getSum(index, total, count, includeLeafs);
	return (total / float(count));
}

void TreeEvaluator::correlate(FloatTreeAnnotation* i1, FloatTreeAnnotation* i2,
		CorrSums &cs, bool includeLeafs) {

	map<RR_Node*, float> fullData = i1->getFullData();
	map<RR_Node*, float>::iterator iter;

	RR_Node* curNode;
	float lX = 0;
	float lY = 0;
	for (iter = fullData.begin(); iter != fullData.end(); iter++) {

		curNode = iter->first;

		if (curNode->isLeaf() & !includeLeafs) {
			continue;
		}
		// else
		try {
			lX = i1->getElement(curNode) - cs.meanX;
			lY = i2->getElement(curNode) - cs.meanY;
		} catch (Exception e) {
			ostringstream os;
			os << "ERROR in TreeEvaluator::correlate: unable to access node!"
					<< '\n';
			os << " original error message was " << e.msg();
			os << " node id: " << curNode->id << '\n';
			os << " is leaf: " << curNode->isLeaf() << '\n';
			os << " in i1: " << i1->hasElement(curNode) << '\n';
			os << " in i2: " << i2->hasElement(curNode) << '\n';
			os << " i1: " << i1->getDescription() << '\n';
			os << " i2: " << i2->getDescription() << '\n';
			throw Exception(os.str());
		}
		cs.linXY += (lX * lY);

		cs.squX += (lX * lX);
		cs.squY += (lY * lY);
	}
}

float TreeEvaluator::correlate(FloatTreeAnnotation* first,
		FloatTreeAnnotation* second, bool includeLeafs) {

	CorrSums cs;
	cs.linXY = 0.0f;
	cs.squX = 0.0f;
	cs.squY = 0.0f;
	cs.meanX = getMean(first, includeLeafs);
	cs.meanY = getMean(second, includeLeafs);

	correlate(first, second, cs, includeLeafs);

	return cs.linXY / sqrt(cs.squX * cs.squY);
}

float TreeEvaluator::MSE(FloatTreeAnnotation* i1, FloatTreeAnnotation* i2,
		bool includeLeafs) {

	map<RR_Node*, float> fullData = i1->getFullData();
	map<RR_Node*, float>::iterator iter;

	RR_Node* curNode;
	float mse = 0;
	size_t count = 0;

	for (iter = fullData.begin(); iter != fullData.end(); iter++) {

		curNode = iter->first;

		if (curNode->isLeaf() & !includeLeafs) {
			continue;
		}
		// else
		float lX = 0;
		float lY = 0;
		try {
			lX = i1->getElement(curNode);
			lY = i2->getElement(curNode);
		} catch (Exception e) {
			ostringstream os;
			os << "ERROR in TreeEvaluator::MSE: unable to access node!"
					<< '\n';
			os << " original error message was " << e.msg();
			os << " node id: " << curNode->id << '\n';
			os << " is leaf: " << curNode->isLeaf() << '\n';
			os << " in i1: " << i1->hasElement(curNode) << '\n';
			os << " in i2: " << i2->hasElement(curNode) << '\n';
			os << " i1: " << i1->getDescription() << '\n';
			os << " i2: " << i2->getDescription() << '\n';
			throw Exception(os.str());
		}

		mse += pow(lX - lY, 2);
		count++;
	}
	return mse / float(count);
}

float TreeEvaluator::MPE(FloatTreeAnnotation* i1, FloatTreeAnnotation* i2) {

	map<RR_Node*, float> fullData = i1->getFullData();
	map<RR_Node*, float>::iterator iter;

	RR_Node* curNode;
	float mpe = 0;
	size_t count = 0;

	for (iter = fullData.begin(); iter != fullData.end(); iter++) {

		curNode = iter->first;

		// prediction error is only computed on leaf nodes,
		// so skip if curNode != a leaf
		if (!curNode->isLeaf()) {
			continue;
		}
		// else
		float lX = 0, lY = 0;
		try {
			lX = i1->getElement(curNode);
			lY = i2->getElement(curNode);
		} catch (Exception e) {
			ostringstream os;
			os << "ERROR in TreeEvaluator::MPE: unable to access node!"
					<< '\n';
			os << " original error message was " << e.msg();
			os << " node id: " << curNode->id << '\n';
			os << " is leaf: " << curNode->isLeaf() << '\n';
			os << " in i1: " << i1->hasElement(curNode) << '\n';
			os << " in i2: " << i2->hasElement(curNode) << '\n';
			os << " i1: " << i1->getDescription() << '\n';
			os << " i2: " << i2->getDescription() << '\n';
			throw Exception(os.str());
		}
		// don't do any square, just add up
		mpe += lX - lY;
		// count number so we can normalize variance
		// of the mean prediction error
		count++;
	}
	return mpe / float(count);
}

TreeEvaluator::TreeEvaluator(bool verbose) {

	this->verbose = verbose;

}
