/*
 * Tree.h
 *
 *  Created on: Nov 8, 2012
 *      Author: ctusche
 */

#ifndef xTREE_H_
#define xTREE_H_
#include <cstdlib>
#include <cstdio>

using namespace std;

#include <vector>
#include <string>
#include <ostream>
#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <map>
#include <set>


enum TreePrintMode {
	Newick, SimpleNewick, Nexus, Cytoscape, Pdf
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
	set<std::string> getTipNames(); // a function to traverse the tree and push all node names in a set, which are leaves, i.e. have no further children

	static IdType curID; ///< a counter to provide unique ids
	float dist;
	int _height;

	//map<size_t, float> features;

	IdType id; ///< unique identifier
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

	void setInnerNames();

	bool isLeaf();

	/**
	 * Counts the leaf nodes and all edges in the tree
	 */
	pair<size_t, size_t> getNumberOfLeafsAndEdges();
};

double round_nplaces(double value, int to);

#endif /* TREE_H_ */

