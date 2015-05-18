/*
 * TreeExtractor.cpp
 *
 *  Created on: Nov 14, 2012
 *      Author: ctusche
 */

#include "RR_TreeExtractor.h"
#include "SetsAndMaps.h"
#include <sstream>

TreeExtractor::TreeExtractor(RR_Node* tree) {
	this->tree = tree;
}

// extract a mapping from every node in the tree to a name
void TreeExtractor::getIdToInfoMapping(RR_Node* curNode,
		map<IdType, string> &M) {

	// if this is an internal node that has no name, give him one
	if (curNode->info == "") {
		ostringstream os;
		os << "i" << curNode->id;
		curNode->info = os.str();
	}

	// store this nodes name
	M[curNode->id] = curNode->info;

	// go on with children
	for (size_t i = 0; i < curNode->children.size(); i++) {
		getIdToInfoMapping(curNode->children.at(i), M);
	}

}

// actual feature extraction
void TreeExtractor::printLeafFeatures(RR_Node* curNode,
		const std::set<IdType> nodesOfThisTree, FloatTreeAnnotation* pheno, std::map<IdType, float> M,
		std::ostream &target) {

	set<IdType>::iterator iter;
	IdType curFeature;

	// if this is a leaf node, then this node is identical to a sample.
	// for this sample, we will have to write down all the nodes that were
	// involved going down from the root to this node.
	// By using the recursion, we make sure these are exactly the nodes stored
	// in M at this point
	if (curNode->children.size() == 0) {

		if (!pheno->hasElement(curNode)) {
			Exception e;
			e
					<< "ERROR in TreeExtractor::printLeafFeatures: empty child node!\n";
			e << " child name was ";
			e << curNode->info;
			throw e;
		}

		// extract value at the simulation index
		// @ todo use param instead of zero
		target << pheno->getElement(curNode) << '\t';

		for (iter = nodesOfThisTree.begin(); iter != nodesOfThisTree.end();
				iter++) {

			curFeature = *iter;

			if (curFeature == curNode->id) {
				target << curNode->dist;
			} else if (contains(M, curFeature)) {
				target << M[curFeature];
			} else {
				target << "0";
			}
			if (curFeature != curNode->curID) {
				target << '\t';
			}
		}
		target << '\n';

		// if this is no leaf, we don't have to print out anything
	} else {
		// all we need to do is to update M for its children
		if (curNode->dist != 0) {
			M[curNode->id] = curNode->dist;
		}
		// and make the recursive call for them
		for (size_t i = 0; i < curNode->children.size(); i++) {
			printLeafFeatures(curNode->children.at(i), nodesOfThisTree, pheno, M,
					target);
		}
	}

}

set<IdType> getNodeIDsOfThisTree(RR_Node* curNode) {

	set<IdType> M, childSet;
	set<IdType>::iterator iter;
	if (curNode->children.size() == 0) {
		M.insert(curNode->id);
	} else {
		for (size_t c = 0; c < curNode->children.size(); c++) {
			childSet = getNodeIDsOfThisTree(curNode->children.at(c));
			for (iter = childSet.begin(); iter != childSet.end(); iter++) {
				M.insert(*iter);
			}
		}
		M.insert(curNode->id);
	}
	return M;
}

set<RR_Node*> getNodesOfThisTree(RR_Node* curNode) {

	set<RR_Node*> M, childSet;
	set<RR_Node*>::iterator iter;
	for (size_t c = 0; c < curNode->children.size(); c++) {
		childSet = getNodesOfThisTree(curNode->children.at(c));
		for (iter = childSet.begin(); iter != childSet.end(); iter++) {
			M.insert(*iter);
		}
	}
	M.insert(curNode);

	return M;
}

// write leaf features to a file
void TreeExtractor::printLeafFeatures(string basePath, FloatTreeAnnotation* pheno) {

	map<IdType, string> M;
	map<IdType, string>::iterator iter;

	// extract the names of the nodes (.e.g leaf/sample names, or
	// i4711 for internal nodes)
	getIdToInfoMapping(this->tree, M);

	set<IdType> nodesOfThisTree = getNodeIDsOfThisTree(this->tree);

	ofstream ofs(basePath.c_str());

	// write a header containing the feature names ----------------------------
	// @ todo catch null pointer exceptions
	ofs << "phenotype\t";
	string name;
	for (IdType i = 1; i < this->tree->curID; i++) {
		ofs << M[i];
		if (i != this->tree->curID) {
			ofs << '\t';
		}
	}
	ofs << '\n';

	// write the features for every leaf --------------------------------------
	std::map<IdType, float> K;

	// make sure distance above root is 1 (and also store that in K)
	// this->tree == root of the tree
	this->tree->dist = 1.0f;
	K[this->tree->id] = 1;
	this->printLeafFeatures(this->tree, nodesOfThisTree, pheno, K, ofs);
	ofs.close();
}

// write leaf features to any output stream
void TreeExtractor::printLeafFeatures(std::ostream &target, FloatTreeAnnotation* pheno) {

	std::map<IdType, float> M;
	set<IdType> nodesOfThisTree = getNodeIDsOfThisTree(this->tree);

	// start method on the root of the tree
	this->printLeafFeatures(this->tree, nodesOfThisTree, pheno, M, target);

}

