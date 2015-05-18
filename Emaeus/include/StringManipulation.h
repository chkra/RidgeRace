/*
 * StringManipulation.h
 *
 *  Created on: Sep 29, 2011
 *      Author: ctusche
 */

#ifndef STRINGMANIPULATION_H_
#define STRINGMANIPULATION_H_

#include <sstream>
#include <string>
#include <vector>

using namespace std;

std::vector<float> tokenizeToFloat(const std::string& str,
		const std::string& delimiters = "\t ");
std::vector<int> tokenizeToInt(const std::string& str,
		const std::string& delimiters = "\t ");
std::vector<std::string> tokenizeToString(const std::string& str,
		const std::string& delimiters = "\t ");


template<class T>
std::string toString(T t) {
	std::ostringstream os;
	os << t;
	return os.str();
}


/**
 * Takes a string, converts every character in it to its according integer
 * asccii value, stores that value as a float.
 */
std::vector<float> string2floatVector(std::string s);

struct lessSize {
  bool operator()(const std::string & a, const std::string & b) const {
	if (a.size() < b.size()) return true;
	else if (a.size() > b.size()) return false;
	else return a < b;
  }
};


bool equal(string s1, string s2);



#endif /* STRINGMANIPULATION_H_ */
