/*
 * RidgeRacer.cpp
 *
 *  Created on: Mar 22, 2013
 *      Author: ctusche
 */

#include "RidgeRacer.h"
#include "RR_TreeAnnotation.h"
#include "StringManipulation.h"

// ############################################################################

string RidgeRacer::_basePath = "./trees/tree";
string RidgeRacer::_outputPath = "log/default";

void RidgeRacer::print() {

	ofstream of(_curLogPath.c_str());

	if (!of.good()) {
		cerr << "\n\nERROR in RidgeRacer::print(): unable to open log file!";
		cerr << "\n\tpath was:\n\t" << _curLogPath << endl;
		throw Exception("File not found");
	}

	size_t trueResults = 0;
	for (size_t i = 0; i < _results.size(); i++) {
		if (equal(_results.at(i)->getType(), "empty")) {
			continue;
		}
		of << _results.at(i)->getType();
		_results.at(i)->print(of);
		trueResults++;
	}
	of.close();

	if (trueResults != 0) {
		cerr << "Wrote " << trueResults << " results to " << _curLogPath
				<< "\n";
	}
}

// ----------------------------------------------------------------------------

void RidgeRacer::printTree() {
	_baseTree.print(Newick, -1, std::cout);
}

// ############################################################################

void RR_Check::run() {

	RegimeSpecification rs(3, _rri.grandMean, _rri.startStd, _rri.stdMode);
	cerr << "Set Regime specifications" << endl;

	// create a random tree;
	_baseTree.setRandom(50, _rri.treeSimFile);
	cerr << "Created random tree with 50 nodes" << endl;

	// simulate Brownian Motion style evolution
	_baseTree.setSimulator(rs);
	cerr << "Initialized Simulator" << endl;

	_baseTree.simulateBM();
	cerr << "Simulated Brownian motion" << endl;

	Result* res;
	cerr << "Running ... " << endl;
	for (size_t i = 0; i < _algos.size(); i++) {
		cerr << "\n\n----" << _algos.at(i)->getDescription() << "----" << "\n"
				<< endl;
		res = _algos.at(i)->run(_baseTree);
		cerr << endl << "finished " << _algos.at(i)->getDescription() << endl;
		res->print("functionCheck.dat");
	}

	_baseTree.clear();

	cerr << "Finished RR_Check.\n";
}

// ############################################################################

void RR_Correlater::evaluate() {

	Caller C;
	vector<string> params;
	params.push_back(_curLogPath);

	cerr << "Running scripts/plotTestRateCorrelationOutput.r\n";
	C.callR("scripts/plotTestRateCorrelationOutput.r", params);
}

void RR_Correlater::run() {

	RegimeSpecification rs;
	rs._grandMean = _rri.grandMean;

	if (equal(_rri.stdMode, "drawStd")) {
		rs._mode = drawStd;
	} else if (equal(_rri.stdMode, "constStd")) {
		rs._mode = constStd;
	} else {
		cerr << "mode was " << _rri.stdMode << endl;
		throw Exception("ERROR in RR_Correlater::run(): invalid BaseStdMode\n");
	}
	if (rs._mode == constStd && _rri.maxNrOfRegimes != 1) {
		throw Exception(
				"ERROR in RR_Correlater::run(): number of regimes must equal one when using constStd!\n");
	}

	// iterate over random trees of different sizes
	for (unsigned int treeSize = _rri.startTreeSize;
			treeSize <= _rri.stopTreeSize; treeSize += _rri.stepTreeSize) {

		// make sure that we have steps like 10, 50, 100, 150 ...
		if (treeSize == _rri.startTreeSize + _rri.stepTreeSize) {
			treeSize -= _rri.startTreeSize;
		}

		cerr << "   tree size (" << treeSize << "/" << _rri.stopTreeSize
				<< ")\n";

		// create a random tree;
		_baseTree.setRandom(treeSize, _rri.treeSimFile);

		// use the random tree several times
		for (unsigned int treeRepeats = 0; treeRepeats < _rri.repeatTreeSim;
				treeRepeats++) {

			// run through different degrees of base std
			for (float bstd = _rri.startStd; bstd <= _rri.endStd;
					bstd += _rri.stepStd) {

				rs._baseStd = bstd;

				// run through different numbers of regimes
				for (size_t i = 1; i <= _rri.maxNrOfRegimes;
						i += _rri.nrOfRegimesStepSize) {

					rs._nrOfRegimes = i;
					testRateCorrelation(rs);
				}
			}
		}

		_baseTree.clear();
		cerr << "\n";
	}
	cerr << "Finished evaluation.\n";
}

