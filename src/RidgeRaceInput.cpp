/*
 * RidgeRaceInput.cpp
 *
 *  Created on: Mar 25, 2013
 *      Author: ctusche
 */

#include "RidgeRaceInput.h"




RidgeRaceInput readConfigFile(string path) {

	ConfigFile cf(path);
	RidgeRaceInput rri;

	// run info
	rri.testMode = cf.Value("run", "TESTMODE");

	// input data
	rri.inTree = (string) cf.Value("input", "inTree");
	rri.startTreeSize = cf.Value("input", "startTreeSize");
	rri.stopTreeSize = cf.Value("input", "stopTreeSize");
	rri.stepTreeSize = cf.Value("input", "stepTreeSize");
	rri.repeatTreeSim = cf.Value("input", "repeatTreeSim");
	rri.treeSimFile = (string) cf.Value("input", "treeSimFile");

	// simulation parameters
	rri.grandMean = cf.Value("simulation", "grandMean");
	rri.maxNrOfRegimes = cf.Value("simulation", "maxNrOfRegimes");
	rri.nrOfRegimesStepSize = cf.Value("simulation", "nrOfRegimesStepSize");
	rri.stdMode = (string) cf.Value("simulation", "stdMode");
	rri.startStd = cf.Value("simulation", "startStd");
	rri.endStd = cf.Value("simulation", "endStd");
	rri.stepStd = cf.Value("simulation", "stepStd");

	// evaluation parameters
	rri.roundsOfSimulation = cf.Value("evaluation", "roundsOfSimulation");
	rri.roundsOfEvolution = cf.Value("evaluation", "roundsOfEvolution");
	rri.testCorrelationOutputFile = (string) cf.Value("evaluation", "testCorrelationOutputFile");
	rri.drawTrees = cf.Value("evaluation", "drawTrees");

	// inference
	rri.phenoPath = (string) cf.Value("inference", "phenoPath");
	rri.sequencePath = (string) cf.Value("inference", "sequencePath");
	rri.distanceMatrixPath = (string) cf.Value("inference", "distanceMatrixPath");
	rri.posNamePath = (string) cf.Value("inference", "posNamePath");

	return rri;
}
