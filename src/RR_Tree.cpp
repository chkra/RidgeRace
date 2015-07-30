/*
 * RR_Tree.cpp
 *
 *  Created on: Oct 16, 2013
 *      Author: ctusche
 */

#include "RR_Tree.h"
#include "StringManipulation.h"
#include "prefix.h"

RR_Tree::RR_Tree() {
	rrsimulatorIsSet = false;
	rrroot = 0;
	rrsize = 0;
}

RR_Tree::RR_Tree(RR_Node* root) {
	rrsimulatorIsSet = false;
	rrroot = root;
	rrsize = root->getSetOfAllIdsBelow().size() + 1;
}

RR_Tree::RR_Tree(string path) {
	rrsimulatorIsSet = false;
	NewickParser nwp(path);
	rrroot = (RR_Node*) nwp.getTree();
	rrsize = rrroot->getSetOfAllIdsBelow().size() + 1;
}

void RR_Tree::printSelector(TreePrintMode tpm, vector<string> relAnnotations,
		int minSupport, ostream &target) {
	switch (tpm) {
	case Newick:
		printNewick(rrroot, relAnnotations, minSupport, target);
		target << ';';
		break;
	case SimpleNewick:
		printSimpleNewick(rrroot, target);
		target << ';';
		break;
	default:
		printNewick(rrroot, relAnnotations, minSupport, target);
		break;

	}
}

void RR_Tree::print(TreePrintMode tpm, int minSupport, ostream &target) {
	vector<string> relAnnotations;
	map<string, TreeAnnotation*>::iterator iter;
	for (iter = rrannotations.begin(); iter != rrannotations.end(); iter++) {
		if (equal(iter->first, "sankoff")) {
			continue;
		}
		relAnnotations.push_back(iter->first);
	}

	printSelector(tpm, relAnnotations, minSupport, target);
}

void RR_Tree::print(TreePrintMode tpm, vector<string> relAnnotations,
		int minSupport, ostream &target) {
	printSelector(tpm, relAnnotations, minSupport, target);
}

void printSelector(vector<string> relAnnotations, int minSupport,
		ostream &target) {

}

void RR_Tree::printNewick(RR_Node* curNode, vector<string> relAnnotations,
		int minSupport, ostream &target) {

	if (curNode->isLeaf()) {
		target << '\'' << curNode->info << "\'";
	} else {

		size_t L = curNode->children.size();
		target << "(";
		for (size_t i = 0; i < L - 1; i++) {
			printNewick(curNode->children[i], relAnnotations, minSupport,
					target);
			target << ',';
		}
		printNewick(curNode->children[L - 1], relAnnotations, minSupport,
				target);
		target << ')';

		if (curNode->info == "") {
			ostringstream os;
			os << 'i' << curNode->id;
			curNode->info = os.str();
			target << '\'' << curNode->info << "\'";
		} else {
			target << '\'' << curNode->info << "\'";
		}
	}

	//if (!curNode->isLeaf()) {
	string annotation = getAnnotationStringForNode(curNode, relAnnotations,
			minSupport);
	if (annotation.size() > 0) {
		target << "[&";
		target << annotation;
		target << "]";
	}
	//}
	target << ":" << curNode->dist;

}

void RR_Tree::printSimpleNewick(RR_Node* curNode, ostream &target) {

	if (curNode->isLeaf()) {
		target << '\'' << curNode->info << "\'";
	} else {

		size_t L = curNode->children.size();
		target << "(";
		for (size_t i = 0; i < L - 1; i++) {
			printSimpleNewick(curNode->children[i], target);
			target << ',';
		}
		printSimpleNewick(curNode->children[L - 1], target);
		target << ')';

		if (curNode->info != "") {
			target << '\'' << curNode->info << "\'";
		} else {
			ostringstream os;
			os << 'i' << curNode->id;
			curNode->info = os.str();
			target << '\'' << curNode->info << "\'";
		}
	}
	target << ":" << curNode->dist;
}