void RR_Correlater::testRateCorrelation(RegimeSpecification rs) {

	size_t ts = _baseTree.getSize();

	cerr << "      updating " << this->_curTmpPath << endl;

	ofstream of;
	of.open(this->_curTmpPath.c_str(), std::ios::app);

	/*
	 * We want to make sure that regimes appear in higher nodes, not close
	 * to the leafs. In a tree with N leafs (height log N), there are
	 * >= N/4 nodes "older" than the "grandparents" of leafs, i.e. there
	 * are >= N/4 nodes suitable to be heads of regimes (including the root).
	 * To allow a certain flexibility, we demand that <= N/5 nodes are
	 * chosen from these N/4 available ones.
	 */

	if (0.2 * float(ts) <= float(rs._nrOfRegimes)) {
		cerr << "Tree size of " << ts << " is to small for " << rs._nrOfRegimes
				<< " regimes.";
		cerr << " Skipping this turn.\n";
		return;
	}

	Result* r;
	float lambda;
	Ridge_Single* rSingle = new Ridge_Single(_id, false);
	time_t begin, end;

	// redefine regime setup
	for (size_t roundsSim = 0; roundsSim < _rri.roundsOfSimulation;
			roundsSim++) {

		cerr << "       sim round " << (roundsSim + 1) << '/'
				<< _rri.roundsOfSimulation << endl;
		// set new regimes according to rs
		_baseTree.setSimulator(rs);

		for (size_t roundsEval = 0; roundsEval < _rri.roundsOfEvolution;
				roundsEval++) {

			cerr << "        eval round " << (roundsEval + 1) << '/'
					<< _rri.roundsOfEvolution << endl;

			// create phenotypes, store in simulation index
			_baseTree.simulateBM();

			cerr << "         lambda";

			time(&begin);

			// run RidgeRace with best lambda
			RR_LambdaSearch* lambdaSearch = new RR_LambdaSearch(_baseTree);
			lambdaSearch->run();
			lambda = lambdaSearch->getBestLambda();
			// delete will NOT remove _baseTree
			delete lambdaSearch;
			time(&end);
			cerr << "  (" << float(difftime(end, begin)) << " s)" << endl;

			time(&begin);
			cerr << "         ridge";

			rSingle->setLambda(lambda);
			r = rSingle->run(_baseTree);
			_results.push_back(r);
			r->print(of);
			time(&end);
			cerr << "  (" << float(difftime(end, begin)) << " s)" <<endl;

			// run remaining algorithms
			for (size_t a = 0; a < _algos.size(); a++) {
				/* careful: RR_Algorithm::run(RR_Tree &tree)
				 * takes the tree as a reference, i.e. it changes it!
				 * Therefore it makes sense to call the tree plot
				 * algorithm in the end!
				 */
				cerr << "         " << _algos.at(a)->getDescription();
				time(&begin);

				r = _algos.at(a)->run(_baseTree);

				// store the result in this instance of class RidgeRacer,
				// so that it is written to a logfile in the end
				_results.push_back(r);

				// but also write it in the temporary file
				r->print(of);
				time(&end);
				cerr << "  (" << float(difftime(end, begin)) << " s)" << endl;
			}
		}
	}

	delete rSingle;
	of.close();

}

// #############################################################################

