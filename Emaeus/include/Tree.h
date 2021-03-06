/*
 * Tree.h
 *
 *  Created on: Nov 8, 2012
 *      Author: ctusche
 */

#ifndef xTREE_H_
#define xTREE_H_

#include <vector>
#include <string>
#include <ostream>
#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <map>
#include <set>

using namespace std;

enum TreePrintMode {
	Newick, Cytoscape, Pdf
};
typedef unsigned int IdType;

// ############################################################################

class Node {

private:

	void printGraphViz(std::ofstream &target);

	void getTreeFromPath(string path);

	void printNewick(std::ostream &target, bool quotes = true);
	void printNewick(std::ostream &target, map<int, std::string> featureNames, bool quotes = true);

	void printToPdf(std::string path);
	void printCytoscape(std::ofstream &network, std::ofstream &nodes,
			std::ofstream &edgesPhyl, std::ofstream &edgesPhen);

public:
	set<IdType> getSetOfAllIdsBelow();
	set<IdType> getSetOfAllIdsBelow(int minHeight);

	static IdType curID; ///< a counter to provide unique ids
	float dist;
	int _height;

	map<size_t, float> features;

	unsigned long id; ///< unique identifier
	std::string info;

	vector<Node*> children;
	Node* father;

	size_t size() {
		return children.size();
	}

	Node();
	~Node();
	Node(Node* a, Node* b);

	bool operator <(const Node& b) const {
		return this->id < b.id;
	}

	//Node(string path);

	void print(TreePrintMode tpm, std::ostream &target = std::cout,
			bool quotes = true);
	void print(TreePrintMode tpm, map<int, std::string> featureNames,
			std::ostream &target = std::cout, bool quotes = true);

	void print(TreePrintMode tpm, string path, bool quotes = true);

	void draw(string path);

	void annotateNodeHeight();
};

#endif /* TREE_H_ */

