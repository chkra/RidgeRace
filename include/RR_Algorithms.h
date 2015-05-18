/*
 * RR_Algorithms.h
 *
 *  Created on: Mar 22, 2013
 *      Author: ctusche
 */

#ifndef ACR_ALGORITHMS_H_
#define ACR_ALGORITHMS_H_

#include <iostream>
#include <cmath>
#include <cfloat>

#include "Parser.h"

#include "Result.h"
#include "RR_Tree.h"

using namespace std;

class RR_Algorithm {

protected:

public:
	RR_Algorithm() {};

	virtual ~RR_Algorithm() {};

	virtual Result* run(RR_Tree &tree) = 0;

	virtual string getDescription() = 0;
};

// ============================================================================

class ML_ACR: public RR_Algorithm {
private:
	string _mlTreeInputPath;
	string _mlDataInputPath;
	string _mlOutputPath;
	size_t _headID;
public:

	ML_ACR(size_t id) :
			RR_Algorithm() {
		ostringstream os;
		os << "trees/tree.newick" << '.' << id << ".tree";
		_mlTreeInputPath = os.str();
		os.str("");

		os << "trees/tree.newick" << '.' << id << ".dat";
		_mlDataInputPath = os.str();
		os.str("");

		os << "trees/tree.newick." << id << ".ml";
		_mlOutputPath = os.str();

		_headID = id;

	}

	ML_Result* run(RR_Tree &tree);

	~ML_ACR() {};

	string getDescription();
};

// ============================================================================

class GLS_ACR: public RR_Algorithm {
private:
	string _glsTreeInputPath;
	string _glsDataInputPath;
	string _glsOutputPath;
	string _someOtherString;
	size_t _headID; ///< ID of this RidgeRacer instance
	/// @to do: check if this belongs in super class

public:
	GLS_Result* run(RR_Tree &tree);

	string getDescription();

	GLS_ACR(size_t id) :
			RR_Algorithm() {
		ostringstream os;
		os << "trees/tree.newick" << '.' << id << ".tree";
		_glsTreeInputPath = os.str();
		os.str("");

		os << "trees/tree.newick" << '.' << id << ".dat";
		_glsDataInputPath = os.str();
		os.str("");

		os.str("");

		os << "trees/tree.newick." << id << ".gls";
		_glsOutputPath = os.str();

		_headID = id;

		os.str("");
		os << "trees/tree.newick." << id << ".gls";
		_someOtherString = os.str();

		cerr << _someOtherString << endl;
	}

	~GLS_ACR() {};
};

// ============================================================================

class Ridge_MultiLambda: public RR_Algorithm {

private:
	string _ridgeInputPath;
	string _ridgeOutputPath;

	size_t _headID; ///< ID of this RidgeRacer instance

public:

	Ridge_Result* run(RR_Tree &tree);

	Ridge_Result* runOldStyleRidge(RR_Tree &tree);

	string getDescription();

	/**
	 * Constructor - simple initialization, setting some paths.
	 * @see run
	 * @param id the process id of this RidgeRace instance
	 */
	Ridge_MultiLambda(size_t id) :
			RR_Algorithm() {
		ostringstream os;
		os << "trees/tree.ridge.branchStats" << '.' << id;
		_ridgeInputPath = os.str();

		os.str("");

		os << "trees/tree.ridge.dat" << '.' << id;
		_ridgeOutputPath = os.str();

		_headID = id;

	}

	~Ridge_MultiLambda() {};

};

class Ridge_Single: public RR_Algorithm {

private:
	string _ridgeInputPath;
	string _ridgeOutputPath;

	size_t _headID; ///< ID of this RidgeRacer instance

	float _biasTerm;
	float _lambda;
	bool _tryToCleanTree;

	float removeBiasTerm();

public:

	Ridge_Result* run(RR_Tree &tree);

	Ridge_Result* run_oldStyle(RR_Tree &tree);

	string getDescription();

