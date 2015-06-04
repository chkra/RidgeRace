/*
 * RR_Algorithms.cpp
 *
 *  Created on: Mar 25, 2013
 *      Author: ctusche
 */

#include "RR_Algorithms.h"
#include "RidgeSolver.h"
#include "Node.h"
#include "RR_Tree.h"
#include "prefix.h"

#include "StringManipulation.h"
#include "SetsAndMaps.h"
#include <cstdlib>
#include <cstdio>

using namespace std;

string TreePlot::_outPath;
unsigned int TreePlot::_counter = 0;

// ============================================================================

pair<string, size_t> TreePlot::outputSingleTree(RR_Tree &tree) {

	_counter++;
	ostringstream os;
	os << _outPath << '_' << _counter << ".newick";
	string path = os.str();

	ofstream of;
	of.open(path.c_str());

	//of << "#NEXUS\n";
	//of << "begin trees;\ntree TREE1 = [&R] ";
	tree.print(Newick, -1, of);
	//of << ";\n";
	//of << "end;\n";
	//of.close();

	pair<string, size_t> p;
	p.first = path;
	p.second = _counter;

	return p;
}

string TreePlot::getDescription() {
	return "TreePlot";
}

// ---------------------------------------------------------------------------

Result* TreePlot::run(RR_Tree &tree) {

	if (tree.hasInterestingSummary()) {
		AnnoTree_Result* res = new AnnoTree_Result();

		pair<string, size_t> p = outputSingleTree(tree);

		res->setTreePath(p.first);
		res->setTreeID(p.second);
		return res;

	} else {
		Empty_Result* res = new Empty_Result();
		return res;
	}
}

// ============================================================================

/*Ridge_Result* Ridge_ACR::run(RR_Tree &tree) {

 float lambda;
 Ridge_Result* res = new Ridge_Result();
 pair<string, string> s;
 float corr;

 // extract features from simulation and write them down
 cerr << "wrote features to " << _ridgeInputPath << endl;
 tree.printLeafFeatures(_ridgeInputPath);

 cerr << "wrote features to " << _ridgeInputPath << endl;

 for (float exp = -2.0f; exp >= -6.0f; exp -= 1.0f) {
 //for (float exp = +2.0f; exp >= -6.0f; exp -= 1.0f) {

 lambda = pow(10, exp);
 vector<string> params;
 params.push_back(_ridgeInputPath);
 params.push_back(toString(lambda));
 params.push_back(toString(_headID));

 Caller C;
 C.callR("scripts/ridge.r", params);

 // read in results of ridge regression
 tree.addIndex(ridgeIndex, ridgeIndex, _ridgeOutputPath, true);
 tree.addIndex(ridgePhenoIndex, "ridgePheno", _ridgeOutputPath, false);

 corr = tree.correlate(simulationIndex, ridgeIndex, false);

 s.first = toString(lambda);
 s.second = toString(corr);
 res->addEntry(s, tree);
 }

 tree.setSummary(ridgeIndex, res->getCharacteristic());

 return (res);

 }*/

struct NamedUvector {
	uvector values;
	vector<RR_Node*> names;
};

NamedUvector getUvectorFromLfd(LeafFeatureData lfd) {

	NamedUvector nu;

	FloatTreeAnnotation* nodeValues = lfd.phenotypes;
	nu.values = uvector(nodeValues->size());

	map<RR_Node*, float> phenotypeData = nodeValues->getFullData();
	map<RR_Node*, float>::iterator iter;

	size_t k = 0;
	for (iter = phenotypeData.begin(); iter != phenotypeData.end(); iter++) {
		nu.values(k) = iter->second;
		nu.names.push_back(iter->first);
		k++;
	}
	return nu;
}

struct NamedUmatrix {
	umatrix U;
	vector<RR_Node*> rowNames;
};

NamedUmatrix getUmatrixFromLfd(LeafFeatureData lfd) {

	FloatVectorTreeAnnotation* branchValues = lfd.branchLengths;
	map<RR_Node*, vector<float> > blData = branchValues->getFullData();
	map<RR_Node*, vector<float> >::iterator mIt;
	vector<RR_Node*> rowNames;

	NamedUmatrix num;
	umatrix M(lfd.branchLengths->size(), lfd.nodesOfThisTree.size());

	size_t m = 0;
	for (mIt = blData.begin(); mIt != blData.end(); mIt++) {
		for (size_t n = 0; n < mIt->second.size(); n++) {
			M(m, n) = mIt->second.at(n);
		}
		m++;
		rowNames.push_back(mIt->first);
	}

	num.U = M;
	num.rowNames = rowNames;

	return num;
}

