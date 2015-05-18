/*
 * RR_TreeAnnotation.h
 *
 *  Created on: Oct 16, 2013
 *      Author: ctusche
 */

#ifndef RR_TREEANNOTATION_H_
#define RR_TREEANNOTATION_H_

#include "Parser.h"
#include "Files.h"

#include "RR_Node.h"
#include "SetsAndMaps.h"
#include <sstream>
#include <ostream>

// ############################################################################

class TreeAnnotation {

public:
	virtual string getType() = 0;
	virtual string getDescription() = 0;
	virtual string getElementAsString(RR_Node* r) = 0;
	virtual bool hasElement(RR_Node* r) = 0;
	virtual void print(std::ostream &os) = 0;
	virtual size_t size() = 0;
	virtual ~TreeAnnotation() {};
};

template<typename T>
class GenericTreeAnnotation: public TreeAnnotation {

protected:

	string _description;
	std::map<RR_Node*, T> _data;

public:

	GenericTreeAnnotation() {
		_description = "generic";
	}

	GenericTreeAnnotation(string desc) {
		_description = desc;
	}

	~GenericTreeAnnotation() {
	}
	;

	string getDescription() {
		return _description;
	}

	string getType() {
		return "generic";
	}

	std::map<RR_Node*, T> getFullData() {
		return _data;
	}

	size_t size() {
		return _data.size();
	}

	T& operator[](RR_Node* r) {
		try {
			return _data.at(r);
		} catch (...) {
			cerr
					<< "ERROR in GenericTreeAnnotation::operator[]: no such index!\n";
			cerr << " You tried to access node " << r->info << " with id "
					<< r->id << '\n';
			cerr << " leaf node: " << r->isLeaf() << '\n';
			cerr << " description of this annotation: " << this->getDescription() << '\n';
			cerr << " type of this annotation: " << this->getType() << '\n';
			throw Exception("No such index\n");
		}
	}

	T& getElement(RR_Node* r) {
		try {
			return _data.at(r);
		} catch (...) {
			cerr
					<< "ERROR in GenericTreeAnnotation::getElement: no such index!\n";
			cerr << " You tried to access node " << r->info << " with id "
					<< r->id << '\n';
			cerr << " leaf node: " << r->isLeaf() << '\n';
			cerr << " description of this annotation: " << this->getDescription() << '\n';
			cerr << " type of this annotation: " << this->getType() << '\n';
			throw Exception("No such index\n");
		}
	}

	void setElement(RR_Node* r, T s) {
		_data[r] = s;
	}

	bool hasElement(RR_Node* r) {
		return _data.find(r) != _data.end();
	}

	void print(std::ostream &os=std::cerr) {
		typename std::map<RR_Node*, T>::iterator iter;
		for (iter = _data.begin(); iter != _data.end(); iter++) {
			//
			if (iter->first->info != "") {
				os << iter->first->info;
			}
			else {
				os << "id_" << iter->first->id;
			}
			os << '\t';
			os << getElementAsString(iter->first);
			os << '\n';
		}
	}

	// @ todo there should be some generic solution to this?!?
	string getElementAsString(RR_Node* node) {
		return "##NO_STRING_REPRESENTATION_AVAILABLE##";
	}

};

// ############################################################################

enum FileInputMode {
	Fasta, TabSeq
};

class StringTreeAnnotation: public GenericTreeAnnotation<string> {

private:

	map<string, string> readFastaFile(string path);

	void mapAnnotation(RR_Node* curNode, const map<string, string> &infoMap);

public:

	StringTreeAnnotation(string path, FileInputMode mode, RR_Node* root,
			string desc);

	~StringTreeAnnotation();

	string getType() {
		return "string";
	}

	void print();

	string getElementAsString(RR_Node* node) {
		ostringstream os;
		if (!this->hasElement(node)) {
			throw Exception(
					"ERROR in GenericTreeAnnotation::getElementAsString: no such element\n");
		}
		os << _data.at(node);
		return os.str();
	}

};

// ############################################################################

class IntegerTreeAnnotation: public GenericTreeAnnotation<int> {

public:

	IntegerTreeAnnotation(string desc);
	~IntegerTreeAnnotation() {
	}
	;

	string getType() {
		return "integer";
	}

	string getElementAsString(RR_Node* node) {
		ostringstream os;
		if (!this->hasElement(node)) {
			throw Exception(
					"ERROR in IntegerTreeAnnotation::getElementAsString: no such element\n");
		}
		os << _data.at(node);
		return os.str();
	}
};

// ############################################################################

class FloatTreeAnnotation: public GenericTreeAnnotation<float> {

private:

	bool _IS_RIDGE_SCHEME;

	map<string, float> readInputFile(string path);

	// @ todo: simplify this using updateToRidgeSchemeInternal
	void mapAnnotation(RR_Node* curNode, const map<string, float> &infoMap,
			bool ridgeScheme = false);

