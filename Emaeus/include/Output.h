/*
 * Output.h
 *
 *  Created on: Sep 29, 2011
 *      Author: ctusche
 */

#ifndef OUTPUT_H_
#define OUTPUT_H_

#include <map>
#include <vector>
#include <iostream>

using namespace std;

template <class T>
void printVectorLn(std::vector<T> a, std::ostream &target=std::cout) {

	for (unsigned long long i = 0; i<a.size(); i++) {
		target << a[i] << "\n";
	}

}

template <class T>
void printVector(std::vector<T> a, std::ostream &target=std::cout)  {

	for (unsigned long long i = 0; i<a.size(); i++) {
		target << a[i] << "\t";
	}

}

template<class T, class S>
void print(std::map<T, S> a);




#endif /* OUTPUT_H_ */