	Ridge_Single(size_t id, bool tryToCleanTree, float lambda = 0.001) :
			RR_Algorithm() {
		ostringstream os;
		os << "trees/tree.ridge.branchStats" << '.' << id;
		_ridgeInputPath = os.str();

		os.str("");

		os << "trees/tree.ridge.dat" << '.' << id;
		_ridgeOutputPath = os.str();
		_lambda = lambda;
		_headID = id;
		_biasTerm = 0;

		_tryToCleanTree = tryToCleanTree;
	}

	~Ridge_Single() {};

	void setLambda(float lambda) {
		_lambda = lambda;
	}

};

class PhylogeneticMean: public RR_Algorithm {

private:

	size_t _headID; ///< ID of this RidgeRacer instance
	FloatTreeAnnotation* _values;

	float inferMeans(RR_Node* curNode, FloatTreeAnnotation* mean);

public:

	PhyloMean_Result* run(RR_Tree &tree);

	string getDescription();

	PhylogeneticMean(size_t id) :
			RR_Algorithm() {
		_values = new FloatTreeAnnotation("phenotype");
		_headID = id;
	}

	~PhylogeneticMean() {};

};

// ============================================================================

class TreePlot: public RR_Algorithm {

private:

	static string _outPath;
	static unsigned int _counter;

	pair<string, size_t> outputSingleTree(RR_Tree &tree);

public:

	/* @ todo this could be improved by
	 * creating a different path for every tree (and writing this path to
	 * the tree result
	 */
	Result* run(RR_Tree &tree);

	string getDescription();

	TreePlot(size_t id) :
			RR_Algorithm() {
		ostringstream os;
		os << "trees/evalTree" << '.' << id;
		_outPath = os.str();

	}

	~TreePlot() {};
};

// ==========================================================================

class Sankoff: public RR_Algorithm {

private:

	size_t _headID; ///< ID of this RidgeRacer instance
	DistanceMatrix _D;

