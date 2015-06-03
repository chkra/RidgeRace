/*
 * Node.cpp
 *
 *  Created on: Nov 8, 2012
 *      Author: ctusche
 */

#include "Node.h"
#include "Exception.h"
#include "StringManipulation.h"
#include "../../include/prefix.h"

unsigned int Node::curID = 1;

unsigned int MAX_FEATURE_OUTPUT = 10;

#include <cmath>
double round_nplaces(double value, int to) {
	double places = pow(10.0, to);
	return round(value * places) / places;
}

void Node::printNewick(std::ostream &target, bool quotes) {

	size_t L = this->children.size();

	if (L == 0) {
		target << '\'' << this->info << "\'";
		//target << this->info;
	} else {
		target << "(";
		for (size_t i = 0; i < L - 1; i++) {
			this->children[i]->printNewick(target, quotes);
			target << ',';
		}
		this->children[L - 1]->printNewick(target, quotes);
		target << ')';
		if (this->info != "") {
			if (quotes) {
				target << '\'' << this->info << "\'";
			} else {
				target << this->info;
			}
			//target << this->info;
		}
	}

	/*
	// @todo: this is an evil hack to prevent that a complete genetic sequence
	// is written in features. solve this by giving a vector of output-relevant
	// features to this method
	if (features.size() <= MAX_FEATURE_OUTPUT) {

		if (features.size() > 0) {
			target << "[&";
		}

		double roundedVal;
		int someInt;

		// @ todo this is also dangerous ... since features is a map, we
		// should either
		// - make it a vector again
		// - check if this index really exists
		for (size_t i = 0; i < this->features.size(); i++) {

			// @ todo
			// the class Tree has a vector called phenoNames for this purpose
			target << "f" << (i + 1) << "=";
			roundedVal = round_nplaces(features[i], 2);
			target << roundedVal;

			// This is a hack for figtree. It fails to parse a double value if
			// there is at least one occurence of that value as an integer. So
			// in case we have a 'round' number like 1, write it 1.0.
			//
			someInt = roundedVal;
			if (someInt == roundedVal) {
				target << ".0";
			}

			if (i < this->features.size() - 1) {
				target << ',';
			}
		}
		if (features.size() > 0) {
			target << "]";
		}
	}*/

	target << ":" << this->dist;

}

void Node::printNewick(std::ostream &target, map<int, std::string> featureNames,
		bool quotes) {

	size_t L = this->children.size();

	if (L == 0) {
		target << '\'' << this->info << "\'";
		//target << this->info;
	} else {
		target << "(";
		for (size_t i = 0; i < L - 1; i++) {
			this->children[i]->printNewick(target, featureNames, quotes);
			target << ',';
		}
		this->children[L - 1]->printNewick(target, featureNames, quotes);
		target << ')';
		if (this->info != "") {
			if (quotes) {
				target << '\'' << this->info << "\'";
			} else {
				target << this->info;
			}
			//target << this->info;
		}
	}

	// @todo: this is an evil hack to prevent that a complete genetic sequence
	// is written in features. solve this by giving a vector of output-relevant
	// features to this method
	/*if (features.size() <= MAX_FEATURE_OUTPUT) {

		if (features.size() > 0) {
			target << "[&";
		}

		double roundedVal;
		int someInt;

		// @ todo this is also dangerous ... since features is a map, we
		// should either
		// - make it a vector again
		// - check if this index really exists

		std::map<size_t, float>::iterator iter;
		int index;
		float fValue;
		size_t k = 0;

		for (iter = features.begin(); iter != features.end(); iter++) {

			index = iter->first;
			fValue = iter->second;
			k++;

			// @ todo
			// the class Tree has a vector called phenoNames for this purpose

			if (featureNames.find(index) == featureNames.end()) {
				cerr << "  WARNING: unable to find feature name for index "
						<< index << endl;
				target << "F" << k << "=";
			} else {
				target << featureNames.at(index) << "=";
			}

			roundedVal = round_nplaces(fValue, 2);
			target << roundedVal;

			 // This is a hack for figtree. It fails to parse a double value if
			 // there is at least one occurence of that value as an integer. So
			 // in case we have a 'round' number like 1, write it 1.0.
			 //
			someInt = roundedVal;
			if (someInt == roundedVal) {
				target << ".0";
			}

			if (k < this->features.size()) {
				target << ',';
			}
		}
		if (features.size() > 0) {
			target << "]";
		}
	}*/

	target << ":" << this->dist;

}

