/*
 * Files.cpp
 *
 *  Created on: Sep 29, 2011
 *      Author: ctusche
 */


#include "Files.h"
#include "Exception.h"
#include "StringManipulation.h"

void getUserInputAndWriteToFile(std::string path) {

	std::string line;

	checkNonExistence(path);

	std::getline(std::cin, line);
	std::ofstream os(path.c_str());
	os << line << '\n';
	os.close();

}


void checkNonExistence(std::string path) {
	std::fstream fin;
	fin.open(path.c_str(),std::ios::in);

	char c = '%';
	if( fin.is_open() ) {
		std::cout << "WARNING: " << path << " exists! Overwrite it? ";
		while ((c != 'y') & (c!='n')) {
			std::cout << "\n   [y/n] ";
		}
		if (c=='n') {
			fin.close();
			throw Exception("Program canceled by user.\n");
		}
	}
	fin.close();
}

size_t estimateLines(string path, string skip) {

	ifstream is(path.c_str());
	string line;
	size_t k=0;

	while (!is.eof()) {
		getline(is, line);
		if (line.size()==0) continue;
		if (skip.find(line[0]) != string::npos) continue;

		k++;
	}

	is.close();
	return k;
}


Table::Table(std::string path, std::string sep) {

	//unsigned long numLines = getNumLines(path);

	unsigned long numLines = 20;
	std::ifstream in;
	in.open(path.c_str());
	std::vector<std::string> parts;
	std::vector<std::vector<std::string> > res;


	if (!in.is_open()) {
		std::string msg("Table (constructor): unable to open file \n\t");
		msg.append(path);
		throw Exception(msg);
	}



	if (numLines > 1000000) {
		std::cout << "reading file " << path <<  "(" << numLines << " lines)" << std::endl;
	}



	std::string line;
	unsigned long k = 0;
	size_t pos;
	while (!in.eof()) {

		getline(in, line);

		//
		if (line[0] != '#') {

			k++;

			pos = line.rfind("  ");
			while (pos != std::string::npos) {
				line.erase(pos, 1);
				pos = line.rfind("  ");
			}
			parts = tokenizeToString(line, sep);
			res.push_back(parts);


		}
		//if (numLines > 1000000 & float(k)/float(numLines)>=curStep/100.0f ) {
		//	progressBar(curStep, 100);
		//	curStep++;
		//}
	}
//	if (f > 10000000) std::cout << std::endl;

	if (numLines > 1000000) std::cout << std::endl;
	this->T = res;
	//std::cout << T.size() << std::endl;
}

Table::Table() {

}

void Table::print(std::ostream &target) {

	for (size_t i=0; i<this->T.size(); i++) {
		for (size_t j=0; j<T[i].size(); j++) {
			target << T[i][j] << "\t";
		}
		target << std::endl;
	}

}



unsigned long Table::rows() {
	return this->T.size();
}

std::vector< std::string > Table::col(unsigned long long k) {

	std::vector< std::string > res;
	for (unsigned long long  i=0; i<this->T.size(); i++) {
		if (this->T[i].size() > k) {
			res.push_back( this->T[i][k]);
		}
	}
	return res;
}

std::vector< std::string > Table::row(unsigned long long  k) {
	return  this->T[k];
}