	// @ todo shorten and simplify this method
	void computeSankoffCosts(RR_Node* curNode,
			SankoffTreeAnnotation* sankoffCostsOfNodes,
			StringTreeAnnotation* sequenceOfNodes) {

		// ---------- prepare stuff ------------------------------------

		// the distance matrix _D is a member of the Sankoff Algorithm class
		DistanceMatrix::iterator iter;
		vector<string> characterStates;
		string cState, fatherState, childState, seqChar;

		// get the key set of _D, i.e. get all known character states
		// @ todo it must be possible to do this better
		for (iter = _D.begin(); iter != _D.end(); iter++) {
			characterStates.push_back(iter->first);
		}

		if (characterStates.size() == 0) {
			throw Exception(
					"ERROR in Sankoff::reconstructSankoff: no character states found!\n");
		}

		// ---------- start actual Sankoff algo -------------------------

		// we have to create a new SankoffMap for this node;
		SankoffMap curNodeSM;

		// if this is a leaf, then we are in the trivial Sankoff case
		if (curNode->isLeaf()) {

			// get the sequence of this leaf node
			string sequence = sequenceOfNodes->getElement(curNode);

			// run through all positions of the sequence
			for (size_t pos = 0; pos < sequence.size(); pos++) {

				// get the character state currently at this sequence
				seqChar.clear();
				seqChar.push_back(sequence.at(pos));

				// run through all possible character states
				for (size_t charStateIndex = 0;
						charStateIndex < characterStates.size();
						charStateIndex++) {
					cState = characterStates.at(charStateIndex);

					// the sankoff entry (length = #character states) for a speciffic position should be
					// [inf, inf, inf, ..., inf, 0, inf, ..., inf]
					// only zero at the position where the sequence has the actual character state
					// in fact: not zero, but D[cState][cState]

					// therefore, set the Sankoff variable accordingly
					if (equal(cState, seqChar)) {
						curNodeSM[cState].push_back(_D[cState][cState]);
					} else {
						curNodeSM[cState].push_back(FLT_MAX);
					}
				}
			}

			// we finished the sankoffMap for this node - store it
			// (done at the end of the function)
		}
		// else, if this is no leaf, we are in the recursive Sankoff case
		else {

			if (curNode->children.size() == 1) {
				cerr
						<< "--- WARNING in Sankoff::reconstructSankoff: inner node ";
				cerr << curNode->id;
				cerr
						<< " has only one child - nothing bad will happen, but maybe check tree?";
				cerr << endl;

				RR_Node* onlyChild = curNode->children.at(0);

				computeSankoffCosts(onlyChild, sankoffCostsOfNodes,
						sequenceOfNodes);
				sequenceOfNodes->setElement(curNode,
						sequenceOfNodes->getElement(onlyChild));
				curNodeSM = sankoffCostsOfNodes->getElement(onlyChild);

			} else {

				// 1. make sure sankoff matrices are filled for all children,
				// we'll need them
				for (size_t c = 0; c < curNode->children.size(); c++) {
					computeSankoffCosts(curNode->children.at(c),
							sankoffCostsOfNodes, sequenceOfNodes);
				}

				// find out how long the sequence is
				string temporarySequence = sequenceOfNodes->getElement(
						curNode->children.at(0));
				size_t L = temporarySequence.size();

				// 2. fill own sankoff matrix

				// repeat the process for all sites, i.e.
				// run through all positions of the sequence
				for (size_t pos = 0; pos < L; pos++) {

					// run through all possible character states
					// compute their minimum costs
					for (size_t fatherStateIndex = 0;
							fatherStateIndex < characterStates.size();
							fatherStateIndex++) {

						// the current father state, we're going to compute the costs for it
						fatherState = characterStates.at(fatherStateIndex);
						float curCost = 0;

						// to do that, run through all children
						for (size_t childIndex = 0;
								childIndex < curNode->children.size();
								childIndex++) {

							float curCostForThisChild, minCostForThisChild =
							FLT_MAX; // minimum cost for transition to this child, given that
							// father state is fatherState

							// get the sankoff matrix for this particular child
							SankoffMap sm = sankoffCostsOfNodes->getElement(
									curNode->children.at(childIndex));

							// find the "cheapest" child state
							for (size_t childStateIndex = 0;
									childStateIndex < characterStates.size();
									childStateIndex++) {
								childState = characterStates.at(
										childStateIndex);

								// get the already computed costs for this child node
								// having this character state at this position in the sequence
								curCostForThisChild = sm[childState][pos];

								// also add transition costs
								curCostForThisChild +=
										_D[fatherState][childState];

								// if we improved the minimum, update
								if (curCostForThisChild < minCostForThisChild) {
									minCostForThisChild = curCostForThisChild;
								}
							}

							// we now now the cheapest costs for this
							// father state and this child
							curCost += minCostForThisChild;
						}

						// now that we've seen all children, we know the minimum cost
						// for this node at this positiion to have this state
						curNodeSM[fatherState].push_back(curCost);
					}
				}

				sequenceOfNodes->setElement(curNode, temporarySequence);
			}
		}

		sankoffCostsOfNodes->setElement(curNode, curNodeSM);
		//cout << "finished sankoff map for id " << curNode->id << endl;
		//sankoffCostsOfNodes->printElement(curNode);
		//cout << endl;

	}

