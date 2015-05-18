/*
 * RR_TreeAnnotation.cpp
 *
 *  Created on: Oct 16, 2013
 *      Author: ctusche
 */

#include "RR_TreeAnnotation.h"
#include "Node.h"

map<string, string> StringTreeAnnotation::readFastaFile(string path) {
	DataParser DP(path);
	return DP.readFasta();
}

void StringTreeAnnotation::mapAnnotation(RR_Node* curNode,
		const map<string, string> &infoMap) {

	// if the map contains an info entry, store that info to _data
	if (contains(infoMap, curNode->info)) {
		_data[curNode] = infoMap.at(curNode->info);
	}

	for (size_t c = 0; c < curNode->children.size(); c++) {
		mapAnnotation(curNode->children.at(c), infoMap);
	}
}

StringTreeAnnotation::StringTreeAnnotation(string path, FileInputMode mode,
		RR_Node* root, string desc) :
		GenericTreeAnnotation<string>(desc) {

	map<string, string> inputFile;

	switch (mode) {
	case TabSeq:
		throw Exception(
				"StringTreeAnnotation::Constructor: TabSeq is not implemented");
	case Fasta:
	default:
		inputFile = readFastaFile(path);
		break;
	}

	mapAnnotation(root, inputFile);
}

StringTreeAnnotation::~StringTreeAnnotation() {
}
FloatTreeAnnotation::~FloatTreeAnnotation() {
}
MutationTreeAnnotation::~MutationTreeAnnotation() {
}

void StringTreeAnnotation::print() {

	size_t MAXL = 5;

	map<RR_Node*, string>::iterator iter;
	for (iter = _data.begin(); iter != _data.end(); iter++) {
		cerr << iter->first->id << '\t';
		if (iter->second.length() > MAXL) {
			cerr << iter->second.substr(0, MAXL) << "...\t";
		} else {
			cerr << iter->second << '\t';
		}
		cerr << iter->first->info << '\n';
	}
}

// ############################################################################

void FloatTreeAnnotation::updateToRidgeScheme(RR_Node* curNode) {

	if (_IS_RIDGE_SCHEME) {
		throw Exception(
				"ERROR in FloatTreeAnnotation::updateToRidgeScheme: is already ridge scheme!\n");
	}
	_IS_RIDGE_SCHEME = true;

	updateToRidgeSchemeInternal(curNode);
}

void FloatTreeAnnotation::updateToRidgeSchemeInternal(RR_Node* curNode) {

	RR_Node* fatherNode = curNode->father;
	float fatherValue;
	if (fatherNode == 0) {
		fatherValue = 0;
	} else {
		fatherValue = this->getElement(fatherNode);
	}

	// get current nodes distance and value
	float dist = curNode->dist;
	float inValue = this->getElement(curNode);

	// currect value and update it in internal data structure
	float thisNodesValue = fatherValue + (dist * inValue);
	this->setElement(curNode, thisNodesValue);

	for (size_t c = 0; c < curNode->children.size(); c++) {
		updateToRidgeSchemeInternal(curNode->children.at(c));
	}

}

void FloatTreeAnnotation::mapAnnotation(RR_Node* curNode,
		const map<string, float> &infoMap, bool ridgeScheme) {

	if (ridgeScheme) {

		if (!_IS_RIDGE_SCHEME) {
			throw Exception(
					"ERROR in FloatTreeAnnotation::mapAnnotation(ridgeScheme): no ridge scheme specified!\n");
		}

		if (!contains(infoMap, curNode->info)) {
			cerr << "Did not find entry for node " << curNode->info << '\n';
			throw Exception(
					"ERROR in FloatTreeAnnotation::mapAnnotation(ridgeScheme): no such entry");
		}

		RR_Node* fatherNode = curNode->father;
		float fatherValue;
		if (fatherNode == 0) {
			fatherValue = 0;
		} else {
			fatherValue = this->getElement(fatherNode);
		}

		float dist = curNode->dist;
		float inValue = infoMap.at(curNode->info);

		float thisNodesValue = fatherValue + (dist * inValue);
		this->setElement(curNode, thisNodesValue);
	} else {

		if (_IS_RIDGE_SCHEME) {
			throw Exception(
					"ERROR in FloatTreeAnnotation::mapAnnotation(ridgeScheme): ridge scheme specified!\n");
		}

		// default case, nothing special, just copying
		// if the map contains an info entry, store that info to _data
		if (contains(infoMap, curNode->info)) {
			_data[curNode] = infoMap.at(curNode->info);
		}
	}

	for (size_t c = 0; c < curNode->children.size(); c++) {
		mapAnnotation(curNode->children.at(c), infoMap, ridgeScheme);
	}
}

