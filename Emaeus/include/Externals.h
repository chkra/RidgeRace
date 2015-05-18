/*
 * Externals.h
 *
 *  Created on: Nov 14, 2012
 *      Author: ctusche
 */

#ifndef EXTERNALS_H_
#define EXTERNALS_H_


#include <vector>
#include <string>
using namespace std;

class Caller {

public:
	Caller() {};


	int callR(string scriptPath, vector<string> params);

	int removeFile(string path);

	bool fileExists(string path);

	int makeDir(string path, bool quiet);

	string getTimestamp();

};

#endif /* EXTERNALS_H_ */
