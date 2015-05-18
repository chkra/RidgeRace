/*
 * Exceptions.h
 *
 *  Created on: Sep 29, 2011
 *      Author: ctusche
 */

#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_


#include <string>
#include <sstream>

class Exception {

private:
	std::ostringstream ss;
	std::string error;

public:


	Exception();
	Exception(const Exception& e);


	template <class T>
	Exception(std::string arg1, T arg2) {
		std::stringstream ss;
		ss << arg1;
		ss << arg2;
		error = ss.str();
	}

	template <class T>
	Exception(std::string arg1, T arg2[], size_t length) {
		std::stringstream ss;
		ss << arg1;
		for (size_t i=0; i<length; i++) {
			ss << arg2[i];
			ss << ' ';
		}

		error = ss.str();
	}



	Exception(std::string arg) {
		error = arg ;
	}

	std::string msg() const;

	template <typename T>
	void operator<<(const T& t) {
		this->ss << t;
		error = ss.str();
	}

};





#endif /* EXCEPTIONS_H_ */
