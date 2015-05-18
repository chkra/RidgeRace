/*
 * DataParser.cpp
 *
 *  Created on: Nov 14, 2012
 *      Author: ctusche
 */

#include "Parser.h"
#include "Exception.h"
#include "StringManipulation.h"

// ############################################################################

std::string DataParser::NO_HEADER_SYM = "$$$%%%$$$";

DataParser::DataParser(std::string path) {
	this->path = path;
	curFile.open(path.c_str());
	if (!curFile.is_open()) {
		std::cerr << "filename: " << path << std::endl;
		throw Exception("ERROR in Dataparser: unable to open file!\n");
	}
}

// --------------------------------------------------------------------------

void DataParser::getTableEntry(std::string &name, std::vector<float> &values) {
	name.clear();
	values.clear();

	std::string line;
	size_t pos;

	if (this->curFile.eof()) {
		name = NO_HEADER_SYM;
		return;
	}
	getline(this->curFile, line);

	if (line.size() < 3) {
		name = NO_HEADER_SYM;
		return;
	}

	pos = line.find('\t');
	if (pos == std::string::npos) {
		pos = line.find(' ');
	}
	if (pos == std::string::npos) {
		std::cerr << line << std::endl;
		throw Exception(
				"ERROR in DataParser::getTableEntry: no name separable\n");
	}
	name = line.substr(0, pos); // get identifier of this line
	values = tokenizeToFloat(line.substr(pos + 1, line.size() - pos));
}

void DataParser::getFastaEntry(std::string &header, std::string &sequence) {

	header.clear();
	sequence.clear();

	// if necessary, go to begin of next entry
	if (line.find_first_of('>') == std::string::npos) {
		while (!this->curFile.eof()
				&& line.find_first_of('>') == std::string::npos) {
			getline(this->curFile, line);
		}
	}

	// if we came out the loop for false reasons, return dummy
	if (this->curFile.eof()) {
		header = NO_HEADER_SYM;
		sequence = "";
		return;
	}

	// otherwise everything is fine, line holds current fasta header
	header = line.substr(1, line.size() - 1);

	// extract sequence
	getline(this->curFile, line);
	while (line.length() != 0 && !this->curFile.eof()
			&& line.find_first_of('>') == std::string::npos) {
		sequence.append(line);
		getline(this->curFile, line);
	}

}

/*
 std::vector<std::vector<std::string> > DataParser::readTableString() {
 std::ifstream is;
 is.open(path.c_str());

 if (!is.is_open()) {
 std::cerr << path << std::endl;
 throw Exception("ERROR in readTableString: unable to open file!\n");
 }

 std::vector<std::vector<std::string> > v;
 std::vector<std::string> tokens;

 std::string line;
 while (!is.eof()) {
 getline(is, line);
 if (line.size() == 0)
 continue;
 tokens = tokenizeToString(line, "\t ");
 v.push_back(tokens);
 }

 return v;
 }
 */

std::map<string, float> DataParser::readStringFloat() {

	std::ifstream is;
	is.open(path.c_str());

	if (!is.is_open()) {
		std::cerr << path << std::endl;
		throw Exception(
				"ERROR in DataParser::readStringFloat: unable to open file!\n");
	}

	std::vector<std::string> tokens;
	std::map<string, float> res;

	std::string line;
	while (!is.eof()) {
		getline(is, line);
		if (line.size() == 0)
			continue;
		tokens = tokenizeToString(line, "\t ");
		if (tokens.size() != 2) {
			cerr << "line content >>" << line << "<<\n";
			throw Exception(
					"ERROR in DataParser::readStringFloat: line does not have two entries\n");
		}
		res[tokens[0]] = atof(tokens[1].c_str());
	}

	return res;
}

// ----------------------------------------------------------------------------

SampleNameToPhenotypeMapping DataParser::getSampleNameToPhenotypeMapping(
		size_t column) {

	SampleNameToPhenotypeMapping dpm;
	std::ifstream is(path.c_str());

	if (!is.is_open()) {
		std::cerr << "path: " << path << '\n';
		throw Exception(
				"ERROR in getSampleNameToPhenotypeMapping: file does not exist\n");
	}

	std::string line;
	std::vector<std::string> v;

	while (!is.eof()) {

		getline(is, line);

		if (line.size() < 3)
			continue;
		v = tokenizeToString(line, "\t");
		if (v.size() < 2) {
			std::cerr << line << std::endl;
			throw Exception(
					"ERROR in getSampleNameToPhenotypeMapping: invalid line format!\n");
		}
		dpm[v.at(0)] = atof(v.at(column).c_str());
	}
	return dpm;
}