#include "StringManipulation.h"
string RR_Tree::getAnnotationStringForNode(RR_Node* node,
		vector<string> relAnnotations, int minSupport) {

	if (relAnnotations.size() == 0)
		return "";

	ostringstream os;
	string name;
	size_t i = 0;

	IntegerTreeAnnotation* support;
	// if the user wants only nodes above a certain minSupport,
	// i.e. if minSupport != -1, first check if we can get support info
	// if so, check if support of this node is > minSupport
	// if not, return empty string, otherwise go on printing
	if (minSupport > 0) {
		try {
			support = (IntegerTreeAnnotation*) this->getAnnotation("support");
		} catch (...) {
			throw Exception(
					"ERROR in  RR_Tree::getAnnotationStringForNode: you demanded min support printing, but no support annotation was found\n");
		}
	}

	for (; i < (relAnnotations.size() - 1); i++) {
		name = relAnnotations[i];

		if (this->getAnnotation(name)->hasElement(node)
				&& this->getAnnotation(name)->getElementAsString(node).size()
						!= 0) {

			if (minSupport > 0 && support->getElement(node) < minSupport
					&& (equal(name, "ridgeWeight"))) {
				os << name << '=';
				os << "0.0";
				os << ',';
			} else if (minSupport > 0 && support->getElement(node) < minSupport
					&& (equal(name, "mutations"))) {
			} else {
				os << name << '=';
				os << this->getAnnotation(name)->getElementAsString(node);
				os << ',';
			}
		}
	}
	i = relAnnotations.size() - 1;
	name = relAnnotations[i];
	if (this->getAnnotation(name)->hasElement(node)) {
		os << name << '=';
		os << this->getAnnotation(name)->getElementAsString(node);
	}

	return os.str();
}

/*
 void RR_Tree::addIndex(RidgeRaceIndex rri, string nameOfIndex, string path,
 bool ridgeScheme) {
 _indexNames[rri] = nameOfIndex;
 TreeAnnotator TA;
 TA.annotate(_root, path, rri, ridgeScheme);
 }

 void RR_Tree::addIndex(RidgeRaceIndex rri, string nameOfIndex,
 NodeAnnotation<float>::Type v, bool ridgeScheme) {
 _indexNames[rri] = nameOfIndex;
 TreeAnnotator TA;
 TA.annotate(_root, rri, v, ridgeScheme);
 }

 void RR_Tree::addIndex(RidgeRaceIndex rri, string name) {
 _indexNames[rri] = name;
 }*/

#include <sstream>
void RR_Tree::setSummary(RidgeRaceIndex rri, string value) {

	stringstream sstr;
	float f;
	sstr << value;
	sstr >> f;
	rrsummary[rri] = f;
}

bool RR_Tree::hasInterestingSummary() {

	if (!contains(rrsummary, Constants::glsIndex)
			|| !contains(rrsummary, Constants::mlIndex)
			|| !contains(rrsummary, Constants::ridgeIndex)) {
		if (contains(rrsummary, Constants::ridgeIndex)) {
			return true;

		} else {
			throw Exception(
					"ERROR in RR_Tree::hasInterestingSummary: did not find all required indizes\n");
		}
	} else {
		return rrsummary[Constants::ridgeIndex]
				< 0.75 * rrsummary[Constants::mlIndex]
				|| rrsummary[Constants::ridgeIndex]
						< 0.75 * rrsummary[Constants::glsIndex]
				|| (0.75 * rrsummary[Constants::ridgeIndex]
						> rrsummary[Constants::mlIndex]
						&& 0.75 * rrsummary[Constants::ridgeIndex]
								> rrsummary[Constants::glsIndex]);
	}

}

float RR_Tree::correlate(string firstIndex, string secondIndex,
		bool includeLeafs) {

	if (!contains(rrannotations, firstIndex)
			|| !contains(rrannotations, firstIndex)) {
		cerr << "first index: " << firstIndex << endl;
		cerr << "second index: " << secondIndex << endl;
		throw Exception("ERROR in RR_Tree::correlate: no such index\n");
	}
	if (rrannotations.at(firstIndex)->getType() != "float"
			|| rrannotations.at(secondIndex)->getType() != "float") {
		cerr << "first index: " << rrannotations.at(firstIndex)->getType()
				<< endl;
		cerr << "second index: " << rrannotations.at(secondIndex)->getType()
				<< endl;
		throw Exception(
				"ERROR in RR_Tree::correlate: annotations are not of type float!\n");
	}

	TreeEvaluator TEval;
	float corr = TEval.correlate(
			(FloatTreeAnnotation*) rrannotations.at(firstIndex),
			(FloatTreeAnnotation*) rrannotations.at(secondIndex), includeLeafs);
	return corr;
}

