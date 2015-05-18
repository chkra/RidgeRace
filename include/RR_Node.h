/*
 * RRTree.h
 *
 *  Created on: Nov 8, 2012
 *      Author: ctusche
 */

#ifndef RRTREE_H_
#define RRTREE_H_

#include "Node.h"
#include <string>
#include <sstream>
#include "Exception.h"
#include "StringManipulation.h"

typedef float CharacterType;
using namespace std;

enum BaseStdMode {constStd, drawStd};

typedef string RidgeRaceIndex;



class Constants {
public:
    static const RidgeRaceIndex glsIndex;
    static const RidgeRaceIndex mlIndex;
    static const RidgeRaceIndex ridgeIndex;
    static const RidgeRaceIndex simulationIndex;
    static const RidgeRaceIndex regimeIndex;
    static const RidgeRaceIndex stdIndex;
    static const RidgeRaceIndex ridgeWeightIndex;
};









class RegimeSpecification {

public:
	size_t _nrOfRegimes;
	CharacterType _grandMean;
	float _baseStd;
	BaseStdMode _mode;

	RegimeSpecification() {
		_nrOfRegimes = 0;
		_grandMean = 0;
		_baseStd = 0;
		_mode = drawStd;
	}


	RegimeSpecification(size_t n, CharacterType g, float b, string mode) {
		_nrOfRegimes = n;
		_grandMean = g;
		_baseStd = b;
		if (equal(mode, "drawStd")) {
			_mode = drawStd;
		}
		else if (equal(mode, "constStd")) {
			_mode = constStd;
		}
		else {
			throw Exception("ERROR in RegimeSpecification::constructor: invalid BaseStdMode\n");
		}

		if (_mode==constStd && _nrOfRegimes != 1) {
			throw Exception("ERROR in RegimeSpecification::constructor: number of regimes must equal one when using constStd!\n");
		}
	}


	string toString() {
		std::ostringstream os;
		os << _nrOfRegimes << '\t';
		os << _grandMean << '\t';
		os << _baseStd;
		return os.str();
	}
};





/**
 * @ todo tidy up / reimplement class Node
 */
typedef Node RR_Node;

//template<class T>
//struct NodeAnnotation {
//	typedef std::map<RR_Node*, T> Type;
//};






#endif /* RRTREE_H_ */
