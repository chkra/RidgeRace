/*
 * RidgeRacer.h
 *
 *  Created on: Mar 22, 2013
 *      Author: ctusche
 */

#ifndef RIDGERACER_H_
#define RIDGERACER_H_

#include <iostream>
#include <cstring>
#include <vector>

#include <time.h>
#include "Files.h"
#include "Result.h"
#include "Externals.h"
#include "RidgeRaceInput.h"
#include "RR_Tree.h"
#include "Simulator.h"
#include "RR_Algorithms.h"
#include "prefix.h"

using namespace std;

/**
 * The base instance of the RidgeRace program. Reads input configuration, does nothing.
 *
 * Allows to run init(), which will define an id, a time, and
 * a few file names. Does NOT create any trees or data.
 * Instantiating this class allows to add RR_Algorithms that perform
 * some analysis on the input data.
 */
class RidgeRacer {

protected:

	size_t _id;

	RidgeRaceInput _rri;
	//static string _basePath;
	static string _outputPath;
	RR_Tree _baseTree;

	string _curLogPath;
	string _curTmpPath;

	vector<RR_Algorithm*> _algos;
	vector<Result*> _results;

	map<RidgeRaceIndex, string> getIndexNameMapping();

	virtual void addAlgorithms() = 0;

	void init() {

		_id = getId();
		//

		Caller C;

		std::ostringstream os;
		os << _outputPath << '_' << _id << '_' << C.getTimestamp() << ".log";
		_curLogPath = os.str();

		os.str("");

		os << _outputPath << '_' << _id << '_' << C.getTimestamp()
				<< ".tmp.log";
		_curTmpPath = os.str();

		os.str("");

		os << RidgeRace::treeDir.str() << "/simulatedTree.newick" << '.' << _id;
		_rri.treeSimFile = os.str();
	}

	size_t getId() {

		time_t timer;
		struct tm y2012;
		long seconds;

		y2012.tm_hour = 0;
		y2012.tm_min = 0;
		y2012.tm_sec = 0;
		y2012.tm_year = 112;
		y2012.tm_mon = 0;
		y2012.tm_mday = 1;

		time(&timer);

		seconds = difftime(timer, mktime(&y2012));
		return seconds % 10000;
	}

public:

	RidgeRacer(RidgeRaceInput rri) {
		_rri = rri;
		_outputPath = rri.testCorrelationOutputFile;
		init();
	}

	RidgeRacer() {
		_outputPath = "RR_defaultOutput";
		init();
	}

	///Prints all results
	void print();

	/// Virtual method to run this particular RidgeRace Analysis.
	virtual void run() = 0;

	void printTree();

	virtual void evaluate() = 0;

	virtual ~RidgeRacer() {
		while (_algos.size()>0) {
				delete _algos.back();
				_algos.pop_back();
		}
	};

};

// ============================================================================

/**
 * The RidgeRace test module.
 *
 * Starts a RidgeRacerr, calls addAlgorithms to add
 * Ridge, ML and GLS reconstructions as well as tree plotting,
 * creates a random tree with 10 nodes, simulates default Brownian Motion,
 * runs the algorithms on the tree (including plotting),
 * does not perform any actual evaluation, but checks if
 * the evaluation script can be found.
 *
 */
class RR_Check: public RidgeRacer {

protected:

	void addAlgorithms() {
		_algos.push_back(new Ridge_MultiLambda(_id));
		_algos.push_back(new ML_ACR(_id));
		_algos.push_back(new GLS_ACR(_id));
		_algos.push_back(new TreePlot(_id));
	}
public:

	RR_Check(RidgeRaceInput rri) :
			RidgeRacer(rri) {

		cerr << "--------------------------------\n";
		cerr << " WARNING: RUNNING TESTMODE ONLY\n";
		cerr << "--------------------------------\n";

		cerr << "ID of this instance of RR is " << _id << "\n\n";
		addAlgorithms();
	}

	~RR_Check() {
		cerr << "--------------------------------------------\n";
		cerr << "Finished RR_Check, clearing base tree" << endl;
		_baseTree.clear();
	}
	;

	void evaluate() {
		Caller C;

		std::stringstream scriptFilename;
		scriptFilename << RidgeRace::prefix << "/share/" << RidgeRace::progname << "/scripts/plotTestRateCorrelationOutput.r";
		if (C.fileExists(scriptFilename.str())) {
			cerr << "GOOD:  found the evaluation plotting script\n";
		}

	}
	;

