/*
 * Simulator.cpp
 *
 *  Created on: Nov 8, 2012
 *      Author: ctusche
 */

#include "Simulator.h"
#include "SetsAndMaps.h"
#include "Random.h"
#include "Exception.h"
#include <cstdlib>

Simulator::Simulator(RR_Node* tree, RegimeSpecification rs) {
	this->baseTree = tree;
	this->rs = rs;
	init();
}

void Simulator::simulate(RR_Node* n, SimulationPackage sm, float curStd, float curMu) {

	// if we reached the start of a new regime, update the std for it
	IdType id = n->id;
	if (contains(regimeHeads, id)) {
		curStd = regimeHeads[id];
		//n->features[regimeIndex] = id;
		sm.regimeAnno->setElement(n, id);
	}

	sm.stdAnno->setElement(n, curStd);


	float v, bl;

	for (size_t c = 0; c < n->size(); c++) {

		// childrens value is drawn from N(mu_father, branch_length * std)
		// see Felsenstein 73 for that
		bl = n->children[c]->dist;
		v = randNormal(curMu, curStd * bl);
		//n->children[c]->features[simulationIndex] = v;
		sm.simulation->setElement(n->children[c], v);
		simulate(n->children[c], sm, curStd, v);
	}

}

float getRegimeStd(float baseStd, BaseStdMode mode) {
	float resStd;
	if (mode == constStd) {
		resStd = baseStd;
	}
	else {
		// get a value from N(0, baseStd):
		//resStd = abs(randNormal(0, rs._baseStd));

		// get a value from U(0, baseStd):
		resStd = float(rand()) / float(RAND_MAX) * baseStd;
	}
	return resStd;
}

void Simulator::init() {


	this->regimeHeads.clear();

	// compute the height for every node in the tree
	// if this is called multiple times, it will stop immediately at the root
	baseTree->annotateNodeHeight();

	// get the ids of all nodes that have a minimum height of 2
	set<IdType> allIds = baseTree->getSetOfAllIdsBelow(2);
	//set<IdType> allIds = baseTree->getSetOfAllIdsBelow();

	//if (rs._nrOfRegimes > 0.25 * allIds.size()) {
	if (rs._nrOfRegimes >= allIds.size()) {
		cerr << "\n\nERROR in Simulator::init()\n";
		cerr << "You are asking for an unreasonable large number of regimes!\n";
		cerr << " nr of regimes required:\t" << rs._nrOfRegimes << "\n";
		cerr << " available regime heads:\t" << allIds.size() << "\n";
		cerr << "This might take a long time - better update the ";
		cerr << "implementation in RRNode* Simulator::simulate()";
		throw Exception("ERROR in Simulator::simulate: too many regimes\n");
	}


	this->regimeHeads[baseTree->id] = getRegimeStd(rs._baseStd, rs._mode);


	IdType tmp;
	while (this->regimeHeads.size() != rs._nrOfRegimes) {
		tmp = getRandomElement(allIds);
		this->regimeHeads[tmp] = getRegimeStd(rs._baseStd, rs._mode);
	}

}

void Simulator::reInitialize() {
	this->regimeHeads.clear();
	init();
}


SimulationPackage Simulator::simulate() {

	if (this->regimeHeads.size() == 0) {
		Exception e;
		e << "ERROR in Simulator::simulate(): cannot simulate";
		e << "before regime definition is set. Run init() first!";
		throw e;
	}

	//baseTree->features[simulationIndex] = rs._grandMean;

	SimulationPackage sm;
	sm.regimeAnno = new IntegerTreeAnnotation(Constants::regimeIndex);
	sm.simulation = new FloatTreeAnnotation(Constants::simulationIndex);
	sm.stdAnno  = new FloatTreeAnnotation(Constants::stdIndex);

	sm.simulation->setElement(baseTree, rs._grandMean);
	simulate(baseTree, sm, -1.0, rs._grandMean);

	return sm;

}



void BiasedSimulator::init() {


	this->regimeHeads.clear();

	// compute the height for every node in the tree
	// if this is called multiple times, it will stop immediately at the root
	baseTree->annotateNodeHeight();

	// get the ids of all nodes that have a minimum height of 2
	set<IdType> allIds = baseTree->getSetOfAllIdsBelow(2);
	//set<IdType> allIds = baseTree->getSetOfAllIdsBelow();

	//if (rs._nrOfRegimes > 0.25 * allIds.size()) {
	if (rs._nrOfRegimes >= allIds.size()) {
		cerr << "\n\nERROR in Simulator::init()\n";
		cerr << "You are asking for an unreasonable large number of regimes!\n";
		cerr << " nr of regimes required:\t" << rs._nrOfRegimes << "\n";
		cerr << " available regime heads:\t" << allIds.size() << "\n";
		cerr << "This might take a long time - better update the ";
		cerr << "implementation in RRNode* Simulator::simulate()";
		throw Exception("ERROR in Simulator::simulate: too many regimes\n");
	}


	this->regimeHeads[baseTree->id] = getRegimeStd(rs._baseStd, rs._mode);

	this->regimeHeads[baseTree->children.at(0)->id] = 1.3;
	this->regimeHeads[baseTree->children.at(1)->id] = 5.3;
	this->regimeHeads[baseTree->children.at(0)->children.at(1)->children.at(1)->id] = 2.3;

	cerr << "SET A BIASED SIMULATER" << endl;

}