/**
 * Update the entries in a NodeAnnotation<float> with the entries of a vector.
 * We need the oldAnno only to iterate through it in the correct order,
 * because the order of the keys in oldAnno is the order of the entries in v
 */
FloatTreeAnnotation updateNodeAnno(FloatTreeAnnotation oldAnno, uvector v) {

	FloatTreeAnnotation newAnno(oldAnno.getDescription());
	map<RR_Node*, float> data = oldAnno.getFullData();
	map<RR_Node*, float>::iterator iter;

	size_t entry = 0;
	RR_Node* curNode;
	for (iter = data.begin(); iter != data.end(); iter++) {
		curNode = iter->first;
		newAnno[curNode] = v(entry);
		entry++;
	}
	return newAnno;
}

FloatTreeAnnotation* getNodeAnnotation(NamedUvector v, string description) {

	FloatTreeAnnotation* nAnno = new FloatTreeAnnotation(description);
	vector<RR_Node*>::iterator iter;

	if (v.names.size() != v.values.size()) {
		cerr << "number of nodes: " << v.names.size() << endl;
		cerr << "number of values: " << v.values.size() << endl;
		throw Exception(
				"ERROR in getNodeAnnotation: values and nodes not synchronized\n");
	}

	for (size_t entry = 0; entry < v.names.size(); entry++) {
		nAnno->setElement(v.names.at(entry), v.values(entry));
	}

	return nAnno;
}

FloatTreeAnnotation* getNodeAnnotationRidgeScheme(NamedUvector v, RR_Node* root,
		string description) {

	FloatTreeAnnotation* ridge = getNodeAnnotation(v, description);
	ridge->updateToRidgeScheme(root);
	return ridge;

}

Ridge_Result* Ridge_MultiLambda::runOldStyleRidge(RR_Tree &tree) {

	cout << " \n...............................\n";
	cout << "Running old style\n";

	float lambda;
	Ridge_Result* res = new Ridge_Result();
	pair<string, string> s;

	// extract features from simulation and write them down
	tree.printLeafFeatures(_ridgeInputPath, Constants::simulationIndex);
	cerr << "wrote features to " << _ridgeInputPath << endl;

	for (float exp = -2.0f; exp >= -6.0f; exp -= 1.0f) {
		//for (float exp = +2.0f; exp >= -6.0f; exp -= 1.0f) {

		lambda = pow(10, exp);
		vector<string> params;
		params.push_back(_ridgeInputPath);
		params.push_back(toString(lambda));
		params.push_back(toString(_headID));

		Caller C;
		std::stringstream scriptFilename;
		scriptFilename << RidgeRace::prefix << "/share/" << RidgeRace::progname << "/scripts/ridge.r";
		C.callR(scriptFilename.str(), params);

		// read in results of ridge regression
		// call with ridgeScheme = true
		FloatTreeAnnotation* ancestors = new FloatTreeAnnotation(
				_ridgeOutputPath, tree.getRoot(), Constants::ridgeIndex, true);
		FloatTreeAnnotation* ridgeWeight = new FloatTreeAnnotation(
				_ridgeOutputPath, tree.getRoot(), Constants::ridgeWeightIndex);

		tree.addAnnotation(Constants::ridgeIndex, ancestors);
		tree.addAnnotation(Constants::ridgeWeightIndex, ridgeWeight);

		//tree.addIndex(ridgeIndex, ridgeIndex, _ridgeOutputPath, true);
		//tree.addIndex(ridgeWeightIndex, ridgeWeightIndex, _ridgeOutputPath, false);

		tree.correlate(Constants::ridgeIndex, Constants::simulationIndex, false);

		float corr = tree.correlate(Constants::simulationIndex, Constants::ridgeIndex, false);
		float mse = tree.MSE(Constants::simulationIndex, Constants::ridgeIndex, false);
		vector<string> s;
		s.push_back(toString(lambda));
		s.push_back(toString(corr));
		s.push_back(toString(mse));
		res->addEntry(s, tree);
	}

	tree.setSummary(Constants::ridgeIndex, res->getCharacteristic());

	tree.print(Newick);
	cout << " \n...............................\n";

	return (res);

}