float RR_Tree::MSE(string firstIndex, string secondIndex, bool includeLeafs) {

	if (!contains(rrannotations, firstIndex)
			|| !contains(rrannotations, firstIndex)) {
		cerr << "first index: " << firstIndex << endl;
		cerr << "second index: " << secondIndex << endl;
		throw Exception("ERROR in RR_Tree::MSE: no such index\n");
	}
	if (rrannotations.at(firstIndex)->getType() != "float"
			|| rrannotations.at(secondIndex)->getType() != "float") {
		cerr << "first index: " << rrannotations.at(firstIndex)->getType()
				<< endl;
		cerr << "second index: " << rrannotations.at(secondIndex)->getType()
				<< endl;
		throw Exception(
				"ERROR in RR_Tree::MSE: annotations are not of type float!\n");
	}

	TreeEvaluator TEval;
	float corr = TEval.MSE((FloatTreeAnnotation*) rrannotations.at(firstIndex),
			(FloatTreeAnnotation*) rrannotations.at(secondIndex), includeLeafs);
	return corr;
}

float RR_Tree::MPE(string firstIndex, string secondIndex) {

	if (!contains(rrannotations, firstIndex)
			|| !contains(rrannotations, firstIndex)) {
		cerr << "first index: " << firstIndex << endl;
		cerr << "second index: " << secondIndex << endl;
		throw Exception("ERROR in RR_Tree::MPE: no such index\n");
	}
	if (rrannotations.at(firstIndex)->getType() != "float"
			|| rrannotations.at(secondIndex)->getType() != "float") {
		cerr << "first index: " << rrannotations.at(firstIndex)->getType()
				<< endl;
		cerr << "second index: " << rrannotations.at(secondIndex)->getType()
				<< endl;
		throw Exception(
				"ERROR in RR_Tree::MPE: annotations are not of type float!\n");
	}

	TreeEvaluator TEval;
	float corr = TEval.MPE((FloatTreeAnnotation*) rrannotations.at(firstIndex),
			(FloatTreeAnnotation*) rrannotations.at(secondIndex));
	return corr;
}

void RR_Tree::clear() {

	map<string, TreeAnnotation*>::iterator iter;
	for (iter = rrannotations.begin(); iter != rrannotations.end(); iter++) {
		delete iter->second;
	}

	if (rrroot != 0) {
		delete rrroot;
		rrroot = 0;
	}
	rrannotations.clear();
}

void RR_Tree::setRandom(size_t size, string path) {

	vector<string> params;
	params.push_back(toString(size)); // tree size
	params.push_back(path);

	// create the random tree
	Caller C;
	std::stringstream scriptFilename;
	scriptFilename << RidgeRace::prefix << "/share/" << RidgeRace::progname << "/scripts/createRandomTree.r";
	C.callR(scriptFilename.str(), params);

	cerr << "created a new tree" << endl;

	// get the random tree
	this->clear();
	cerr << " removed old tree, updating now" << std::endl;
	NewickParser nwp(path);
	rrroot = (RR_Node*) nwp.getTree();
	rrsize = size;
}

void RR_Tree::printLeafFeatures(string path, RidgeRaceIndex rs) {
	TreeExtractor TE(rrroot);
	TreeAnnotation* as = this->getAnnotation(rs);
	if (as->getType() != "float") {
		throw Exception(
				"ERROR in RR_Tree::printLeafFeatures: can only print FloatTreeAnnotations\n");
	}
	TE.printLeafFeatures(path, (FloatTreeAnnotation*) as);
}

