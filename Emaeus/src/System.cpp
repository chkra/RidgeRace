/*
 * System.cpp
 *
 *  Created on: Sep 29, 2011
 *      Author: ctusche
 */

#include "System.h"
#include <stdio.h>
#include <stdlib.h>




Progresser::Progresser(size_t total) {
	this->total = total;
	this->curCount = 0;
}


void Progresser::print2screen() {
	size_t j;

	float ratio;
	size_t nTotal = this->total;
	size_t nCurCount = this->curCount;

	if (nTotal > 60) {
		ratio = float(total) / 60.0;
		nTotal = 60;
		nCurCount = nCurCount / ratio;
	}

	fprintf(stderr, "  [");
	for (j = 0; j < nCurCount; j++)
		fprintf(stderr, "=");

	for (; j < nTotal; j++)
		fprintf(stderr, " ");

	fprintf(stderr, "]");

	size_t cur = (size_t) float(nCurCount) / float(nTotal) * 100.0f;
	fprintf(stderr, "%3d%%", (int) cur);

	fprintf(stderr, "\r");
	fflush(stderr);


	if (nCurCount == nTotal) {
		fprintf(stderr, "  [=======       completed       ");
		fprintf(stderr, "\n");
	}
}

//-------------------------------------------------------------------------


void Progresser::print(size_t current) {

	this->curCount = current;
	this->print2screen();
}

//-------------------------------------------------------------------------

/** increases the current count by one and updates the progress bar.
 */
void Progresser::print() {
	this->curCount ++;
	this->print2screen();
}

//-------------------------------------------------------------------------


void Progresser::finish() {
	this->curCount = this->total;
	this->print2screen();
}