void RR_Performer::run() {

	// read in tree
	_baseTree = RR_Tree(_rri.inTree);

	/* add values to leaf nodes.
	 @todo think about that:
	 we add it to the simulation index here basically because
	 the "correct" (gold standard) values are stored in the
	 simulation index by default*/
	FloatTreeAnnotation* simulation = new FloatTreeAnnotation(_rri.phenoPath,_baseTree.getRoot(), Constants::simulationIndex);
	_baseTree.addAnnotation(Constants::simulationIndex, simulation);

	Ridge_Single* rSingle = new Ridge_Single(_id, false);
	float lambda;
	// run RidgeRace with best lambda
	RR_LambdaSearch* lambdaSearch = new RR_LambdaSearch(_baseTree);
	lambdaSearch->run();
	lambda = lambdaSearch->getBestLambda();
	// delete will NOT remove _baseTree
	delete lambdaSearch;

	Ridge_Result* r = rSingle->run(_baseTree);
	_results.push_back(r);

	cout << "original tree with lambda = " << lambda << endl;

	cout << "#NEXUS\n";
	cout << "begin trees;\n";
	cout << "tree 'T' = ";
	_baseTree.print(Newick, -1, std::cout);
	cout << ";\n";
	cout << "end;";

	rSingle->setLambda(lambda * 10.0f);
	r = rSingle->run(_baseTree);
	_results.push_back(r);

	cout << "weight tree with lambda = " << (10.0f * lambda) << endl;

	cout << "#NEXUS\n";
	cout << "begin trees;\n";
	cout << "tree 'T' = ";
	_baseTree.print(Newick, -1, std::cout);
	cout << ";\n";
	cout << "end;";

	// @ todo reactivate this nice feature as soon as Ridge_Single has
	// its lambda search internalized
	/*
	 Result* res;
	 cerr << "Running ... " << endl;
	 for (size_t i = 0; i < _algos.size(); i++) {
	 cerr << "running " << _algos.at(i)->getDescription() << endl;
	 res = _algos.at(i)->run(_baseTree);
	 cerr << "finished " << _algos.at(i)->getDescription() << endl;
	 delete res;
	 }
	 */

	cerr << " correlation: " << _baseTree.correlate("simulation", "ridge", true)
			<< endl;

	cerr << "Finished RR_Performer.\n";
}

void RR_Performer::evaluate() {

}

// #############################################################################

void RR_Example::run() {

	// read in tree
	_baseTree.setRandom(_rri.startTreeSize, _rri.treeSimFile);
	RegimeSpecification rs(3, _rri.grandMean, _rri.startStd, _rri.stdMode);
	cerr << "Set Regime specifications" << endl;

	// create a random tree;
	cerr << "Created random tree with" << _rri.startTreeSize << "nodes" << endl;

	// simulate Brownian Motion style evolution
	_baseTree.setBiasedSimulator(rs);

	cerr << "Initialized Biased Simulator" << endl;

	_baseTree.simulateBM();
	cerr << "Simulated Brownian motion" << endl;

	for (size_t i = 0; i < _algos.size(); i++) {
		cerr << _algos.at(i)->getDescription() << endl;
		_algos.at(i)->run(_baseTree);
		cerr << "finished " << _algos.at(i)->getDescription() << endl;
	}


	cerr << "Finished RR_Example.\n";

}

void RR_Example::evaluate() {

	cout << "#NEXUS\n";
	cout << "begin trees;\n";
	cout << "tree 'T' = ";
	_baseTree.print(Newick, -1, std::cout);
	cout << ";";
	cout << "end;";
}

// @ todo as soon as ridge stuff is also stored in tree annotations,
// update this!
void RR_Inference::getMutationStatsPerPosition(RR_Node* curNode,
		MutationTreeAnnotation* mutations, FloatTreeAnnotation* ridgeWeight,
		PositionWeightMap &pwm) {

	// save weights for all mutations on a branch to this node
	// but only if this is not a leaf

	int MIN_SUPPORT = 3;

	IntegerTreeAnnotation* support;
	support = (IntegerTreeAnnotation*) _baseTree.getAnnotation("support");

	//cerr << "WARNING in RR_Inference::getMutationStatsPerPosition: including mutations on external branches" << endl;
	//if (!curNode->isLeaf()) {
	if (support->getElement(curNode) > MIN_SUPPORT) {
		size_t mutId;
		Mutation mut;
		size_t nrOfMutsOnThisBranch = mutations->getElement(curNode).size();
		for (mutId = 0; mutId < nrOfMutsOnThisBranch; mutId++) {
			mut = mutations->getElement(curNode).at(mutId);
			if (mut._from == "A") {
				pwm[mut._pos].push_back(ridgeWeight->getElement(curNode));
			}
			if (mut._from == "B") {
				pwm[mut._pos].push_back(
						-1.0f * ridgeWeight->getElement(curNode));
			}
			//pwm[mut._pos].push_back( ridgeWeight->getElement(curNode) / float(nrOfMutsOnThisBranch) );
		}
	}

	// do the same for all children
	RR_Node* curChild;
	for (size_t childIndex = 0; childIndex < curNode->children.size();
			childIndex++) {
		curChild = curNode->children.at(childIndex);
		getMutationStatsPerPosition(curChild, mutations, ridgeWeight, pwm);
	}
}