LeafFeatureData RR_Tree::getLeafFeatureData(RidgeRaceIndex rs) {
	TreeExtractor TE(rrroot);
	FloatTreeAnnotation* as;
	if (this->hasAnnotation(rs)) {
		as = (FloatTreeAnnotation*) this->getAnnotation(rs);
	}
	else {
		throw Exception(
						"ERROR in RR_Tree::getLeafFeatureData: no such index!\n");
	}
	if (as->getType() != "float") {
		cerr << "Type was " << as->getType() << endl;
		cerr << "Description was " << as->getDescription() << endl;
		throw Exception(
				"ERROR in RR_Tree::getLeafFeatureData: can only get FloatTreeAnnotations\n");
	}
	return TE.getLeafFeatureData(as);
}

/*LeafFeatureData getLeafFeatureMatrix() {
 TreeExtractor TE(_root);
 return TE.getLeafFeatureData();
 }*/

void RR_Tree::setSimulator(RegimeSpecification rs) {
	rrS = Simulator(rrroot, rs);
	rrrs = rs;

	rrsimulatorIsSet = true;
	//addIndex(simulationIndex, "simulation");
	//addIndex(regimeIndex, "regime");
	//addIndex(stdIndex, "std");
}

void RR_Tree::setBiasedSimulator(RegimeSpecification rs) {
	rrS = BiasedSimulator(rrroot, rs);
	rrrs = rs;

	rrsimulatorIsSet = true;
	//addIndex(simulationIndex, "simulation");
	//addIndex(regimeIndex, "regime");
	//addIndex(stdIndex, "std");
}

RegimeSpecification RR_Tree::getRegimeSpecification() {
	if (!rrsimulatorIsSet) {
		throw Exception(
				"ERROR in RR_Tree::getRegimeSpecification: please set simulator!\n");
	}
	return rrrs;
}

void RR_Tree::simulateBM() {
	if (!rrsimulatorIsSet) {
		throw Exception(
				"ERROR in RR_Tree::simulateBM: please set simulator!\n");
	}
	SimulationPackage sm = rrS.simulate();
	this->addAnnotation(Constants::regimeIndex, sm.regimeAnno);
	this->addAnnotation(Constants::simulationIndex, sm.simulation);
	this->addAnnotation(Constants::stdIndex, sm.stdAnno);

}

size_t RR_Tree::getSize() {
	return rrsize;
}

void RR_Tree::annotateNodeHeight() {

	// if we called annotateNodeHeight before, do nothing
	if (rrroot->_height != -1) {
		return;
	}
	// otherwise run through the tree once
	else {
		rrroot->annotateNodeHeight();
	}
}

int RR_Tree::annotateNodeSupport(RR_Node* curNode) {

	IntegerTreeAnnotation* sAnno;
	sAnno = (IntegerTreeAnnotation*) this->getAnnotation("support");

	if (curNode->isLeaf()) {
		sAnno->setElement(curNode, 1);
		return 1;
	} else {
		int sum = 0;
		for (size_t c = 0; c < curNode->children.size(); c++) {
			sum += annotateNodeSupport(curNode->children[c]);
		}
		sAnno->setElement(curNode, sum);
		return sum;
	}

}

void RR_Tree::annotateNodeSupport() {
	IntegerTreeAnnotation* nSupport = new IntegerTreeAnnotation("support");
	this->addAnnotation("support", nSupport);
	annotateNodeSupport(rrroot);
}

RR_Node* RR_Tree::getRoot() {
	return rrroot;
}

void RR_Tree::addAnnotation(string key, TreeAnnotation* anno, bool deleteOld) {

	if (contains(rrannotations, key)) {
		if (deleteOld) {
			delete rrannotations[key];
			rrannotations[key] = 0;
		}
		else {
			//cerr << "---WARNING in RR_Tree::addAnnotation: overwriting annotation ";
			//cerr << key << " with " << anno->getDescription() << '\n';
		}
	}
	rrannotations[key] = anno;

}

TreeAnnotation* RR_Tree::getAnnotation(string key) {
	if (!contains(rrannotations, key)) {
		cerr << "ERROR in RR_Tree::getAnnotation: did not find annotation ";
		cerr << key << '\n';
		throw Exception("index not found\n");
	}

	return rrannotations[key];

}

bool RR_Tree::hasAnnotation(string key) {
	return (contains(rrannotations, key));
}

void RR_Tree::dropAnnotation(string key) {
	if (contains(rrannotations, key)) {
		rrannotations.erase(key);
	}
}

// ############################################################################

