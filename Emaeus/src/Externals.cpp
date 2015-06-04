/*
 * Externals.cpp
 *
 *  Created on: Nov 14, 2012
 *      Author: ctusche
 */

#include <stdlib.h>
#include <fstream>
#include <sstream>
#include "Externals.h"
#include "../../include/prefix.h"

int Caller::callR(string scriptPath, vector<string> params) {

	std::ostringstream os;

	os << RidgeRace::Rbin << " --vanilla --quiet ";
	for (size_t i = 0; i < params.size(); i++) {
		os << " " << params.at(i);
	}

	os << " < " << scriptPath;
	os << " > " << RidgeRace::tmpdir << "/tmp" << RidgeRace::uid << ".r.out  2>&1";

	string call = os.str();

	return system(call.c_str());

}

int Caller::removeFile(string path) {
	std::ostringstream os;

	os << "rm " << path << "2> " << RidgeRace::tmpdir << "/tmp" << RidgeRace::uid << ".r.out";
	string call = os.str();

	return system(call.c_str());
}

bool Caller::fileExists(string path) {
	ifstream ifile(path.c_str());
	return ifile;
}

int Caller::makeDir(string path, bool quiet) {
	std::ostringstream os;

	os << "mkdir " << path;

	if (quiet)
		os << " 2> " << RidgeRace::tmpdir << "/tmp" << RidgeRace::uid << ".r.out";

	string call = os.str();

	return system(call.c_str());

}

string Caller::getTimestamp() {

	time_t t = time(0); // get time now
	struct tm* now = localtime(&t);

	std::ostringstream os;
	os << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-'
			<< now->tm_mday;
	os << "-" << now->tm_hour << ':' << now->tm_min;

	return os.str();
}