void RR_Inference::printPositionWeightMap(PositionWeightMap pwm,
		Table posNames) {
	BasicStats ms;
	cerr
			<< "\n\n================= position weight map ======================\n\n";
	cerr << "pos\tname\tmean\tstd\tmin\tmax\ttotal\n";

	// read pwm, sort results by absolute mean
	PositionWeightMap::iterator iter;
	map<float, size_t> sortedResults;
	for (iter = pwm.begin(); iter != pwm.end(); iter++) {
		ms = getBasicStats(iter->second);
		sortedResults[(abs(ms.mean))] = iter->first;
	}

	// go through sorted map, print out pwm
	map<float, size_t>::iterator sortIter;
	size_t curPos;
	for (sortIter = sortedResults.begin(); sortIter != sortedResults.end();
			sortIter++) {
		curPos = sortIter->second;
		ms = getBasicStats(pwm.at(curPos));
		cerr << curPos << '\t' << posNames.T.at(curPos - 1).at(0) << '\t'
				<< ms.mean;
		cerr << '\t' << ms.std << '\t' << ms.min << '\t' << ms.max << '\t'
				<< pwm.at(curPos).size() << '\n';
	}
}

void RR_Inference::printMutationWeightMap(MutationWeightMap mwm) {
	BasicStats ms;
	MutationWeightMap::iterator iter;
	cerr
			<< "\n\n================= mutation weight map ======================\n\n";
	cerr << "mut\tmean\tstd\ttotal\n";

	map<float, Mutation> sortedResults;
	for (iter = mwm.begin(); iter != mwm.end(); iter++) {
		ms = getBasicStats(iter->second);
		sortedResults.at(abs(ms.mean)) = iter->first;
	}

	map<float, Mutation>::iterator sortIter;
	Mutation curMut;
	for (sortIter = sortedResults.begin(); sortIter != sortedResults.end();
			sortIter++) {
		curMut = sortIter->second;
		ms = getBasicStats(mwm.at(curMut));
		cerr << curMut.getString() << '\t' << ms.mean;
		cerr << '\t' << ms.std << '\t' << iter->second.size() << '\n';
	}
}

// @ todo as soon as ridge stuff is also stored in tree annotations,
// update this!
void RR_Inference::getMutationStatsPerMutation(RR_Node* curNode,
		MutationTreeAnnotation* mutations, FloatTreeAnnotation* ridgeWeight,
		MutationWeightMap &mwm) {

	// save weights for all mutations on a branch to this node
	// but only if this is not a leaf
	if (!curNode->isLeaf()) {
		size_t mutId;
		Mutation mut;
		for (mutId = 0; mutId < mutations->getElement(curNode).size();
				mutId++) {
			mut = mutations->getElement(curNode).at(mutId);
			mwm[mut].push_back(ridgeWeight->getElement(curNode));
		}
	}

	// do the same for all children
	RR_Node* curChild;
	for (size_t childIndex = 0; childIndex < curNode->children.size();
			childIndex++) {
		curChild = curNode->children.at(childIndex);
		getMutationStatsPerMutation(curChild, mutations, ridgeWeight, mwm);
	}
}

