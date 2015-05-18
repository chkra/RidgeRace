/*
 * Simulator.h
 *
 *  Created on: Mar 25, 2013
 *      Author: ctusche
 */

#ifndef SIMULATOR_H_
#define SIMULATOR_H_

//#include "RR_Tree.h"
//#include "RR_Node.h"
#include "RR_TreeAnnotation.h"


struct SimulationPackage {
	FloatTreeAnnotation* simulation;
	FloatTreeAnnotation* stdAnno;
	IntegerTreeAnnotation* regimeAnno;
};


class Simulator {

protected:

	RR_Node* baseTree;
	RegimeSpecification rs;
	void simulate(RR_Node* n, SimulationPackage sm, float curStd, float curMu);

	map<IdType, float> regimeHeads;

	/// Defines regimes and the standard deviation in them.
	/**
	 * Standard deviation is drawn from N(0,baseStd), and baseStd is
	 * defined in rs, given to constructor;
	 */
	void init();

public:

	Simulator(RR_Node* tree, RegimeSpecification rs);

	Simulator() {};

	/// for a given regime definition, runs a Brownian Motion process.
	SimulationPackage simulate();

	/// Removes current regimes and defines new ones.
	void reInitialize();

	/// Informs if this simulator has been initialized or not.
	bool isRead();

};


class BiasedSimulator : public Simulator {

private:
	void init();

public:
	BiasedSimulator() : Simulator() {
		Simulator();
	}

	BiasedSimulator(RR_Node* tree, RegimeSpecification rs) : Simulator(tree,rs) {
		//Simulator(tree, rs);
		init();
	}


};



#endif /* SIMULATOR_H_ */