LeafFeatureData TreeExtractor::getLeafFeatureData(FloatTreeAnnotation* pheno) {

	/// @todo using this->tree is horrible, use _tree instead
	// prepare the data structures to hold the information
	tree->getNumberOfLeafsAndEdges();

	LeafFeatureData lfd;

	// get temporary matrix to call getLeafFeatureData and
	// store the current position in the tree
	std::map<IdType, float> M;

	// necessary to have an ordering, to get the right number for entries in M
	set<RR_Node*> nodesOfThisTree = getNodesOfThisTree(this->tree);
	lfd.nodesOfThisTree = nodesOfThisTree;

	set<RR_Node*>::iterator iter;
	//cout << "Nodes of this tree:\n";
	//for (iter=nodesOfThisTree.begin(); iter!=nodesOfThisTree.end(); iter++) {
	//	cout << (*iter)->info << endl;
	//}

	// make sure distance above root is 1 (and also store that in M)
	// this->tree == root of the tree
	this->tree->dist = 1.0f;
	M[this->tree->id] = 1.0f;

	getLeafFeatureData(this->tree, M, pheno, lfd);		// call by reference for lfd!

	return lfd;

}

//
/* Actual feature extraction for RidgeRace regression.
 * Recursively run through the tree. Every leaf node corresponds to a sample.
 * Through recursive calling, M contains all nodes on the current path from
 * the root to the current node.
 * If the current node is a leaf:
 * 	Run through ALL inner nodes of this tree (stored as RR_Node* in lfd)
 * 		if one of them is on the current path, store its branch length
 * 		if not, store 0
 * 	Nodes on current path are stored as IDTypes in M
 * 	  @ todo: maybe move from IDType to RR_Node*?
 */
void TreeExtractor::getLeafFeatureData(RR_Node* curNode,
		std::map<IdType, float> M, FloatTreeAnnotation* pheno, LeafFeatureData &lfd) {

	set<RR_Node*>::iterator iter;	///< to run through all nodes
	RR_Node* curFeature;			///< helper variable for that

	// if this is a leaf node, then this node is identical to a sample.
	// for this sample, we will have to write down all the nodes that were
	// involved going down from the root to this node.
	// By using the recursion, we make sure these are exactly the nodes stored
	// in M at this point
	if (curNode->isLeaf()) {

		if (!pheno->hasElement(curNode)) {
			Exception e;
			e << "ERROR in TreeExtractor::getLeafFeatureData: child node without annotation!\n";
			e << " child name was ";
			e << curNode->info;
			e << " child id was ";
			e << curNode->id;
			throw e;
		}

		// extract value at the simulation index
		// i.e. the phenotype we're interested in
		// remember: this is a leaf!
		// @ todo use param (simulation index) instead of zero
		lfd.phenotypes->setElement(curNode, pheno->getElement(curNode));

		vector<float> branchLengths; ///< row representing all lengths of branches leading to this tree
		// equivalent to one sample in the ridge regression
		// we will store

		for (iter = lfd.nodesOfThisTree.begin();
				iter != lfd.nodesOfThisTree.end(); iter++) {

			curFeature = *iter;	// RR_Node* associated with the current branch / feature

			if (curFeature == curNode) {
				branchLengths.push_back(curNode->dist);

			} else if (contains(M, curFeature->id)) {
				branchLengths.push_back(M[curFeature->id]);
				//lfd.branchLengths(curRow, curCol) = M[curFeature];
			} else {
				//lfd.branchLengths(curRow,curCol) = 0;
				branchLengths.push_back(0);
			}

		}
		lfd.branchLengths->setElement(curNode, branchLengths);

		// if this is no leaf, we don't have to print out anything
	} else {

		// all we need to do is to update M for its children
		// @ todo actually, what does that mean? why check for == 0?
		//if (curNode->dist != 0) {
		M[curNode->id] = curNode->dist;
		//}
		// and make the recursive call for them
		for (size_t i = 0; i < curNode->children.size(); i++) {
			getLeafFeatureData(curNode->children.at(i), M, pheno, lfd); // call by reference for lfd!
		}
	}

}

void LeafFeatureData::print(std::ostream &out) {


	map<RR_Node*, vector<float> > branchLengthData = branchLengths->getFullData();

	map<RR_Node*, vector<float> >::iterator iter;
	out << "branch lengths:\n";
	for (iter = branchLengthData.begin(); iter != branchLengthData.end(); iter++) {
		vector<float> tmp = iter->second;
		for (size_t i = 0; i != tmp.size(); i++) {
			out << tmp.at(i) << "\t";
		}
		out << "\n";
	}
	out << "\n";

	out << "feature names (inner node names):\n";
	set<RR_Node*>::iterator iter2;
	for (iter2 = nodesOfThisTree.begin(); iter2 != nodesOfThisTree.end();
			iter2++) {
		out << (*iter2)->info << '\t';
	}

	out << "\n";

}
