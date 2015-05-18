#include "RR_TreeAnnotator.h"

#include <sstream>

TreeAnnotator::TreeAnnotator(bool verbose) {
	_verbose = verbose;
}

/*
void TreeAnnotator::annotate(RR_Node* node, string path, size_t index,
		bool ridgeScheme) {

	DataParser dp(path);
	SampleNameToPhenotypeMapping spm = dp.getSampleNameToPhenotypeMapping(1);

	// if we use the ridge scheme, the root should have an appropriate distance
	// (typically, the root has no distance at all because there simply is no
	// edge above it. However, for the ridge scheme we assume one)
	if (ridgeScheme) {
		if (node->dist != 1) {
			cerr << "WARNING in TreeAnnotator::annotate: ";
			cerr << "found " << node->dist;
			cerr << " for branch length above root\n";
		}
	}

	addSampleNameToPhenotypeMapping(node, spm, index, ridgeScheme);

}*/
/*
void TreeAnnotator::addSampleNameToPhenotypeMapping(Node* node,
		SampleNameToPhenotypeMapping sMap, size_t index, bool ridgeScheme) {

	SampleNameToPhenotypeMapping::iterator iter;

	// if this node has a name ...
	if (node->info.size() > 0) {

		// ... look up node nodes name in the mapping
		iter = sMap.find(node->info);

		if (iter != sMap.end()) { // if we found it, store the phenotype
				// if normal scheme
			if (!ridgeScheme) {
				node->features[index] = iter->second;
			} else {
				float phenoRate = iter->second;
				float fathersVal;
				// in the ridge scheme, the root has a father in distance
				// d=1. Its value is zero.
				if (node->father == 0) {
					fathersVal = 0;
				} else {
					fathersVal = node->father->features[index];
				}

				// this nodes val = fathersval + phenorate * dist
				node->features[index] = fathersVal;
				node->features[index] += phenoRate * node->dist;

				if (_verbose) {
					cout << node->info << '\t';
					cout << node->features[0] << '\t';
					cout << node->features[index] << '\n';
				}
			}
		} else {
			// if node is a child, things are bad, because our annotation is
			// incomplete, give a warning
			//if (node->children.size() == 0) {
			cerr
					<< "WARNING in TreeAnnotator::addSampleNameToPhenotypeMapping:";
			cerr << "unable to find phenotype information for node with label ";
			cerr << node->info << endl;

			SampleNameToPhenotypeMapping::iterator iter;
			cerr << "here are the (max) first 10 entries of the mapping:\n";
			size_t k =0;
			for (iter=sMap.begin(); iter!=sMap.end(); iter++) {
				if (k==10) break;
				k++;
				cerr << '\t' << iter->first << '\t' << iter->second << '\n';
			}

			//throw("ERROR in TreeAnnotator::addSampleNameToPhenotypeMapping ");
			//}
		}

	}

	for (size_t i = 0; i < node->children.size(); i++) {
		addSampleNameToPhenotypeMapping(node->children[i], sMap, index,
				ridgeScheme);
	}

}
*/
/*
void TreeAnnotator::addNodeToPhenotypeMapping(Node* node,
		NodeAnnotation<float>::Type sMap, size_t index, bool ridgeScheme) {

	NodeAnnotation<float>::Type::iterator iter;
	Node* tmp;

	// if we simply have to enter the values, don't use tree structure, just add
	// using the pointer information
	if (!ridgeScheme) {
		for (iter = sMap.begin(); iter != sMap.end(); iter++) {
			tmp = iter->first;
			tmp->features[index] = iter->second;
		}
	}

	// if we have to use ridge scheme, use tree structure by recursive calls
	// to this method
	else {

		iter = sMap.find(node);
		if (iter != sMap.end()) { // if we found it, store the phenotype

			float phenoRate = iter->second;
			float fathersVal;

			// in the ridge scheme, the root has a father in distance
			// d=1. Its value is zero.
			if (node->father == 0) {
				fathersVal = 0;
			} else {
				fathersVal = node->father->features[index];
			}

			// ridgescheme means:
			// this nodes val = fathersval + pheno * dist
			node->features[index] = fathersVal;
			node->features[index] += phenoRate * node->dist;

			if (_verbose) {
				cout << node->info << '\t';
				cout << node->features[0] << '\t';
				cout << node->features[index] << '\n';
			}

		}
	}

	for (size_t i = 0; i < node->children.size(); i++) {
		addNodeToPhenotypeMapping(node->children[i], sMap, index, ridgeScheme);
	}

}
*/

/*
void TreeAnnotator::annotate(RR_Node* node, size_t index,
		NodeAnnotation<float>::Type v, bool ridgeScheme) {

// if we use the ridge scheme, the root should have an appropriate distance
// (typically, the root has no distance at all because there simply is no
// edge above it. However, for the ridge scheme we assume one)
	if (ridgeScheme) {
		if (node->dist != 1) {
			cerr << "WARNING in TreeAnnotator::annotate: ";
			cerr << "found " << node->dist;
			cerr << " for branch length above root\n";
		}
	}

	addNodeToPhenotypeMapping(node, v, index, ridgeScheme);

}*/