Ridge_Result* Ridge_MultiLambda::run(RR_Tree &tree) {

	//runOldStyleRidge(tree);

	float lambda;
	Ridge_Result* res = new Ridge_Result();
	pair<string, string> s;

	// ------- add phylomean correction here

	// compute phyloMean
	PhylogeneticMean meanInferer(_headID);
	PhyloMean_Result* meanRes = meanInferer.run(tree);

	// center all data
	FloatTreeAnnotation* sim = (FloatTreeAnnotation*) tree.getAnnotation(
			"simulation");
	//sim->substractFromValues(meanRes->_mean);
	cerr << "estimated phylogenetic mean to be " << meanRes->_mean << endl;

	// --------------------------------------

	// extract features from simulation and write them down
	LeafFeatureData lfd = tree.getLeafFeatureData(Constants::simulationIndex);

	NamedUvector phenos = getUvectorFromLfd(lfd);
	NamedUmatrix blength = getUmatrixFromLfd(lfd);

	RidgeRegressionResult ridRes;

	for (float exp = -2.0f; exp >= -3.0f; exp -= 1.0f) {

		lambda = pow(10, exp);
		cout << "\n > lamda = " << lambda << endl;

		RidgeRegression RidReg(blength.U, phenos.values, lambda);

		ridRes = RidReg.solve();

		NamedUvector namedBeta;
		namedBeta.values = ridRes._beta;
		namedBeta.names = set2vec(lfd.nodesOfThisTree);

		FloatTreeAnnotation* ancestors = getNodeAnnotationRidgeScheme(namedBeta,
				tree.getRoot(), Constants::ridgeIndex);
		FloatTreeAnnotation* ridgeWeights = getNodeAnnotation(namedBeta,
				Constants::ridgeWeightIndex);

		tree.addAnnotation(Constants::ridgeIndex, ancestors);
		tree.addAnnotation(Constants::ridgeWeightIndex, ridgeWeights);

		tree.correlate(Constants::simulationIndex, Constants::ridgeIndex, false);

		FloatTreeAnnotation* difference =
				(FloatTreeAnnotation*) ancestors->getIdenticalCopy(
						"difference");
		difference->substractFromValues(sim);

		tree.addAnnotation("difference", difference);

		float corr = tree.correlate(Constants::simulationIndex,
				Constants::ridgeIndex, false);
		float mse = tree.MSE(Constants::simulationIndex, Constants::ridgeIndex,
				false);
		vector<string> s;
		s.push_back(toString(lambda));
		s.push_back(toString(corr));
		s.push_back(toString(mse));
		res->addEntry(s, tree);

		tree.print(Newick);
		cout << "\n<res>\ttotal-error-withLeafs:\t"
				<< difference->getSumSq(true) << '\n';
		cout << "<res>\ttotal-error-noLeafs:\t" << difference->getSumSq(false)
				<< '\n';

	}

	tree.setSummary(Constants::ridgeIndex, res->getCharacteristic());

	return (res);

}

string Ridge_MultiLambda::getDescription() {
	return "Ridge_MultiLambda";
}

// ============================================================================

GLS_Result* GLS_ACR::run(RR_Tree &tree) {

	ofstream of(_glsTreeInputPath.c_str());
	tree.print(SimpleNewick, -1, of);
	of.close();

	ofstream dataOf(_glsDataInputPath.c_str());
	tree.getAnnotation("simulation")->print(dataOf);
	dataOf.close();

	GLS_Result* res = new GLS_Result();
	vector<string> params;
	params.push_back(_glsTreeInputPath);
	params.push_back(_glsDataInputPath);
	params.push_back(toString(_headID));

	// call GLS regression
	Caller C;
	std::stringstream scriptFilename;
	scriptFilename << RidgeRace::prefix << "/share/" << RidgeRace::progname << "/scripts/gls.r";
	C.callR(scriptFilename.str(), params);

	//_glsOutputPath = "";

	FloatTreeAnnotation* gls = new FloatTreeAnnotation(this->_someOtherString, tree.getRoot(), "gls", false);

	tree.addAnnotation("gls", gls);

	float corr = tree.correlate(Constants::simulationIndex, Constants::glsIndex,
			false);
	float mse = tree.MSE(Constants::simulationIndex, Constants::glsIndex,
			false);
	vector<string> s;
	s.push_back("NA");
	s.push_back(toString(corr));
	s.push_back(toString(mse));
	res->addEntry(s, tree);

	tree.setSummary("gls", res->getCharacteristic());

	return (res);
}

