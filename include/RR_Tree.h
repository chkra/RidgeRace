/*
 * Tree.h
 *
 *  Created on: Mar 26, 2013
 *      Author: ctusche
 */

#ifndef RRR_TREE_H_
#define RRR_TREE_H_

#include <cstdlib>
#include <cstdio>

using namespace std;
#include <map>
#include <vector>
#include <sstream>

#include "Node.h"

#include "RR_Node.h"
#include "RR_TreeAnnotator.h"
#include "RR_TreeEvaluator.h"
#include "RR_TreeExtractor.h"
#include "Simulator.h"
#include "RidgeSolver.h"

#include "StringManipulation.h"
#include "Externals.h"
#include "SetsAndMaps.h"

#include "RR_TreeAnnotation.h"


class RR_Tree {

private:

	RR_Node* rrroot;

	Simulator rrS;
	RegimeSpecification rrrs;

	bool rrsimulatorIsSet;

	size_t rrsize;

	//map<int, string> _indexNames;

	map<RidgeRaceIndex, float> rrsummary;

	//map<string, TreeAnnotation* > _annotations;

	void printNewick(RR_Node* curNode, vector<string> relAnnotations, int minSupport,  ostream &target );

	void printSimpleNewick(RR_Node* curNode, ostream &target);

	string getAnnotationStringForNode(RR_Node* node, vector<string> relAnnotations, int minSupport);


	int annotateNodeSupport(RR_Node* curNode);

	void printSelector(TreePrintMode tpm, vector<string> relAnnotations,
			int minSupport, ostream &target);



public:

	map<string, TreeAnnotation* > rrannotations;

	RR_Tree();

	RR_Tree(RR_Node* root);

	RR_Tree(string path);


	void print(TreePrintMode tpm, int minSupport=-1, ostream &target = std::cout);
	void print(TreePrintMode tpm, vector<string> relAnnotations, int minSupport=-1, ostream &target=std::cout);


	RR_Node* getRoot();

	void setSummary(RidgeRaceIndex rri, string value);

	bool hasInterestingSummary();

	float correlate(string firstIndex, string secondIndex,
			bool includeLeafs = false);

	float MSE(string firstIndex, string secondIndex,
			bool includeLeafs);

	float MPE(string firstIndex, string secondIndex);

	void clear();

	void setRandom(size_t size, string path);

	void printLeafFeatures(string path, RidgeRaceIndex rs);

	LeafFeatureData getLeafFeatureData(RidgeRaceIndex rs);

	void setSimulator(RegimeSpecification rs);

	void setBiasedSimulator(RegimeSpecification rs);

	RegimeSpecification getRegimeSpecification();

	void simulateBM();

	void annotateNodeHeight();

	void annotateNodeSupport();

	size_t getSize();

	bool hasAnnotation(string key);
	void addAnnotation(string key, TreeAnnotation* anno, bool deleteOld=false);

	/* WARNING: this will NOT delete the actual content
	 * of the annotation (will not delete the pointer!
	 * you have to take care of that yourself!
	 */
	void dropAnnotation(string key);

	TreeAnnotation* getAnnotation(string key);
};



#endif /* RR_TREE_H_ */
