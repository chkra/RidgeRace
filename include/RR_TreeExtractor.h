/*
 * TreeExtractor.h
 *
 *  Created on: Nov 14, 2012
 *      Author: ctusche
 */

#ifndef TREEEXTRACTOR_H_
#define TREEEXTRACTOR_H_

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include "RR_TreeAnnotation.h"

// DON'T do that ... this will make 'vector' ambiguous ... ugh ...
//using namespace boost::numeric::ublas;


typedef boost::numeric::ublas::vector<float> LeafEntryVector;






class TreeExtractor {

private:
	RR_Node* tree;

	void printLeafFeatures(RR_Node* curNode,
			const std::set<IdType> nodesOfThisTree, FloatTreeAnnotation* pheno, std::map<IdType, float> M,
			std::ostream &target);
	void getIdToInfoMapping(RR_Node* curNode, map<IdType, string> &M);

	void getLeafFeatureData(RR_Node* curNode,
			std::map<IdType, float> M,
			FloatTreeAnnotation* pheno,
			LeafFeatureData &lfd);

public:

	TreeExtractor() {};
	TreeExtractor(RR_Node* tree);

	void printLeafFeatures(std::ostream &target, FloatTreeAnnotation* pheno);
	void printLeafFeatures(string basePath, FloatTreeAnnotation* pheno);

	LeafFeatureData getLeafFeatureData(FloatTreeAnnotation* pheno);

};

#endif /* TREEEXTRACTOR_H_ */