void RR_Inference::printMutationStatsPerNode(RR_Node* curNode,
		MutationTreeAnnotation* mutations, FloatTreeAnnotation* simulation,
		FloatTreeAnnotation* ridge, FloatTreeAnnotation* ridgeWeight) {

	size_t MAX = 5;
	size_t mutNum = mutations->getElement(curNode).size();

	if (mutNum > 0) {

		// 1. print the node id
		cerr << curNode->id << '\t';

		// 2. print all mutations at this node

		size_t mut;
		for (mut = 0; ((mut < mutNum) & (mut < MAX)); mut++) {
			cerr << mutations->getElement(curNode).at(mut).getString() << ',';
		}
		if (mut == MAX) {
			cerr
					<< "-- WARNING in RR_Inference::printMutationStatsPerNode: reached maximum output limit\n";
		}

		// 3. if possible, print out true value
		if (simulation->hasElement(curNode)) {
			cerr << '\t' << simulation->getElement(curNode) << '\t';

		}	// 3. else print out NA
		else {
			cerr << "\tNA\t";
		}

		// 4. print out ridge value
		cerr << ridge->getElement(curNode) << '\t';

		// 5. print out ridge weight
		cerr << ridgeWeight->getElement(curNode) << '\t';

		// 6. print out label (or nothing)
		cerr << curNode->info << '\n';
	}

	// 7. do the same for all children
	RR_Node* curChild;
	for (size_t childIndex = 0; childIndex < curNode->children.size();
			childIndex++) {
		curChild = curNode->children.at(childIndex);
		printMutationStatsPerNode(curChild, mutations, simulation, ridge,
				ridgeWeight);
	}
}

void RR_Inference::evaluate() {

	_baseTree.annotateNodeSupport();

	Table posNames(_rri.posNamePath);
	cerr << "opening _rri.posNamePath " << endl;

	cerr << "\n\n================= tree ======================\n\n";

	MutationTreeAnnotation* mutations =
			(MutationTreeAnnotation*) _baseTree.getAnnotation("mutations");
	mutations->rewriteMutStrings(posNames);

	cout << "#NEXUS\n";
	cout << "begin trees;\n";
	cout << "tree 'T' = ";
	_baseTree.print(Newick, 3, std::cout);
	cout << ";";
	cout << "end;";

	FloatTreeAnnotation* ridge = (FloatTreeAnnotation*) _baseTree.getAnnotation(
			Constants::ridgeIndex);
	FloatTreeAnnotation* ridgeWeight =
			(FloatTreeAnnotation*) _baseTree.getAnnotation(
					Constants::ridgeWeightIndex);
	FloatTreeAnnotation* simulation =
			(FloatTreeAnnotation*) _baseTree.getAnnotation(
					Constants::simulationIndex);

	// do not call for root directly
	// no mutations are stored for it
	RR_Node* curChild;

	cerr << "\n\n================= branch-wise map ======================\n\n";
	cerr << "nodeID\tmutations\ttrueValue\tinferredValue\tinferredWeight\tlabel"
			<< endl;

	PositionWeightMap pwm;

	for (size_t childIndex = 0;
			childIndex < _baseTree.getRoot()->children.size(); childIndex++) {
		curChild = _baseTree.getRoot()->children.at(childIndex);
		printMutationStatsPerNode(curChild, mutations, simulation, ridge,
				ridgeWeight);
		getMutationStatsPerPosition(curChild, mutations, ridgeWeight, pwm);
	}

	printPositionWeightMap(pwm, posNames);

}

void RR_Inference::run() {

	// read in tree
	_baseTree = RR_Tree(_rri.inTree);

	// add values to leaf nodes.
	//_baseTree.addIndex(ridgeWeightIndex, "pH", _rri.phenoPath, false);	///< @ todo why would I do that?

	FloatTreeAnnotation* simulation = new FloatTreeAnnotation(_rri.phenoPath,
			_baseTree.getRoot(), Constants::simulationIndex);
	_baseTree.addAnnotation(Constants::simulationIndex, simulation);

	//_baseTree.addIndex(simulationIndex, "pH", _rri.phenoPath, false);

	StringTreeAnnotation* sta = new StringTreeAnnotation(_rri.sequencePath,
			Fasta, _baseTree.getRoot(), "fastaAnnotation");
	_baseTree.addAnnotation("sequence", sta);

	Result* res;
	cerr << "Running ... " << endl;
	for (size_t i = 0; i < _algos.size(); i++) {
		cerr << " starting " << _algos.at(i)->getDescription() << " algorithm"
				<< endl;
		res = _algos.at(i)->run(_baseTree);
		cerr << " finished " << _algos.at(i)->getDescription() << endl;
		delete res;
	}

}