bool find(std::string line, std::string key) {
	return (line.find(key) != std::string::npos);
}

// ----------------------------------------------------------------------------

/*
 DataSet DataParser::readNexus() {

 std::string line;
 std::vector<std::string> v;
 bool seenMatrix = false;

 SampleNameToCharactersMapping m;

 std::cerr << "    inserting the following sequence identifiers:\n";

 while (!this->curFile.eof() & !seenMatrix) {
 //std::cout << line << std::endl;
 getline(this->curFile, line);
 if (find(line, "MATRIX") || find(line, "Matrix")
 || find(line, "matrix")) {
 seenMatrix = true;
 while (!this->curFile.eof()) {
 getline(this->curFile, line);
 if (find(line, ";") || find(line, "end")) {
 break;
 }
 if (line.size() < 3)
 continue;
 v = tokenizeToString(line, "\t");
 if (v.size() < 2) {
 std::cerr << line << std::endl;
 throw Exception(
 "ERROR in readNucleotidesFromNexus: invalid line format!\n");
 }

 m[v.at(0)] = string2floatVector(v.at(1));
 }
 }
 }
 std::cerr << "\n";

 if (m.size() == 0) {
 throw Exception(
 "ERROR in DataParser::readNucleotidesFromNexus: empty map\n");
 }

 DataSet d(m);
 d.setType(Genetic);
 return d;

 }
 */

// ----------------------------------------------------------------------------
/*
 DataSet DataParser::readTable() {

 SampleNameType name;
 std::vector<float> values;

 getTableEntry(name, values);

 SampleNameToCharactersMapping cMap;

 while (name != NO_HEADER_SYM) {
 cMap[name] = values;
 getTableEntry(name, values);
 }

 DataSet d(cMap);
 d.setType(Continoous);
 return d;

 }
 */

// ----------------------------------------------------------------------------

map<string, string> DataParser::readFasta() {

	std::string header;
	std::string sequence;

	map<string, string> nMap;

	getFastaEntry(header, sequence);
	size_t sequenceLength = sequence.size();
	if (sequenceLength == 0) {
		throw Exception(
				"ERROR in DataParser::readFasta: empty (first) sequence");
	}

	while (header != NO_HEADER_SYM) {
		if (sequence.size() != sequenceLength) {
			Exception e(
					"ERROR in DataParser::readFasta: this is no alignment!\n");
			e << " expected size: ";
			e << sequenceLength;
			e << ",\n but size observed in header ";
			e << header;
			e << " is ";
			e << sequence.size();
			throw e;
		}
		//nMap[header] = string2floatVector(sequence);
		nMap[header] = sequence;
		getFastaEntry(header, sequence);
	}

	return nMap;
}

DistanceMatrix DataParser::readDistanceMatrix() {

	DistanceMatrix D;

	ifstream is(path.c_str());
	string line;

	/* Input should look like this:
	 * <empty>	Ac	Bc	Cc
	 * Ap	<numbers>
	 * Bp
	 * Cp
	 *
	 */

	if (!is.good()) {
		cerr << "ERROR";
		throw("error");
	}

	getline(is, line);
	vector<string> v = tokenizeToString(line);
	if (v.size() <= 1) {
		cerr << line << endl;
		throw Exception("ERROR in DataParser::readDistanceMatrix: invalid first line format");
	}

	//create new vector without the empty element
	vector<string> ids(v.begin(), v.end());

	string curId;
	size_t nr =1;
	while (!is.eof()) {
		getline(is, line);
		if (line.size() == 0) continue;

		v = tokenizeToString(line);

		if (v.size() == 0) {
			cerr << " empty vector \n";
			continue;
		}
		if (v.size() != ids.size() + 1) {
			cerr << "Error in line number " << nr << endl;
			cerr << "line was: \n" << line << endl;
			cerr << "expected number of tab-separated entries: " << ids.size() << ", observed " << v.size()
					<< endl;
			throw Exception("ERROR in DataParser::readDistanceMatrix: invalid content format");
		}
		curId = v[0];

		for (size_t i = 1; i < v.size(); i++) {
			D[curId][ids[i - 1]] = atof(v[i].c_str());
		}
		nr++;
	}

	return D;

}


void printDistanceMatrix(DistanceMatrix D) {
	DistanceMatrix::iterator iter;
		map<string, float>::iterator iter2;

		map<string, float> curLine;

		for (iter=D.begin(); iter!=D.end(); iter++) {
			cerr << iter->first << '\t';
			curLine = iter->second;
			for (iter2=curLine.begin(); iter2!=curLine.end(); iter2++) {
				cerr << iter2->second << '\t';
			}
			cerr << '\n';
		}

}
