/*
 * SetsAndMaps.h
 *
 *  Created on: Sep 29, 2011
 *      Author: ctusche
 */

#ifndef SETSANDMAPS_H_
#define SETSANDMAPS_H_


#include <set>
#include <map>
#include <vector>
#include <iostream>
#include <cmath>
#include <cstdlib>


template<class T>
std::set<T> getIntersection(std::set<T> a, std::set<T> b) {
	typename std::set<T>::iterator iter;
	std::set<T> res;
	for (iter = a.begin(); iter != a.end(); iter++) {
		if (b.find(*iter) != b.end()) {
			res.insert(*iter);
		}
	}
	return res;
}

template<class T>
std::set<T> getUnion(std::set<T> a, std::set<T> b) {
	typename std::set<T>::iterator iter;
	std::set<T> res = a;
	for (iter = b.begin(); iter != b.end(); iter++) {
		res.insert(*iter);
	}
	return res;
}




template<class T>
std::set<T> vec2set(std::vector<T> a) {
	typename std::vector<T>::iterator iter;
	std::set<T> s;
	for (iter = a.begin(); iter != a.end(); iter++) {
		s.insert(*iter);
	}
	return s;
}


template<class T>
std::vector<T> set2vec(std::set<T> a) {
	typename std::set<T>::iterator iter;
	std::vector<T> s;
	for (iter = a.begin(); iter != a.end(); iter++) {
		s.push_back(*iter);
	}
	return s;
}



template<class T, class S>
std::map<S, T> getInverseMap(std::map<T, S> a) {

	typename std::map<T, S>::iterator iter;
	std::map<S, T> iMap;

	for (iter = a.begin(); iter != a.end(); iter++) {
		iMap[iter->second] = iter->first;
	}

	return iMap;
}

template<class T, class S>
void printKeysOfMap(std::map<T, S> a) {

	typename std::map<T, S>::iterator iter;
	std::map<S, T> iMap;

	for (iter = a.begin(); iter != a.end(); iter++) {
		std::cout << iter->first << std::endl;
	}

}


template<class T>
T getMostAbundant(std::map<T, size_t> a) {
	std::map<size_t, T> iA = getInverseMap(a);

	// 'a' contains T -> count mapping
	// iA contains all Ts ordered by count
	// rbegin is largest count
	return iA.rbegin()->second;
}


template<class T, class S>
bool contains(std::map<T, S> m, T t) {
	return (m.find(t) != m.end());
}

template<class T>
bool contains(std::set<T> s, T t) {
	return (s.find(t) != s.end());
}

struct BasicStats {
	float mean;
	float std;
	float max;
	float min;
};

BasicStats getBasicStats(std::vector<float> v);

BasicStats getAbsMeanAndStd(std::vector<float> v);


template<class T>
std::map<T, BasicStats> getMeanAndStd(std::map<T, std::vector<float> > a) {

	typename std::map<T, std::vector<float> >::iterator iter;
	std::map<T, BasicStats> res;

	T t;
	BasicStats ms;
	for (iter = a.begin(); iter != a.end(); iter++) {
		t = iter->first;
		ms = getMeanAndStd(iter->second);
		res[t] = ms;
	}

	return res;
}

template<class T>
int getRandomElement(const std::set<T>& s) {
  double r = rand() % s.size();
  typename std::set<T>::iterator it = s.begin();
  for (; r != 0; r--) it++;
  return *it;
}


#endif /* SETSANDMAPS_H_ */