	/** Checks most of the RidgeRace functions.
	 * Calls a few ACR_Algorithms and prints the corresponding Results.
	 */
	void run();
};

// ============================================================================

class RR_Correlater: public RidgeRacer {

protected:

	void addAlgorithms() {
		_algos.push_back(new ML_ACR(_id));
		_algos.push_back(new GLS_ACR(_id));
		//_algos.push_back(new TreePlot(_id));
	}

private:

	void testRateCorrelation(RegimeSpecification rs);

public:

	RR_Correlater(RidgeRaceInput rri) :
			RidgeRacer(rri) {
		cerr << "ID of this instance of RR is " << _id << "\n\n";
		addAlgorithms();
	}

	~RR_Correlater() {
		cerr << "--------------------------------------------\n";
		cerr << "Finished RR_Correlater, clearing base tree" << endl;
		_baseTree.clear();
	}

	void evaluate();

	void run();
};

// ============================================================================

class RR_Example: public RidgeRacer {

protected:

	void addAlgorithms() {
		_algos.push_back(new Ridge_MultiLambda(_id));
		_algos.push_back(new TreePlot(_id));
	}

private:

	void testRateCorrelation(RegimeSpecification rs);

public:

	RR_Example(RidgeRaceInput rri) :
			RidgeRacer(rri) {
		cerr << "ID of this instance of RR is " << _id << "\n\n";
		addAlgorithms();
	}

	~RR_Example() {
		cerr << "--------------------------------------------\n";
		cerr << "Finished RR_Example, clearing base tree" << endl;
		_baseTree.clear();
	}

	void evaluate();

	void run();
};

class RR_Performer: public RidgeRacer {

protected:

	void addAlgorithms() {
		_algos.push_back(new Ridge_Single(_id, false));
	}
public:

	RR_Performer(RidgeRaceInput rri) :
			RidgeRacer(rri) {
		cerr << "ID of this instance of RR is " << _id << "\n\n";
		addAlgorithms();
	}

	~RR_Performer() {
		cerr << "--------------------------------------------\n";
		cerr << "Finished RR_Performer, clearing base tree" << endl;
		_baseTree.clear();
	}

	void evaluate();

	void run();
};

/**
 * The RidgeRace parameter search helper.
 *
 */
class RR_LambdaSearch: public RidgeRacer {

protected:

	typedef pair<RR_Node*, RR_Node*> BrotherNodes;
	typedef vector<BrotherNodes> ListOfBrotherNodes;
	float _bestLambda;

	void init() {

		// read in tree
		_baseTree = RR_Tree(_rri.inTree);

		/* add values to leaf nodes.
		 @todo think about that:
		 we add it to the simulation index here basically because
		 the "correct" (gold standard) values are stored in the
		 simulation index by default*/
		FloatTreeAnnotation* simulation = new FloatTreeAnnotation(
				_rri.phenoPath, _baseTree.getRoot(),
				Constants::simulationIndex);
		_baseTree.addAnnotation(Constants::simulationIndex, simulation);
	}

	void getAllBrotherNodePairs(ListOfBrotherNodes &lbn, RR_Node* curNode) {

		// if curNode is leaf, the nothing to do here
		if (curNode->isLeaf())
			return;

		// if not leaf, it should have at least two children
		// still, only go on if this is not the root,
		// since the root has no father
		if (curNode != _baseTree.getRoot()) {
			bool relNode = true;
			for (size_t c = 0; c < curNode->children.size(); c++) {
				relNode = relNode & curNode->children.at(c)->isLeaf();
			}
			if (relNode & (curNode->children.size() > 1)) {
				pair<RR_Node*, RR_Node*> p;
				p.first = curNode->children.at(0);
				p.second = curNode->children.at(1);
				lbn.push_back(p);
			}
		}

		for (size_t c = 0; c < curNode->children.size(); c++) {
			getAllBrotherNodePairs(lbn, curNode->children.at(c));
		}

	}

	void addAlgorithms() {
	}

public:

	RR_LambdaSearch(RidgeRaceInput rri) :
			RidgeRacer(rri) {

		init();

		//cerr << "---> read tree of size " << _baseTree.getSize() << endl;
		_bestLambda = -1;
	}

	RR_LambdaSearch(RR_Tree baseTree) :
			RidgeRacer() {

		_baseTree = baseTree;
		_bestLambda = -1;
	}

	void clear() {
		cerr << "WARNING: calling RR_LambdaSearch::clear" << endl;
		_baseTree.clear();
	}