FloatTreeAnnotation::FloatTreeAnnotation(string path, RR_Node* root,
		string desc, bool ridgeScheme) :
		GenericTreeAnnotation<float>(desc) {

	_IS_RIDGE_SCHEME = ridgeScheme;

	map<string, float> inputFile;

	DataParser DP(path);
	inputFile = DP.readStringFloat();

	mapAnnotation(root, inputFile, ridgeScheme);

}

void FloatTreeAnnotation::substractFromValues(float f) {
	map<RR_Node*, float>::iterator iter;
	for (iter = _data.begin(); iter != _data.end(); iter++) {
		iter->second -= f;
	}
}

void FloatTreeAnnotation::substractFromValues(FloatTreeAnnotation* fta) {
	map<RR_Node*, float>::iterator iter;
	RR_Node* curNode;

	for (iter = _data.begin(); iter != _data.end(); iter++) {
		curNode = iter->first;
		if (!fta->hasElement(curNode)) {
			throw Exception("ERROR in FloatTreeAnnotation::substractFromValues(FloatTreeAnnotation*): fta's do not fit!\n");
		}
		// else this node is in fta - get float value for it
		iter->second -= fta->getElement(curNode);
	}
}

FloatTreeAnnotation::FloatTreeAnnotation(string desc) :
		GenericTreeAnnotation<float>(desc) {
	_IS_RIDGE_SCHEME = false;
}

string float2figtreeString(float f) {
	ostringstream os;
	double roundedVal = round_nplaces(f, 2);
	int someInt = roundedVal;

	if (float(someInt) == roundedVal) {
		os << someInt << ".0";
	} else {
		os << roundedVal;
	}
	return os.str();
}

string FloatTreeAnnotation::getElementAsString(RR_Node* node) {
	ostringstream os;
	if (!this->hasElement(node)) {
		throw Exception(
				"ERROR in GenericTreeAnnotation::getElementAsString: no such element\n");
	}
	float f = _data.at(node);
	return float2figtreeString(f);
}

IntegerTreeAnnotation::IntegerTreeAnnotation(string desc) :
		GenericTreeAnnotation<int>(desc) {
}

FloatVectorTreeAnnotation::FloatVectorTreeAnnotation(string desc) :
		GenericTreeAnnotation<vector<float> >(desc) {
}

SankoffTreeAnnotation::SankoffTreeAnnotation(string desc) :
		GenericTreeAnnotation<SankoffMap>(desc) {
}

// ############################################################################

void FloatVectorTreeAnnotation::mapAnnotation(RR_Node* curNode,
		const map<string, vector<float> > &infoMap) {

	// if the map contains an info entry, store that info to _data
	if (contains(infoMap, curNode->info)) {
		_data[curNode] = infoMap.at(curNode->info);
	}

	for (size_t c = 0; c < curNode->children.size(); c++) {
		mapAnnotation(curNode->children.at(c), infoMap);
	}
}

FloatVectorTreeAnnotation::FloatVectorTreeAnnotation(RR_Node* root, string desc) :
		GenericTreeAnnotation<vector<float> >(desc) {
}

void FloatVectorTreeAnnotation::print() {
	map<RR_Node*, vector<float> >::iterator iter;
	for (iter = _data.begin(); iter != _data.end(); iter++) {
		cerr << iter->first->info << '\t';
		for (size_t i = 0; i < iter->second.size(); i++) {
			cerr << iter->second.at(i) << '\t';
		}
		cerr << '\t';
	}
}

