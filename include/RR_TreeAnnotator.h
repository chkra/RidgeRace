/*
 * TreeAnnotator.h
 *
 *  Created on: Nov 14, 2012
 *      Author: ctusche
 */

#ifndef TREEANNOTATOR_H_
#define TREEANNOTATOR_H_


#include "Parser.h"
#include "RR_Node.h"
#include "RR_Tree.h"
#include "RidgeSolver.h"
#include "RR_TreeAnnotation.h"



class TreeAnnotator {


private:

	bool _verbose;

	/**
	 * @ todo well, this is weird, decide on difference between node and RR_Node ...
	 */
	void addSampleNameToPhenotypeMapping(Node* node, SampleNameToPhenotypeMapping sMap, size_t index, bool ridgeScheme);

	void addNodeToPhenotypeMapping(Node* node, FloatTreeAnnotation sMap, size_t index, bool ridgeScheme);

public:

	TreeAnnotator(bool verbose=false);

	void annotate(RR_Node* node, string path, size_t index, bool ridgeScheme);

	void annotate(RR_Node* node, size_t index, FloatTreeAnnotation v, bool ridgeScheme);

};

#endif /* TREEANNOTATOR_H_ */
