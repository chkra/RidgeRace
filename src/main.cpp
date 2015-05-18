//============================================================================
// Name        : RidgeRacer_P.cpp
// Author      : ct
// Version     :
// Copyright   :
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <cstdlib>
#include <cstdio>

using namespace std;
#include "RidgeRacer.h"
#include "RidgeRaceInput.h"
#include "Exception.h"

int main(int argc, char* argv[]) {

	if (argc != 2) {
		cerr << "Usage: RidgeRace <path to config file>\n";
		cerr << "set testMode accordingly:\n";
		cerr << "\t0: Correlator\n";
		cerr
				<< "\t    Will perform RidgeRace manuscript simulations according to Config\n";
		cerr << "\t1: Check\n";
		cerr << "\t    Will test main functions\n";
		cerr << "\t2: Performer\n";
		cerr << "\t    Will perform ridge race reconstruction on real data\n";
		cerr << "\t    Set inTree and phenoPath in Config!\n";
		cerr << "\t3  Example\n";
		cerr
				<< "\t    Will create a random tree and perform ridge race reconstruction\n";
		cerr << "\t    Use this for images etc.\n";
		cerr << "\t4: Inferer\n";
		cerr << "\t    Will perform ridge race reconstruction on real data\n";
		cerr << "\t    Will reconstruct mutations\n";
		cerr << "\t    Set inTree, seqTree and phenoPath in Config!\n";
		cerr << "\t5: Lambda Search\n";
		cerr << "\t    Will perform ridge race reconstruction on real data\n";
		cerr << "\t    Running through lambdas to find good parametrization\n";
		cerr << "\t    Set inTree and phenoPath in Config!\n";
		return -1;
	}

	// ------------------------------------------------------------------------

	RidgeRacer* RR;
	time_t begin, end;
	try {

		time(&begin);

		cerr << "\nStarting RidgeRace." << endl;

		RidgeRaceInput rri = readConfigFile(argv[1]);
		cerr << "Read configuration  file " << argv[1] << endl;

		switch (rri.testMode) {
		case 0:
			RR = new RR_Correlater(rri);// will run ML, GLS and RidgeRace on simulated trees
			break;
		case 1:
			RR = new RR_Check(rri);	// will run a few default functions on a random tree
			break;
		case 2:
			RR = new RR_Performer(rri);	// will take given tree & phenotypes & perform phenotype ACE
			break;
		case 3:
			RR = new RR_Example(rri);// will perform phenotype ACE on random data
			break;
		case 4:
			RR = new RR_Inference(rri);	// will perform ACE for phenotype & sequences and rank mutations
			break;
		case 5:
			RR = new RR_LambdaSearch(rri);// will perform lambda search
			break;
		default:
			RR = new RR_Check(rri);
			break;
		}

		RR->run();
		RR->print();
		RR->evaluate();

		delete RR;

	} catch (Exception &e) {
		cerr << "\n\n";
		cerr << e.msg() << endl;
		cerr << "Catched an Exception. Exit." << endl;
		return -1;

	} catch (const char* x) {
		cerr << "\n\n";
		cerr << x << endl;
		cerr << "Catched an Exception. Exit." << endl;
		return -1;
	}

	// ------------------------------------------------------------------------

	time(&end);

	std::cerr << "\n\n-----------------------------------\n";
	std::cerr << "All is well.\n";
	std::cerr << "Time elapsed: " << float(difftime(end, begin)) / 60.0
			<< " minutes" << endl;

	return 0;

}
