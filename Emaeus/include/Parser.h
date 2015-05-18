/*
 * Parser.h
 *
 *  Created on: Nov 8, 2012
 *      Author: ctusche
 */

#ifndef PARSER_H_
#define PARSER_H_

#include "Node.h"
#include <string>

using namespace std;


typedef map<string, map<string, float> > DistanceMatrix;

class NewickParser {

private:

	Node* tree;

	void clip(std::string &line, size_t length);

	void removeLeadingWhite(std::string &line);

	bool isName(char c);

	bool isNum(char c);

	std::string mySubstring(std::string in, size_t beg, size_t length);

	float getLeadingNumber(std::string &line);

	std::string getLeadingName(std::string &line);

	Node* iNode(std::string &line);

	Node* parse(std::string line);

public:

	NewickParser(string path);
	Node* getTree();

};



typedef std::map<string, float>  SampleNameToPhenotypeMapping;


/**
 * A multi-purpose class to parse all kinds of data sets. Is able to read
 * Fasta, Nexus and simple tables, will always return a #DataSet.
 */
class DataParser {

private:

	std::ifstream curFile;
	std::string line;
	std::string path;

	static std::string NO_HEADER_SYM;


	/**
	 * Given that this->mode is Fasta, getFastaEntry reads the next entry
	 * and writes it to header and sequence
	 * @param header (reference) fasta header is written here
	 * @param sequence (reference) fasta sequence is written here
	 */
	void getFastaEntry(std::string &header, std::string &sequence);

	void getTableEntry(std::string &name,std::vector<float> &values);

	std::vector<std::vector<std::string> > readTableString(std::string path);

public:

	/**
	 * Constructor.
	 * @param path	path to file containing data
	 */
	DataParser(std::string path);



	// --------------------------------------------------------------------

	//DataSet readNexus();

	map<string, string> readFasta();

	map<string, float> readStringFloat();

	//DataSet readTable();

	// --------------------------------------------------------------------

	/**
	 * Reads in a phenotype information file. Maps Phenotype descriptions to
	 * internal representation (CharacterStateType).
	 *
	 * Assumes that the phenotype information looks like
	 * someSampleId	someStringDescribingThePhenotype
	 * or
	 * someSampleId	4711
	 * or
	 * someSampleId	47.11
	 * Based on this assumption, the phenotype coding is converted to a
	 * CharacterStateType. In case of categorical (i.e. string) phenotypes,
	 * the strings are numbered based on their first appearance.
	 * @todo describe parameters
	 */
	//PhenoNameToCharacterMapping
	//getPhenoNameToCharacterMapping(bool numericPheno, bool floatPheno,
	//		size_t column);

	// --------------------------------------------------------------------

	/**
	 * Reads in a phenotype information file. Maps sample names
	 * (SampleNameType) to the internal representation of their phenotype
	 * (CharacterStateType).
	 *
	 * @todo describe parameters
	 */
	//SampleNameToPhenotypeMapping getSampleNameToPhenotypeMapping(size_t column,
	//		PhenoNameToCharacterMapping &phenoMap, bool numericPheno,
	//		bool floatPheno);

	SampleNameToPhenotypeMapping getSampleNameToPhenotypeMapping(size_t column);

	// --------------------------------------------------------------------

	DistanceMatrix readDistanceMatrix();

};


void printDistanceMatrix(DistanceMatrix D);


#endif /* PARSER_H_ */
