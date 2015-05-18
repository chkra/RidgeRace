/*
 * StringManipulation.cpp
 *
 *  Created on: Sep 29, 2011
 *      Author: ctusche
 */

#include "StringManipulation.h"
#include <vector>
#include <stdlib.h>



std::vector<float> tokenizeToFloat(const std::string& str,
		const std::string& delimiters) {

	std::vector<float> tokens;
	// Skip delimiters at beginning.
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	std::string::size_type pos = str.find_first_of(delimiters, lastPos);
	std::string tmp;



	while (std::string::npos != pos || std::string::npos != lastPos) {
		// Found a token, add it to the vector.
		tmp = str.substr(lastPos, pos - lastPos);
		//if (!first) {
		tokens.push_back(atof(tmp.c_str()));
		//} else {
		//	first = false;
		//}
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}

	return tokens;
}

std::vector<int> tokenizeToInt(const std::string& str,
		const std::string& delimiters) {

	std::vector<int> tokens;
	// Skip delimiters at beginning.
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	std::string::size_type pos = str.find_first_of(delimiters, lastPos);
	std::string tmp;

	//bool first = true;

	while (std::string::npos != pos || std::string::npos != lastPos) {
		// Found a token, add it to the vector.
		tmp = str.substr(lastPos, pos - lastPos);
		//if (!first) {
		tokens.push_back(atoi(tmp.c_str()));
		//} else {
		//	first = false;
		//}
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}

	return tokens;
}

std::vector<std::string> tokenizeToString(const std::string& str,
		const std::string& delimiters) {

	std::vector<std::string> tokens;
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	std::string::size_type pos = str.find_first_of(delimiters, lastPos);
	std::string tmp;

	if (std::string::npos == pos || std::string::npos == lastPos) {
		tokens.push_back(str);
		return tokens;
	}

	while (std::string::npos != pos || std::string::npos != lastPos) {
		tmp = str.substr(lastPos, pos - lastPos);
		tokens.push_back(tmp);
		lastPos = str.find_first_not_of(delimiters, pos);
		pos = str.find_first_of(delimiters, lastPos);
	}

	return tokens;
}


std::vector<float> string2floatVector(std::string s) {

	std::vector<float> res;
	for (size_t i=0; i<s.length(); i++) {
		res.push_back(s.at(i));
	}
	return res;

}

bool equal(string s1, string s2) {
	return s1.compare(s2)==0;
}