string GLS_ACR::getDescription() {
	return "GLS_ACR";
}

// ============================================================================

ML_Result* ML_ACR::run(RR_Tree &tree) {

	//cerr << "Writing tree to " << _mlTreeInputPath << endl;
	ofstream of(_mlTreeInputPath.c_str());
	tree.print(SimpleNewick, -1, of);
	of.close();

	//cerr << "Writing data to " << _mlDataInputPath << endl;
	ofstream dataOf(_mlDataInputPath.c_str());
	tree.getAnnotation("simulation")->print(dataOf);
	dataOf.close();

	ML_Result* res = new ML_Result();

	vector<string> params;
	params.push_back(_mlTreeInputPath);
	params.push_back(_mlDataInputPath);
	params.push_back(toString(_headID));

	// call ML reconstruction
	Caller C;
	std::stringstream scriptFilename;
	scriptFilename << RidgeRace::prefix << "/share/" << RidgeRace::progname << "/scripts/ml.r";
	C.callR(scriptFilename.str(), params);

	FloatTreeAnnotation* ml = new FloatTreeAnnotation(_mlOutputPath, tree.getRoot(), "ml");
	tree.addAnnotation("ml", ml);

	float corr = tree.correlate(Constants::simulationIndex, Constants::mlIndex,false);
	float mse = tree.MSE(Constants::simulationIndex, Constants::mlIndex, false);
	vector<string> s;
	s.push_back("NA");
	s.push_back(toString(corr));
	s.push_back(toString(mse));
	res->addEntry(s, tree);

	tree.setSummary("ml", res->getCharacteristic());

	return (res);
}

string ML_ACR::getDescription() {
	return "ML_ACR";
}

// ============================================================================

string Ridge_Single::getDescription() {
	return "Ridge_Single";
}

Ridge_Result* Ridge_Single::run_oldStyle(RR_Tree &tree) {

	Ridge_Result* res = new Ridge_Result();

	// extract features from simulation and write them down
	tree.printLeafFeatures(_ridgeInputPath, Constants::simulationIndex);

	float exp = -6.0f;
	float lambda = pow(10, exp);

	vector<string> params;
	params.push_back(_ridgeInputPath);
	params.push_back(toString(lambda));
	params.push_back(toString(_headID));

	Caller C;
	std::stringstream scriptFilename;
	scriptFilename << RidgeRace::prefix << "/share/" << RidgeRace::progname << "/scripts/ridge.r";
	C.callR(scriptFilename.str(), params);

	// read in results of ridge regression

	FloatTreeAnnotation* ridgeWeights = new FloatTreeAnnotation(
			_ridgeOutputPath, tree.getRoot(), Constants::ridgeWeightIndex);
	tree.addAnnotation(Constants::ridgeWeightIndex, ridgeWeights);

	FloatTreeAnnotation* ancestors = new FloatTreeAnnotation(_ridgeOutputPath,
			tree.getRoot(), Constants::ridgeWeightIndex);
	ancestors->updateToRidgeScheme(tree.getRoot());
	tree.addAnnotation(Constants::ridgeIndex, ancestors);

	float corr = tree.correlate(Constants::simulationIndex,
			Constants::ridgeIndex, false);
	float mse = tree.MSE(Constants::simulationIndex, Constants::ridgeIndex,
			false);
	vector<string> s;
	s.push_back(toString(_lambda));
	s.push_back(toString(corr));
	s.push_back(toString(mse));
	res->addEntry(s, tree);

	return (res);

}

/* the same as the Ridge_ACR algorithm, but for a single lambda only
 * Creates empty result (there is nothing to evaluate -
 * inferred data is written to the tree
 * @ todo move this docu to header file
 */
