/*
 * RidgeSolver.h
 *
 *  Created on: Sep 27, 2013
 *      Author: ctusche
 */

#ifndef RIDGESOLVER_H_
#define RIDGESOLVER_H_

#define BOOST_UBLAS_NDEBUG 1

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>
#include "storage_adaptors.hpp"

//using namespace boost::numeric::ublas;
using namespace std;

typedef boost::numeric::ublas::vector<float> uvector;

typedef boost::numeric::ublas::matrix<float> umatrix;


/* Matrix inversion routine.
Uses lu_factorize and lu_substitute in uBLAS to invert a matrix */
template<class T>
bool InvertMatrix(const boost::numeric::ublas::matrix<T>& input, boost::numeric::ublas::matrix<T>& inverse)
{
	typedef boost::numeric::ublas::permutation_matrix<std::size_t> pmatrix;

	// create a working copy of the input
	boost::numeric::ublas::matrix<T> A(input);

	// create a permutation matrix for the LU-factorization
	pmatrix pm(A.size1());

	// perform LU-factorization
	int res = lu_factorize(A, pm);
	if (res != 0)
		return false;

	// create identity matrix of "inverse"
	inverse.assign(boost::numeric::ublas::identity_matrix<T> (A.size1()));

	// backsubstitute to get the inverse
	lu_substitute(A, pm, inverse);

	A.resize(0,0,false);
	pm.resize(0,false);

	return true;
}


// ############################################################################


class RegressionResult {

public:
	uvector _beta;
	uvector _yEst;
	float _sqError;

	RegressionResult(uvector beta, uvector yEst, float sqError);
	RegressionResult();

	void print(ostream &target=std::cout);

	~RegressionResult() {
		_beta.resize(0,false);
		_yEst.resize(0,false);
	};
};

// ############################################################################

class RidgeRegressionResult : public RegressionResult {

public:

	float _lambda;

	RidgeRegressionResult();
	RidgeRegressionResult(uvector beta, uvector yEst, float sqError, float lambda);

	void print(ostream &target=std::cout);

};


// ############################################################################

class RidgeRegression {

public:

	boost::numeric::ublas::matrix<float> rrX;
	boost::numeric::ublas::matrix<float> rrXt;
	uvector rrY;
	float rrlambda;

	RidgeRegression(boost::numeric::ublas::matrix<float> X, uvector Y, float lambda=0.0001);

	RidgeRegressionResult solve();

	~RidgeRegression() {
		rrX.resize(0,0,false);
		rrXt.resize(0,0,false);
		rrY.resize(0, false);
	}

};

#endif /* RIDGESOLVER_H_ */