	void setBestChildSequence(RR_Node* curNode, string curNodesBestSequence,
			SankoffTreeAnnotation* sankoffCostsOfNodes,
			StringTreeAnnotation* sequenceOfNodes) {

		// leafs have their sequence - do not mess with it
		if (curNode->isLeaf()) {
			return;
		}

		// else
		size_t L = curNodesBestSequence.size();

		float curCost, bestCost;
		string bestState;
		char bestStateAsChar;

		SankoffMap SM;
		SankoffMap::iterator characterStateIter;

		string inferredSequenceForNode;
		RR_Node* curChild;

		ostringstream os;
		string fathersState;

		for (size_t childIndex = 0; childIndex < curNode->children.size();
				childIndex++) {

			curChild = curNode->children.at(childIndex);

			// get the sankoff map for this child node
			SM = sankoffCostsOfNodes->getElement(curChild);

			// get an initialization for the sequence of this child node
			inferredSequenceForNode = sequenceOfNodes->getElement(curChild);

			// run through all the positions of the sequence
			for (size_t pos = 0; pos < L; pos++) {

				os << curNodesBestSequence.at(pos);
				fathersState = os.str();
				os.str("");

				// 1. get the best, i.e. the cheapest character for this position
				// 1a. initialize with something trivial
				// something trivial =
				// 	just set child state to fathers state
				bestCost = SM.at(fathersState).at(pos)
						+ _D[fathersState][fathersState];
				bestState = fathersState;

				// 1b. check all possible states for this child
				string childsCharacterState;
				vector<float> childsCostAtPos;

				for (characterStateIter = SM.begin();
						characterStateIter != SM.end(); characterStateIter++) {

					childsCharacterState = characterStateIter->first;
					childsCostAtPos = characterStateIter->second;

					// look up the Sankoff costs for this state at this position in this child
					// @ todo: we're not dealing with states of equal cost here - we simply take the first one
					curCost = childsCostAtPos.at(pos);
					// add transition costs
					curCost += _D[fathersState][childsCharacterState];

					//if (curChild->id == 2) {
					//	cerr << pos << '\t' <<  childsCharacterState  << '\t' << fathersState << "\tf" << bestCost << "\tc" << curCost << endl;
					//}

					/* note: bestCost is initialized with childs cost at fathers state plus transition
					 * if we write " if (curCost < bestCost) ", this is equivalent to
					 * DELTRAN, i.e.: if keeping the fathers state is possible, even if there is a solution
					 * of equivalent cost, then keep it
					 * BUT we want ACCTRAN, i.e.: if there is any other state of less OR EQUAL cost,
					 * then change to it
					 * NOTE: we add the !equal(...) part, because the fathersState is already
					 * the default best state due to the initialization. Its cost are of course
					 * less or equal the initial best cost (because they are equal, by definition).
					 * Since we do not want to overwrite an already made change to the best state,
					 * we only change the best state if it is "away" from the default
					 */
					//if (curCost <= bestCost && !equal(childsCharacterState, fathersState)) {
					if (curCost < bestCost) {
						bestCost = curCost;
						bestState = characterStateIter->first;
					}
				}
				// 2. assign to sequence of root
				bestStateAsChar = bestState.at(0);// @ todo this sucks - decide for the type of character state
				inferredSequenceForNode[pos] = bestStateAsChar;
			}

			// we found the optimal sequence for this node
			// 1. store it
			sequenceOfNodes->setElement(curChild, inferredSequenceForNode);
			cerr << curChild->id << inferredSequenceForNode << endl;
			// 2. correct its own children
			setBestChildSequence(curChild, inferredSequenceForNode,
					sankoffCostsOfNodes, sequenceOfNodes);

		}

	}