Ridge_Result* Ridge_Single::run(RR_Tree &tree) {

	Ridge_Result* res = new Ridge_Result();
	pair<string, string> s;

	// extract features from simulation and write them down
	LeafFeatureData lfd = tree.getLeafFeatureData(Constants::simulationIndex);
	//lfd.print();

	NamedUvector phenos = getUvectorFromLfd(lfd);
	NamedUmatrix blength = getUmatrixFromLfd(lfd);

	RidgeRegressionResult ridRes;

	RidgeRegression RidReg(blength.U, phenos.values, _lambda);
	ridRes = RidReg.solve();

	NamedUvector namedBeta;
	namedBeta.values = ridRes._beta;
	namedBeta.names = set2vec(lfd.nodesOfThisTree);

	FloatTreeAnnotation* ancestors = getNodeAnnotationRidgeScheme(namedBeta,
			tree.getRoot(), Constants::ridgeIndex);

	FloatTreeAnnotation* ridgeWeights = getNodeAnnotation(namedBeta,
			Constants::ridgeWeightIndex);
	tree.addAnnotation(Constants::ridgeIndex, ancestors, _tryToCleanTree);
	tree.addAnnotation(Constants::ridgeWeightIndex, ridgeWeights, _tryToCleanTree);
	tree.setSummary(Constants::ridgeIndex, "NA");

	FloatTreeAnnotation* sim = (FloatTreeAnnotation*) tree.getAnnotation(
			Constants::simulationIndex);

	if (sim->size() == ancestors->size()) {
		try {
			float corr = tree.correlate(Constants::simulationIndex,
					Constants::ridgeIndex, false);
			float mse = tree.MSE(Constants::simulationIndex,
					Constants::ridgeIndex, false);
			float mpe = tree.MPE(Constants::simulationIndex,
					Constants::ridgeIndex);
			vector<string> s;
			s.push_back(toString(_lambda));
			s.push_back(toString(corr));
			s.push_back(toString(mse));
			s.push_back(toString(mpe));
			res->addEntry(s, tree);
		} catch (...) {
			throw Exception(
					"ERROR in Ridge_Single::run: error during correlate\n");
		}
	}

	phenos.values.resize(0, false);
	phenos.names.clear();
	blength.U.resize(0, 0, false);
	blength.rowNames.clear();
	namedBeta.names.clear();
	namedBeta.values.resize(0, false);
	ridRes._beta.resize(0,false);
	ridRes._yEst.resize(0,false);


	return (res);

}

/**
 * stores inferred mean values in mean, returns corrected
 * branch lengths upwards
 * @param curNode
 * @param mean
 * @return
 */
float PhylogeneticMean::inferMeans(RR_Node* curNode,
		FloatTreeAnnotation* mean) {

	float mu;

	if (curNode->isLeaf()) {
		// will throw exception of node has no annoatation
		mu = _values->getElement(curNode);
		mean->setElement(curNode, mu);
		return curNode->dist;
	} else {
		vector<float> branchLengths;
		float t;
		float denom = 0, nomMean = 0, nomBl = curNode->dist;
		for (size_t c = 0; c < curNode->size(); c++) {
			t = inferMeans(curNode->children.at(c), mean);
			branchLengths.push_back(1.0 / t);
			denom += 1.0f / t;
			nomMean += (1.0f / t) * mean->getElement(curNode->children.at(c));
			nomBl *= (1.0f / t);
		}
		mean->setElement(curNode, nomMean / denom);
		return curNode->dist * (nomBl / denom);
	}

}

PhyloMean_Result* PhylogeneticMean::run(RR_Tree &tree) {

	delete _values;
	_values = (FloatTreeAnnotation*) tree.getAnnotation(Constants::simulationIndex);

	FloatTreeAnnotation* fta = new FloatTreeAnnotation("phyloMean");
	inferMeans(tree.getRoot(), fta);
	tree.addAnnotation("phyloMean", fta);
	PhyloMean_Result* pmr = new PhyloMean_Result();
	pmr->_mean = fta->getElement(tree.getRoot());
	return pmr;
}

string PhylogeneticMean::getDescription() {
	return "phyloMean";
}