// @ todo put all this into an init() function
Node::Node(Node* a, Node* b) {

	this->id = curID;
	this->dist = 0;
	this->_height = -1;

	ostringstream os;
	os << 'i' << id;
	this->info = os.str();

	curID++;

	this->children.push_back(a);
	a->father = this;
	this->children.push_back(b);
	b->father = this;

	this->father = 0;



}

Node::Node() {
	this->id = curID;
	this->dist = 0;
	this->_height = -1;
	this->father = 0;

	ostringstream os;
	os << 'i' << id;
	this->info = os.str();

	curID++;
}

Node::~Node() {
	for (size_t i = 0; i < this->children.size(); i++) {
		delete this->children[i];
	}
}

void Node::printGraphViz(std::ofstream &target) {

	target << this->id;
	target << " [label=\"";
	if (this->info != "")
		target << this->info << "\\n";

	target << "(" << this->id << ")\\n";
	target << "(phenotype info here)";
	target << "\"];\n";

	for (size_t i = 0; i < this->children.size(); i++) {
		target << this->id;
		target << " -> ";
		target << this->children[i]->id;
		target << ";\n";
		this->children[i]->printGraphViz(target);
	}
}

// ----------------------------------------------------------------------------

void Node::printToPdf(std::string path) {

	std::string tmp = path;
	tmp.append(".dot");

	std::ofstream os(tmp.c_str());
	os << "digraph myTree {\n";
	printGraphViz(os);
	os << "}\n";
	os.close();
	tmp = "dot -Tpdf ";
	tmp.append(path);
	tmp.append(".dot");
	tmp.append(" -o ");
	tmp.append(path);
	tmp.append(".pdf");

	int result = system(tmp.c_str());
	if (result != 0) {
		throw Exception("ERROR in Node::printToPdf: failed to call dot\n");
	}

}

// ----------------------------------------------------------------------------

void Node::printCytoscape(std::ofstream &network, std::ofstream &nodes,
		std::ofstream &edgesPhyl, std::ofstream &edgesPhen) {

	// --------- network & edges files ---------

	for (size_t i = 0; i < this->children.size(); i++) {
		network << this->id << "\tpp\t" << this->children.at(i)->id << '\n';

		// phylogenetic distance
		edgesPhyl << this->id << " (pp) " << this->children.at(i)->id << "\t";
		edgesPhyl << this->children.at(i)->dist << '\n';

		///@todo compute phenotype distance here
		std::cerr
				<< "WARNING: when using Node::printCytoscape, implement phenoype distance!\n";
		float phenotypeDistance = 4711;

		edgesPhen << this->id << " (pp) " << this->children.at(i)->id << "\t";
		edgesPhen << phenotypeDistance << '\n';

	}

	// ------------ node file -------------------

	///@todo compute phenotype value here
	std::cerr
			<< "WARNING: when using Node::printCytoscape, implement phenoype value!\n";
	float phenotypeValue = 4712;
	nodes << this->id << '\t' << phenotypeValue << '\t';
	nodes << this->info << '\t';

	bool internal = this->children.size() != 0;
	nodes << internal;

	/// @todo maybe adding the genetic sequence as feature makes sense, too
	//	nodes << '\t';
	//	for (size_t i = 0; i < this->s->charFeatures.size(); i++) {
	//		nodes << char(this->s->charFeatures[i]);
	//	}
	nodes << '\n';

	for (size_t i = 0; i < this->children.size(); i++) {
		this->children.at(i)->printCytoscape(network, nodes, edgesPhyl,
				edgesPhen);
	}

}

void Node::print(TreePrintMode tpm, std::ostream &target, bool quotes) {

	switch (tpm) {
	case Newick:
		this->printNewick(target, quotes);
		break;
	case Cytoscape:
		/// @to do
		throw Exception("ERROR in Tree::print: not implemented!");
		break;
	case Pdf:
		throw Exception("ERROR in Tree::print: not implemented!");
		break;
	//Stefan Janssen (18.5.2015)
		case SimpleNewick:
			throw Exception("ERROR in Tree::print: not handled switch!");
			break;
		case Nexus:
			throw Exception("ERROR in Tree::print: not handled switch!");
			break;
	}
}

