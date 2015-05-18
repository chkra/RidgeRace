/*
 * Random.h
 *
 *  Created on: Nov 8, 2012
 *      Author: ctusche
 */

#ifndef RANDOM_H_
#define RANDOM_H_

#include <cmath>
#include "Exception.h"

// source: http://www.dreamincode.net/code/snippet1446.htm
double randNormal(double mu = 0.0, double sigma = 1.0) {

	if (sigma < 0) {
		throw Exception("ERROR in randNormal(..): invalid sigma!");
	}

	static bool deviateAvailable = false; //	flag
	static float storedDeviate; //	deviate from previous calculation
	double polar, rsquared, var1, var2;

	//	If no deviate has been stored, the polar Box-Muller transformation is
	//	performed, producing two independent normally-distributed random
	//	deviates.  One is stored for the next round, and one is returned.
	if (!deviateAvailable) {

		//	choose pairs of uniformly distributed deviates, discarding those
		//	that don't fall within the unit circle
		do {
			var1 = 2.0 * (double(rand()) / double(RAND_MAX)) - 1.0;
			var2 = 2.0 * (double(rand()) / double(RAND_MAX)) - 1.0;
			rsquared = var1 * var1 + var2 * var2;
		} while (rsquared >= 1.0 || rsquared == 0.0);

		//	calculate polar tranformation for each deviate
		polar = sqrt(-2.0 * log(rsquared) / rsquared);

		//	store first deviate and set flag
		storedDeviate = var1 * polar;
		deviateAvailable = true;

		//	return second deviate
		return var2 * polar * sigma + mu;
	}

	//	If a deviate is available from a previous call to this function, it is
	//	returned, and the flag is set to false.
	else {
		deviateAvailable = false;
		return storedDeviate * sigma + mu;
	}
}

#endif /* RANDOM_H_ */
