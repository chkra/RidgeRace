/*
 * RidgeRaceInput.cpp
 *
 *  Created on: Mar 25, 2013
 *      Author: ctusche
 */

#include "RidgeRaceInput.h"
#include <sys/stat.h>
#include <iostream>
#include <cstdio>
#include <string>
#include <stdlib.h>

#include <errno.h>    // errno, ENOENT, EEXIST
#if defined(_WIN32)
#include <direct.h>   // _mkdir
#endif

bool isDirExist(const std::string& path)
{
#if defined(_WIN32)
    struct _stat info;
    if (_stat(path.c_str(), &info) != 0)
    {
        return false;
    }
    return (info.st_mode & _S_IFDIR) != 0;
#else
    struct stat info;
    if (stat(path.c_str(), &info) != 0)
    {
        return false;
    }
    return (info.st_mode & S_IFDIR) != 0;
#endif
}

bool makePath(const std::string& path)
{
#if defined(_WIN32)
    int ret = _mkdir(path.c_str());
#else
    mode_t mode = 0755;
    int ret = mkdir(path.c_str(), mode);
#endif
    if (ret == 0)
        return true;
    if (ret == -1) {
    	std::cerr << "cannot create directory '" << path.c_str() << "'\n";
    	exit(1);
    }
    switch (errno)
    {
    case ENOENT:
        // parent didn't exist, try to create it
        {
            unsigned int pos = path.find_last_of('/');
            if (pos == std::string::npos)
#if defined(_WIN32)
                pos = path.find_last_of('\\');
            if (pos == std::string::npos)
#endif
                return false;
            if (!makePath( path.substr(0, pos) ))
                return false;
        }
        // now, try to create again
#if defined(_WIN32)
        return 0 == _mkdir(path.c_str());
#else
        return 0 == mkdir(path.c_str(), mode);
#endif

    case EEXIST:
        // done!
        return isDirExist(path);

    default:
        return false;
    }
}

void checkDirExistence(const string& str) {
  size_t found;
  found=str.find_last_of("/\\");
  string folder = str.substr(0,found).c_str();
  {
  	    struct stat st;
  	    if(stat(folder.c_str(),&st) != 0) {
  	    	makePath(folder.c_str());

  	    	char actualpath [10000+1];
  	    	char *ptr;
  	    	ptr = realpath(folder.c_str(), actualpath);

  	    	std::cerr << "directory '" << ptr << "' has been created in order to write log file\n";
  	    }
  	}
}

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
	checkDirExistence(rri.testCorrelationOutputFile);

	rri.drawTrees = cf.Value("evaluation", "drawTrees");

	// inference
	rri.phenoPath = (string) cf.Value("inference", "phenoPath");
	rri.sequencePath = (string) cf.Value("inference", "sequencePath");
	rri.distanceMatrixPath = (string) cf.Value("inference", "distanceMatrixPath");
	rri.posNamePath = (string) cf.Value("inference", "posNamePath");

	return rri;
}
