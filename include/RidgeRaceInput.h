/*
 * RidgeRaceInput.h
 *
 *  Created on: Mar 22, 2013
 *      Author: ctusche
 */

#ifndef RIDGERACEINPUT_H_
#define RIDGERACEINPUT_H_

//#include "RR_Node.h"
#include <string>
#include <stdlib.h>
#include "ConfigFile.h"

using namespace std;

struct RidgeRaceInput {

	/**
	 * if set to true, program will call RidgeRace::testFunctions() and
	 * stop afterwards
	 */
	int testMode;

	string inTree;
	string testCorrelationOutputFile;

	size_t maxNrOfRegimes;
	size_t nrOfRegimesStepSize;

	float grandMean;

	/* RidgeRacer will create a number of regimes on the tree.
	 * Each regime will be governed by a phenotypic rate (standard derivation).
	 * This std will be drawn from abs(Norm(0, baseStd)).
	 * The value of baseStd will be iterated in the program:
	 */
	float startStd;
	float endStd;
	float stepStd;

	size_t roundsOfEvolution;
	size_t roundsOfSimulation;

	bool drawTrees;

	unsigned int startTreeSize;
	unsigned int stopTreeSize;
	unsigned int stepTreeSize;
	unsigned int repeatTreeSim;

	string treeSimFile;
	string stdMode;
	string phenoPath;
	string sequencePath;
	string distanceMatrixPath;
	string posNamePath;

};

RidgeRaceInput readConfigFile(string path);


#endif /* RIDGERACEINPUT_H_ */
