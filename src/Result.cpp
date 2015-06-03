/*
 * Result.cpp
 *
 *  Created on: Apr 9, 2013
 *      Author: ctusche
 */

#include "Result.h"
#include "prefix.h"

// ############################################################################

void Result::printToLogfile(ostream &target) {

	for (size_t i = 0; i < _paramEvalStrings.size(); i++) {
		target << _type << '\t';
		target << _paramEvalStrings.at(i) << '\n';
	}
}

void Result::printToLogfile(string path) {

	if (_paramEvalStrings.size() == 0)
		return;

	ofstream os(path.c_str(), fstream::app);
	this->print(os);
	os.close();
}

//void addEntry(pair<string, string> e) {
//	_paramEvalStrings.push_back(e);
//}

// @ todo this is ugly
void Result::addEntry(vector<string> e, RR_Tree t) {

	ostringstream os;
	os << t.getSize() << '\t';
	os << t.getRegimeSpecification().toString() << '\t';
	for (size_t i=0; i<e.size(); i++) {
		os << e.at(i) << '\t';
	}
	_paramEvalStrings.push_back(os.str());
}


string Result::getCharacteristic() {
	size_t last =  _paramEvalStrings.size() - 1;
	return _paramEvalStrings.at(last);
}


string Result::getType() {
	return _type;
}

// ############################################################################

void AnnoTree_Result::setTreePath(string path) {
	_treePath = path;
}

void AnnoTree_Result::setTreeID(size_t id) {
	_id = id;
}


/* overwrites method of base class - will not write parameters and
 * correlations to file, but makes a nice picture
 */
void AnnoTree_Result::print(string path) {

	this->printTree();
}


void AnnoTree_Result::print(ostream &target) {

	this->printTree();
}




void AnnoTree_Result::printTree() {

	// do not write more than 100 trees
	if (_id > 100) {
		return;
	}


	if (_treePath.size() == 0) {
		throw Exception(
				"ERROR in AnnoTree_Result::print(path): _treePath not set\n");
	}
	ifstream is(_treePath.c_str());
	if (!is.good()) {
		cerr << _treePath << endl;
		throw Exception(
				"ERROR in AnnoTree_Result::print(path): unable to open file\n");
	}
	is.close();

	// else
	vector<string> params;
	params.push_back(_treePath);

	Caller C;

	std::stringstream scriptFilename;
	scriptFilename << RidgeRace::prefix << "/share/" << RidgeRace::progname << "/scripts/drawTree.r";
	C.callR(scriptFilename.str(), params);
}












