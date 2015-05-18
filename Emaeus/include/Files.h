/*
 * FileIO.h
 *
 *  Created on: Sep 29, 2011
 *      Author: ctusche
 */

#ifndef FILEIO_H_
#define FILEIO_H_


#include <string>
#include <iostream>
#include <vector>
#include <fstream>

using namespace std;


void checkNonExistence(std::string path);

void getUserInputAndWriteToFile(std::string path);

size_t estimateLines(string path, string skip="");


class Table {

private:
	std::vector< std::string > entries;
	void create( std::string sep );

public:

	std::vector< std::vector<std::string> > T;

	Table();

	Table(std::string path, std::string sep=" \t");
	void print(std::ostream &target=std::cout);
	unsigned long rows();
	std::vector< std::string > col(unsigned long long  k);
	std::vector< std::string > row(unsigned long long  k);


};


#endif /* FILEIO_H_ */
