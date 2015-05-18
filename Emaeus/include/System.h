/*
 * System.h
 *
 *  Created on: Sep 29, 2011
 *      Author: ctusche
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <string>
#include <stddef.h>

using namespace std;




// ============================================================================

/**
 * A progress bar.
 */
class Progresser {

private:
	size_t curCount;
	size_t total;
	float ratio;

	void print2screen();

public:

	/**
	 * Creates a progress bar and starts it with 0%.
	 * @param total the number that corresponds to 100%
	 */
	Progresser(size_t total);

	//-------------------------------------------------------------------------

	/**
	 * Update the progress bar.
	 * @param current the number that corresponds to the current
	 * progress.
	 */
	void print(size_t current);

	//-------------------------------------------------------------------------

	/** increases the current count by one and updates the progress bar.
	 */
	void print();

	//-------------------------------------------------------------------------

	/**
	 * Set the progress bar to 100%.
	 */
	void finish();
};

int cmd(std::string cmdstr);

// ============================================================================

#endif /* SYSTEM_H_ */
