/*
 * Exceptions.cpp
 *
 *  Created on: Sep 29, 2011
 *      Author: ctusche
 */

#include "Exception.h"

Exception::Exception() {};


Exception::Exception(const Exception& e) {
	this->ss << e.msg();
	error = e.msg();
};


std::string Exception::msg() const {
	return error;
}



