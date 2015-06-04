#ifndef PREFIX_HH
#define PREFIX_HH

#include <iostream>
#include <cstdio>

namespace RidgeRace {
	extern const char progname[]; //name of the binary / program
	extern const char prefix[]; //the directory in which the program will be installed. Important to know, because e.g. R script are positioned relative to this file in ../share/RidgeRace/
	extern const char Rbin[]; //path to the R binary
	extern const char* tmpdir; //a directory which can be read and written, used to place temporary results
	extern std::stringstream treeDir;
	extern int uid;
}

#endif