string FloatVectorTreeAnnotation::getElementAsString(RR_Node* node) {
	ostringstream os;
	if (!this->hasElement(node)) {
		throw Exception(
				"ERROR in GenericTreeAnnotation::getElementAsString: no such element\n");
	}
	vector<float> v = _data.at(node);
	for (size_t i = 0; i < v.size(); i++) {
		os << v.at(i) << '_';
	}
	return os.str();
}

// ############################################################################

void SankoffTreeAnnotation::mapAnnotation(RR_Node* curNode,
		const map<string, SankoffMap> &infoMap) {

	// if the map contains an info entry, store that info to _data
	if (contains(infoMap, curNode->info)) {
		_data[curNode] = infoMap.at(curNode->info);
	}

	for (size_t c = 0; c < curNode->children.size(); c++) {
		mapAnnotation(curNode->children.at(c), infoMap);
	}
}

//SankoffTreeAnnotation::SankoffTreeAnnotation(RR_Node* root, string desc) :
//		GenericTreeAnnotation<SankoffMap>(desc) {
//}

void SankoffTreeAnnotation::printElement(RR_Node* r) {
	cout << getElementAsString(r);
}

string SankoffTreeAnnotation::getElementAsString(RR_Node* r) {
	ostringstream os;
	try {
		SankoffMap sm = _data[r];
		SankoffMap::iterator iter;
		for (iter = sm.begin(); iter != sm.end(); iter++) {
			//os << iter->first << '\t';
			os << "_" << iter->first << "_";
			for (size_t k = 0; (k < iter->second.size()-1) & (k < 9); k++) {
				//os << iter->second.at(k) << '\t';
				os << iter->second.at(k) << ';';
			}
			os << iter->second.at( iter->second.size() -1 );
			//os << '\n';
		}
	} catch (...) {
		cerr
				<< "ERROR in SankoffTreeAnnotation::getElementAsString(): no such index!\n";
		cerr << " You tried to access node " << r->info << " with id " << r->id
				<< '\n';
		throw Exception("No such index\n");
	}
	return os.str();
}

// ############################################################################

void MutationTreeAnnotation::print() {

	size_t MAXL = 5;

	map<RR_Node*, vector<Mutation> >::iterator iter;
	for (iter = _data.begin(); iter != _data.end(); iter++) {
		cerr << iter->first->id << '\t';
		vector<Mutation> v = iter->second;

		for (size_t i = 0; (i < v.size()) & (i < MAXL); i++) {
			cerr << v[i].getString() << " ";
		}

		cerr << '\t' << iter->first->info << '\n';
	}
}

MutationTreeAnnotation::MutationTreeAnnotation(string desc) :
		GenericTreeAnnotation<vector<Mutation> >(desc) {
	_description = desc;
	_modifiedOutput = false;
	_mutNames = Table();
}


void MutationTreeAnnotation::rewriteMutStrings(Table T) {
	_modifiedOutput = true;
	_mutNames = T;
}

string MutationTreeAnnotation::getElementAsString(RR_Node* node) {
	ostringstream os;
	if (!this->hasElement(node)) {
		throw Exception(
				"ERROR in GenericTreeAnnotation::getElementAsString: no such element\n");
	}
	vector<Mutation> v = _data.at(node);

	if (v.size() > 5) {
		os << "(" << v.size() <<  " mut.)";
		return os.str();
	}

	size_t geneIndex;
	for (size_t i = 0; i < v.size(); i++) {
		if (_modifiedOutput) {
			if (v.at(i)._from > v.at(i)._to) {
				os << '-';
			}
			else {
				os << '+';
			}
			geneIndex = v.at(i)._pos - 1;
			os << _mutNames.T.at(geneIndex).at(0) << ';';
		}
		else {
			os << v.at(i).getString();
		}
	}
	return os.str();
}