	~RR_LambdaSearch() {
		/* NOT REMOVING _baseTree here ... this is the only RidgeRacer that might
		 * actually  be called within another RidgeRacer, and clearing the tree
		 * might remove the nodes of the owner instance
		 */
		/*
		 * also not removing simulation annotation here, parent class might still need this
		 */

		/*
		if (_baseTree.hasAnnotation(Constants::simulationIndex)) {
			FloatTreeAnnotation* simulation =
					(FloatTreeAnnotation*) _baseTree.getAnnotation(
							Constants::simulationIndex);
			_baseTree.dropAnnotation(Constants::simulationIndex);
			delete simulation;
		}
		 */
		if (_baseTree.hasAnnotation(Constants::ridgeIndex)) {
			FloatTreeAnnotation* ridge =
					(FloatTreeAnnotation*) _baseTree.getAnnotation(
							Constants::ridgeIndex);
			_baseTree.dropAnnotation(Constants::ridgeIndex);
			delete ridge;
		}
		if (_baseTree.hasAnnotation(Constants::ridgeWeightIndex)) {
			FloatTreeAnnotation* ridgeW =
					(FloatTreeAnnotation*) _baseTree.getAnnotation(
							Constants::ridgeWeightIndex);
			_baseTree.dropAnnotation(Constants::ridgeWeightIndex);
			delete ridgeW;
		}

	}

	void evaluate() {
		//cerr << "Write out here the best value for lambda\n";
	}

	ListOfBrotherNodes getAllBrotherNodePairs() {
		ListOfBrotherNodes lbn;
		getAllBrotherNodePairs(lbn, _baseTree.getRoot());
		return lbn;
	}

	void repairTree(BrotherNodes b, float oldDistOfBroth1) {

		RR_Node* brother1 = b.first;
		RR_Node* brother2 = b.second;

		/* the original father is still stored in
		 * brother2, but both are not connected to
		 * the tree
		 */
		RR_Node* father = brother2->father;
		RR_Node* grandfather = brother1->father;

		if (father->father != grandfather) {
			throw Exception(
					"ERROR in RR_LambdaSearch::repairTree: illegal grandfather!");
		}

		// ------------------------------------------------

		// set father back in
		bool foundNode = false;
		for (size_t c = 0; c < grandfather->children.size(); c++) {
			if (grandfather->children.at(c) == brother1) {
				grandfather->children.at(c) = father;
				foundNode = true;
			}
		}
		if (!foundNode) {
			throw Exception(
					"ERROR in RR_LambdaSearch::repairTree: illegal grandfather setting!");
		}

		// father->children is still set correct
		// brother2 is also fine

		// repair brother1
		brother1->father = father;
		brother1->dist = oldDistOfBroth1;
	}

	float removeEntryFromTree(BrotherNodes b) {

		RR_Node* brother1 = b.first;
		RR_Node* brother2 = b.second;
		RR_Node* father = brother1->father;
		if (father != brother2->father) {
			throw Exception(
					"ERROR in RR_LambdaSearch::removeEntryFromTree: nodes should have identical father");
		}
		RR_Node* grandfather = father->father;

		// --------------------------------------

		float fatherBranchLength = father->dist;
		float brotherBranchLength = brother1->dist;

		// --------------------------------------

		// adopt brother1 by grandfather
		brother1->father = grandfather;
		// remove father node from lineage, kick out brother2
		bool foundFather = false;
		for (size_t c = 0; c < grandfather->children.size(); c++) {
			if (grandfather->children.at(c) == father) {
				grandfather->children.at(c) = brother1;
				foundFather = true;
			}
		}
		if (!foundFather) {
			throw Exception(
					"ERROR in RR_LambdaSearch::removeEntryFromTree: illegal grandfather setting!");
		}

		// --------------------------------------

		brother1->dist += fatherBranchLength;

		return brotherBranchLength;
	}

	float getLooError(RR_Node* node) {

		FloatTreeAnnotation* ridgeAnno =
				(FloatTreeAnnotation*) _baseTree.getAnnotation(
						Constants::ridgeIndex);
		FloatTreeAnnotation* simAnno =
				(FloatTreeAnnotation*) _baseTree.getAnnotation(
						Constants::simulationIndex);

		float inferred = 0;
		float truth = 0;
		try {
			inferred = ridgeAnno->getElement(node);
			truth = simAnno->getElement(node);
		} catch (Exception e) {
			ostringstream os;
			os
					<< "ERROR in RR_LambdaSearch::getLooError: not able to access node with id ";
			os << node->id;
			os << "Original exception text: " << e.msg() << endl;
			os << "About the node:\n";
			os << "   leaf: " << node->isLeaf() << "\n";
			os << "   in inference: " << ridgeAnno->hasElement(node) << "\n";
			os << "   in simulation: " << simAnno->hasElement(node) << "\n";
			throw Exception(os.str());
		}

		return pow((inferred - truth), 2.0f);
	}