	/* @ todo: this is horrible:
	 * Distance matrices and Sankoff costs encodes character states as strings,
	 * while StringTreeAnnotation encodes them as characters (C++ chars)
	 */
	void reconstructSankoff(RR_Node* root,
			SankoffTreeAnnotation* sankoffCostsOfNodes,
			StringTreeAnnotation* sequenceOfNodes) {

		// we should have some (arbitrary) sequence assigned to the root already
		// get its size

		string inferredSequenceForNode = sequenceOfNodes->getElement(root);
		size_t L = inferredSequenceForNode.size();

		SankoffMap rootsSankoffMap = sankoffCostsOfNodes->getElement(root);
		SankoffMap::iterator characterStateIter;

		float curCost, bestCost;
		string bestState;
		char bestStateAsChar;

		// run through all positions of the sequence
		for (size_t pos = 0; pos < L; pos++) {
			// 1. get the best, i.e. the cheapest character for this position

			// 1a. initialize with something trivial
			bestCost = FLT_MAX;
			bestState = rootsSankoffMap.begin()->first;

			// 1b. check all possible states
			for (characterStateIter = rootsSankoffMap.begin();
					characterStateIter != rootsSankoffMap.end();
					characterStateIter++) {

				// look up the Sankoff costs for this state at this position
				// @ todo: we're not dealing with states of equal cost here - we simply take the first one
				curCost = characterStateIter->second.at(pos);

				if (curCost < bestCost) {
					bestCost = curCost;
					bestState = characterStateIter->first;
				}
			}
			// 2. assign to sequence of root
			bestStateAsChar = bestState.at(0);// @ todo this sucks - decide for the type of character state
			inferredSequenceForNode[pos] = bestStateAsChar;
		}

		sequenceOfNodes->setElement(root, inferredSequenceForNode);

		sankoffCostsOfNodes->print();

		// now that we have the best reconstruction for the root
		//for (size_t childIndex = 0; childIndex < root->children.size();
		//		childIndex++) {

		//	setBestChildSequence(root->children[childIndex],
		//			inferredSequenceForNode, sankoffCostsOfNodes,
		//			sequenceOfNodes);
		//}
		// father node, sequence of father, all sankoff costs, all settled sequences
		setBestChildSequence(root, inferredSequenceForNode, sankoffCostsOfNodes,
				sequenceOfNodes);

	}

	void inferMutations(RR_Node* curNode, StringTreeAnnotation* sequenceOfNodes,
			MutationTreeAnnotation* mutations) {

		string fatherSequence = sequenceOfNodes->getElement(curNode);
		string childSequence;
		RR_Node* curChild;

		vector<Mutation> m;

		for (size_t childIndex = 0; childIndex != curNode->children.size();
				childIndex++) {
			curChild = curNode->children.at(childIndex);
			childSequence = sequenceOfNodes->getElement(curChild);

			if (childSequence.size() != fatherSequence.size()) {
				throw Exception(
						"ERROR in Sankoff::inferMutations: father sequence and child sequence differ in length!\n");
			}

			for (size_t i = 0; i < childSequence.size(); i++) {
				if (childSequence.at(i) != fatherSequence.at(i)) {
					Mutation mut(toString(fatherSequence.at(i)),
							toString(childSequence.at(i)), i + 1);
					m.push_back(mut);
				}
			}

			mutations->setElement(curChild, m);
			m.clear();

			inferMutations(curChild, sequenceOfNodes, mutations);

		}

	}

public:

	Empty_Result* run(RR_Tree &tree) {

		// @ todo: check that this is truly prepared by RideInferer
		StringTreeAnnotation* sta = (StringTreeAnnotation*) tree.getAnnotation(
				"sequence");
		//sta->print();

		// @ todo: check that this is truly prepared by RideInferer
		//printDistanceMatrix(_D);

		RR_Node* root = tree.getRoot();
		SankoffTreeAnnotation* sankoff = new SankoffTreeAnnotation(
				"SankoffMatrix");

		MutationTreeAnnotation* mutations = new MutationTreeAnnotation(
				"mutations");

		computeSankoffCosts(root, sankoff, sta);
		reconstructSankoff(root, sankoff, sta);
		inferMutations(root, sta, mutations);

		tree.addAnnotation("sankoff", sankoff);
		tree.addAnnotation("sequence", sta);
		tree.addAnnotation("mutations", mutations);

		Empty_Result* e = new Empty_Result();
		return e;
	}

	string getDescription() {
		return "Sankoff";
	}

	Sankoff(size_t id, string pathToDistanceMatrix) :
			RR_Algorithm() {

		DataParser dp(pathToDistanceMatrix);
		_D = dp.readDistanceMatrix();

		_headID = id;
	}

	~Sankoff() {};

};

#endif /* RR_ALGORITHMS_H_ */