void Node::print(TreePrintMode tpm, map<int, std::string> featureNames,
		std::ostream &target, bool quotes) {

	switch (tpm) {
	case Newick:
		this->printNewick(target, featureNames, quotes);
		target << '\n';
		break;
	case Cytoscape:
		/// @to do
		throw Exception("ERROR in Tree::print: not implemented!");
		break;
	case Pdf:
		throw Exception("ERROR in Tree::print: not implemented!");
		break;
	//Stefan Janssen (18.5.2015)
		case SimpleNewick:
			throw Exception("ERROR in Tree::print: not handled switch!");
			break;
		case Nexus:
			throw Exception("ERROR in Tree::print: not handled switch!");
			break;
	}
}

void Node::print(TreePrintMode tpm, string path, bool quotes) {
	ofstream of(path.c_str());
	print(tpm, of, quotes);
	of.close();
}

#include <sstream>
void Node::draw(string path) {

	std::ostringstream os;

	os << "\"";
	this->print(Newick, os);
	os << "\"";

	string newickString = os.str();
	os.str("");

	os << "R --vanilla --quiet ";
	os << path << " ";
	os << " < " << RidgeRace::prefix << "/share/" << RidgeRace::progname << "/scripts/drawTree.r > tmp  2>&1";

	string call = os.str();

	int result = system(call.c_str());
	if (result != 0) {
		throw Exception("ERROR in  Node::draw: failed to call R\n");
	}

}

set<IdType> Node::getSetOfAllIdsBelow() {

	set<IdType> idsBelowCurrent;
	set<IdType> childrensIds;
	set<IdType>::iterator i;
	idsBelowCurrent.insert(this->id);

	for (size_t c = 0; c < this->children.size(); c++) {
		childrensIds = this->children.at(c)->getSetOfAllIdsBelow();
		for (i = childrensIds.begin(); i != childrensIds.end(); i++) {
			idsBelowCurrent.insert(*i);
		}
	}

	return idsBelowCurrent;

}

void Node::setInnerNames() {

	if (this->children.size()>0) {
		this->info = toString(this->id);
	}
	Node* child;
	for (size_t c = 0; c < this->children.size(); c++) {
		for (size_t c = 0; c < this->children.size(); c++) {
			child = this->children.at(c);
			child->setInnerNames();
		}
	}
}

void Node::annotateNodeHeight() {

	Node* child;
	// do this only if it has not been done before
	if (this->_height == -1) {
		this->_height = 0;

		// if this is a leaf node, nothing will happen
		// otherwise, the height of this node is 1 + the maximum height of its
		// child nodes
		for (size_t c = 0; c < this->children.size(); c++) {

			child = this->children.at(c);

			child->annotateNodeHeight();

			if (child->_height >= this->_height) {
				this->_height = child->_height + 1;
			}
		}
	}

}

set<IdType> Node::getSetOfAllIdsBelow(int minHeight) {

	if (this->_height == -1) {
		throw Exception(
				"ERROR in Node::getSetOfAllIdsBelow(int minHeight: you have to compute minHeight before calling this!\n");
	}

	set<IdType> idsBelowCurrent;
	set<IdType> childrensIds;
	set<IdType>::iterator i;

	if (this->_height >= minHeight) {
		idsBelowCurrent.insert(this->id);
	}

	for (size_t c = 0; c < this->children.size(); c++) {

		if (children.at(c)->_height >= minHeight) {

			childrensIds = this->children.at(c)->getSetOfAllIdsBelow();

			for (i = childrensIds.begin(); i != childrensIds.end(); i++) {
				idsBelowCurrent.insert(*i);
			}
		}
	}

	return idsBelowCurrent;

}


pair<size_t, size_t> Node::getNumberOfLeafsAndEdges() {

	pair<size_t, size_t> a, b;
	a.first = 1;	// number of nodes
	a.second = 0;	// number of edges

	for (size_t c = 0; c < this->children.size(); c++) {
		b = children.at(c)->getNumberOfLeafsAndEdges();
		a.first += b.first; // add nodes in child tree, this includes the child itself (ergo NOT + 1)
		a.second += b.second + 1; // add edges in child tree, plus edge to that tree
	}

	return a;
}


bool Node::isLeaf() {
	return this->children.size() == 0;
}