	float evaluateLambda(float lambda, Ridge_Single* ridgeRaceInference,
			ListOfBrotherNodes lbn) {
		ridgeRaceInference->setLambda(lambda);
		float singleError, looError = 0;

		// prepare candidates for leave-one-out (LOO)
		float backupBranchLength;
		Ridge_Result* res;
		string input;

		for (size_t p = 0; p < lbn.size() && p < 25; p++) {

			// prepare LOO sample
			backupBranchLength = removeEntryFromTree(lbn.at(p));
			//cerr << "  ---> removed entry" << endl;

			// run algo for LOO sample
			res = ridgeRaceInference->run(_baseTree);
			delete res;
			//cerr << "  ---> performed RR inference" << endl;


			// compute error
			singleError = getLooError(lbn.at(p).first);

			looError += singleError;
			//cerr << "  ---> error:\t" << singleError << endl;

			// reconstruct original data
			repairTree(lbn.at(p), backupBranchLength);
			//cerr << "  ---> repaired tree" << endl;


		}
		return sqrt(looError);
	}

	float getBestLambda() {
		return _bestLambda;
	}

	void run() {

		// prepare stuff
		Ridge_Single* ridgeRaceInference = new Ridge_Single(this->_id, true);
		ListOfBrotherNodes lbn = getAllBrotherNodePairs();

		if (lbn.size() == 0) {
			throw Exception(
					"ERROR in RR_LambdaSearch::run: tree is to small for lambda search!\n");
		}
		//cerr << "---> found " << lbn.size() << " samples for LOO inference"
		//		<< endl;

		//cout << "exponent\tlambda\terror\n";

		float lambda;
		float error;
		float smallestError = FLT_MAX;
		for (int exp = -2; exp < 4; exp++) {
			lambda = pow(10, exp);
			//cerr << "   --->  testing lambda = " << lambda << endl;
			error = evaluateLambda(lambda, ridgeRaceInference, lbn);
			if (error < smallestError) {
				smallestError = error;
				_bestLambda = lambda;
			}
			//cout << exp << "\t" << lambda << "\t" << error << endl;
		}

		delete ridgeRaceInference;

	}
};

typedef map<Mutation, vector<float> > MutationWeightMap;
typedef map<size_t, vector<float> > PositionWeightMap;

class RR_Inference: public RidgeRacer {

protected:

	void addAlgorithms() {

		_algos.push_back(new Sankoff(_id, _rri.distanceMatrixPath));

		RR_LambdaSearch* lambdaSearch = new RR_LambdaSearch(_rri);
		lambdaSearch->run();

		Ridge_Single* ridge = new Ridge_Single(_id,
				lambdaSearch->getBestLambda());
		_algos.push_back(ridge);
		lambdaSearch->clear();
		delete lambdaSearch;
	}

	void printMutationStatsPerNode(RR_Node* curNode,
			MutationTreeAnnotation* mutations, FloatTreeAnnotation* simulation,
			FloatTreeAnnotation* ridge, FloatTreeAnnotation* ridgeWeight);

	void getMutationStatsPerMutation(RR_Node* curNode,
			MutationTreeAnnotation* mutations, FloatTreeAnnotation* ridgeWeight,
			MutationWeightMap &mwm);

	void getMutationStatsPerPosition(RR_Node* curNode,
			MutationTreeAnnotation* mutations, FloatTreeAnnotation* ridgeWeight,
			PositionWeightMap &pwm);

	void printPositionWeightMap(PositionWeightMap pwm, Table posNames);
	void printMutationWeightMap(MutationWeightMap mwm);

public:

	RR_Inference(RidgeRaceInput rri) :
			RidgeRacer(rri) {
		cerr << "ID of this instance of RR is " << _id << "\n\n";
		addAlgorithms();
	}

	~RR_Inference() {
		cerr << "--------------------------------------------\n";
		cerr << "Finished RR_Inference, clearing base tree" << endl;
		_baseTree.clear();
	}

	void evaluate();

	void run();
};

#endif /* RIDGERACER_H_ */
