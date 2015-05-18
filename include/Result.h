/*
 * Result.h
 *
 *  Created on: Mar 22, 2013
 *      Author: ctusche
 */

#ifndef RESULT_H_
#define RESULT_H_

#include <iostream>
#include <vector>
#include <fstream>

#include "Exception.h"
#include "Externals.h"
#include "RR_Tree.h"

using namespace std;

class Result {

protected:
	string _type;


protected:
	vector<string> _paramEvalStrings;

	void printToLogfile(ostream &target = std::cout);

	void printToLogfile(string path);

public:

	virtual void print(ostream &target = std::cout) = 0;

	virtual void print(string path) = 0;

	//void addEntry(pair<string, string> e) {
	//	_paramEvalStrings.push_back(e);
	//}


	void addEntry(vector<string> e, RR_Tree t);

	string getType();

	string getCharacteristic();

	virtual ~Result() {};
};

// ============================================================================

class ML_Result: public Result {

public:
	ML_Result() :
		Result() {
		_type = "ML";
	}

	~ML_Result(){};

	void print(ostream &target = std::cout) {
		printToLogfile(target);
	}

	void print(string path) {
		printToLogfile(path);
	}
};

class Empty_Result: public Result {

public:
	Empty_Result() :
		Result() {
		_type = "empty";
	}

	~Empty_Result(){};

	void print(ostream &target = std::cout) {};

	void print(string path) {};
};

// ============================================================================

class GLS_Result: public Result {

public:
	GLS_Result() :
		Result() {
		_type = "GLS";
	}

	~GLS_Result(){};

	void print(ostream &target = std::cout) {
		printToLogfile(target);
	}

	void print(string path) {
		printToLogfile(path);
	}
};

// ============================================================================

class Ridge_Result: public Result {

public:
	Ridge_Result() :
		Result() {
		_type = "Ridge";
	}

	~Ridge_Result(){
		// this is paranoid, should not be necessary?
		_paramEvalStrings.clear();
	};

	void print(ostream &target = std::cout) {
		printToLogfile(target);
	}

	void print(string path) {
		printToLogfile(path);
	}
};


// ============================================================================

class PhyloMean_Result: public Result {

public:

	float _mean;

	PhyloMean_Result() :
		Result() {
		_type = "PhyloMean";
		_mean = -1.0f;
	}

	~PhyloMean_Result(){};

	void print(ostream &target = std::cout) {
		printToLogfile(target);
	}

	void print(string path) {
		printToLogfile(path);
	}
};

// ============================================================================

class AnnoTree_Result: public Result {

private:
	string _treePath;
	size_t _id;
	void printTree();

public:
	AnnoTree_Result() :
		Result() {
		_type = "AnnoTree";
		_treePath = "";
	}

	~AnnoTree_Result(){};

	void setTreePath(string path);

	void setTreeID(size_t id);

	/* overwrites method of base class - will not write parameters and
	 * correlations to file, but makes a nice picture instead
	 */
	void print(string path);

	/* overwrites method of base class - will not write parameters and
	 * correlations to file, but makes a nice picture instead
	 */
	void print(ostream &target = std::cout);

};

#endif /* RESULT_H_ */