	void updateToRidgeSchemeInternal(RR_Node* curNode);

public:

	FloatTreeAnnotation(string path, RR_Node* root, string desc,
			bool ridgeScheme = false);
	FloatTreeAnnotation(string desc);
	~FloatTreeAnnotation();

	string getType() {
		return "float";
	}
	void updateToRidgeScheme(RR_Node* curNode);

	string getElementAsString(RR_Node* node);

	void substractFromValues(float f);

	void substractFromValues(FloatTreeAnnotation* fta);

	FloatTreeAnnotation* getIdenticalCopy(string description) {
		FloatTreeAnnotation* gta = new FloatTreeAnnotation(description);
		typename std::map<RR_Node*, float>::iterator iter;
		for (iter = _data.begin(); iter != _data.end(); iter++) {
			gta->setElement(iter->first, iter->second);
		}
		return gta;
	}

	float getSumSq(bool includeLeafs) {
		float sum = 0;
		size_t count = 0;
		typename std::map<RR_Node*, float>::iterator iter;
		for (iter = _data.begin(); iter != _data.end(); iter++) {
			if (iter->first->isLeaf() & !includeLeafs) {
				continue;
			}
			sum += (iter->second * iter->second);
			count ++;
		}
		return sqrt(sum / float(count));
	}
};

// ############################################################################

class FloatVectorTreeAnnotation: public GenericTreeAnnotation<vector<float> > {

private:
	map<string, float> readInputFile(string path);

	void mapAnnotation(RR_Node* curNode,
			const map<string, vector<float> > &infoMap);

	void updateToRidgeSchemeInternal(RR_Node* curNode);

public:

	FloatVectorTreeAnnotation(RR_Node* root, string desc);
	FloatVectorTreeAnnotation(string desc);
	~FloatVectorTreeAnnotation() {};

	string getType() {
		return "float vector";
	}

	void print();
	void updateToRidgeScheme(RR_Node* curNode);

	string getElementAsString(RR_Node* node);
};

// ############################################################################

class Mutation {

public:

	Mutation() {
		_from = "###";
		_to = "###";
		_pos = -1;
	}

	Mutation(string from, string to, size_t pos) {
		_from = from;
		_to = to;
		_pos = pos;
	}

	string _from;
	size_t _pos;
	string _to;

	string getString() {
		ostringstream os;
		os << _from << _pos << _to;
		return os.str();
	}

	bool operator<(const Mutation& rhs) const {
		if (_pos != rhs._pos) {
			return _pos < rhs._pos;
		} else if (_from != rhs._from) {
			return _from < rhs._from;
		} else if (_to != rhs._to) {
			return _to < rhs._to;
		} else
			return false;
	}

	bool operator==(const Mutation& rhs) const {
		return (_pos == rhs._pos) & (_from == rhs._from) & (_to == rhs._to);
	}
};

// ############################################################################

class MutationTreeAnnotation: public GenericTreeAnnotation<vector<Mutation> > {

private:
	bool _modifiedOutput;
	Table _mutNames;
public:

	MutationTreeAnnotation(string desc);
	~MutationTreeAnnotation();

	string getType() {
		return "mutation";
	}

	void print();

	void rewriteMutStrings(Table T);

	string getElementAsString(RR_Node* node);
};

// ############################################################################

/**
 * A map for storing temporary information of the Sankoff algorithm.
 *
 * Used for a single node.
 * CharacterState --> vector< cost of that state at position i >
 */
typedef map<string, vector<float> > SankoffMap;

class SankoffTreeAnnotation: public GenericTreeAnnotation<SankoffMap> {

private:
	//std::map<RR_Node*, SankoffMap> _data;

	void mapAnnotation(RR_Node* curNode,
			const map<string, SankoffMap> &infoMap);

public:

	SankoffTreeAnnotation(string desc);
	~SankoffTreeAnnotation() {};

	void printElement(RR_Node* r);

	string getType() {
		return "Sankoff";
	}

	//void print();

	string getElementAsString(RR_Node* node);

};

// ############################################################################

class LeafFeatureData {

public:

	FloatVectorTreeAnnotation* branchLengths;
	FloatTreeAnnotation* phenotypes;

	LeafFeatureData() {
		branchLengths = new FloatVectorTreeAnnotation("branchLengths");
		phenotypes = new FloatTreeAnnotation("phenotype");
	}

	~LeafFeatureData() {
		// @ todo not sure if this a good idea?
		//delete branchLengths;
		//delete phenotypes;
	}

	// the intrinsic order of nodesOfThisTree is the order in which
	// the columns of branchLengths are sorted
	// this is because we run through this set with an iterator every time
	// we create a row of branchLengths - if the current iterator element
	// is on the tree path, we write a value to branchLengths,
	// otherwise we write zero
	std::set<RR_Node*> nodesOfThisTree;

	void print(std::ostream &out = std::cout);

};

// ############################################################################

#endif /* RR_TREEANNOTATION_H_ */
