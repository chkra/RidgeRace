/*
 * Output.cpp
 *
 *  Created on: Sep 29, 2011
 *      Author: ctusche
 */


#include "Output.h"
#include <iostream>

template<class T>
void printVector(std::vector<T> v) {
	for (unsigned int i = 0; i < v.size(); i++) {
		std::cout << v[i] << '\t';
	}
	std::cout << std::endl;
}


template<class T, class S>
void print(std::map<T, S> a) {

	typename std::map<T, S>::iterator iter;
	for (iter = a.begin(); iter != a.end(); iter++) {
		std::cout << iter->first << '\t' << iter->second << '\n';
	}
}
