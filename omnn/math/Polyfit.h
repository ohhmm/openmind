#pragma once
#include <omnn/rt/custom_allocator.hpp>

#ifdef BOOST_UBLAS_TYPE_CHECK
#	undef BOOST_UBLAS_TYPE_CHECK
#endif
#define BOOST_UBLAS_TYPE_CHECK 0
#ifndef _USE_MATH_DEFINES
#	define _USE_MATH_DEFINES
#endif

#include <iomanip>
#include <stdexcept>
#include <limits>
#include <numeric>
#include <vector>

#include <boost/algorithm/string/replace.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>

/*
	Finds the coefficients of a polynomial p(x) of degree n that fits the data,
	p(x(i)) to y(i), in a least squares sense. The result p is a row vector of
	length n+1 containing the polynomial coefficients in incremental powers.

	param:
 oX				x axis values
 oY				y axis values
 nDegree		polynomial degree including the constant

	return:
 coefficients of a polynomial starting at the constant coefficient and
 ending with the coefficient of power to nDegree.

 */
template<typename T>
auto polyfit( const std::vector<T>& oX, const std::vector<T>& oY, size_t nDegree )
{
    using namespace boost::numeric::ublas;

    // Define a custom allocator type for the Boost Numeric Ublass types
    using custom_alloc_t = custom_allocator<T>;

    // Use the custom allocator for the matrix and vector types
    using matrix_t = matrix<T, basic_row_major<>, unbounded_array<T, custom_alloc_t>>;
    using vector_t = vector<T, unbounded_array<T, custom_alloc_t>>;

    // Define a custom allocator type for int
    using custom_int_alloc_t = custom_allocator<int>;

    // Use the custom allocator for the permutation_matrix type
    using permutation_matrix_t = permutation_matrix<int, unbounded_array<int, custom_int_alloc_t>>;
    if ( oX.size() != oY.size() )
        throw std::invalid_argument( "X and Y vector sizes do not match" );

    // more intuitive this way
    nDegree++;

    auto nCount = oX.size();
    matrix_t oXMatrix(nCount, nDegree);
    matrix_t oYMatrix(nCount, 1);
    // copy y matrix
    for (size_t i = 0; i < nCount; i++)
    {
        new (&oYMatrix(i, 0)) T(oY[i]);
    }

    // create the X matrix
    for (size_t nRow = 0; nRow < nCount; nRow++)
    {
        T nVal = 1.0f;
        for (size_t nCol = 0; nCol < nDegree; nCol++)
        {
            new (&oXMatrix(nRow, nCol)) T(nVal);
            nVal *= oX[nRow];
        }
    }

    // Scale input data
    T xMean = std::accumulate(oX.begin(), oX.end(), T(0)) / T(nCount);
    T xStd = std::sqrt(std::inner_product(oX.begin(), oX.end(), oX.begin(), T(0)) / T(nCount) - xMean * xMean);
    T yMean = std::accumulate(oY.begin(), oY.end(), T(0)) / T(nCount);
    T yStd = std::sqrt(std::inner_product(oY.begin(), oY.end(), oY.begin(), T(0)) / T(nCount) - yMean * yMean);

    std::cout << "Debug: xMean = " << xMean << ", xStd = " << xStd << ", yMean = " << yMean << ", yStd = " << yStd << std::endl;

    // Apply scaling to X matrix and Y vector
    for (size_t i = 0; i < nCount; ++i) {
        for (size_t j = 0; j < nDegree; ++j) {
            oXMatrix(i, j) = std::pow((oX[i] - xMean) / xStd, T(j));
        }
        oYMatrix(i, 0) = (oY[i] - yMean) / yStd;
    }

    // transpose X matrix
    matrix_t oXtMatrix(trans(oXMatrix));
    // multiply transposed X matrix with X matrix
    matrix_t oXtXMatrix(prec_prod(oXtMatrix, oXMatrix));
    // multiply transposed X matrix with Y matrix
    matrix_t oXtYMatrix(prec_prod(oXtMatrix, oYMatrix));

    // Add regularization to handle near-singular matrices
    const T lambda = T(1e-6); // Adjusted regularization parameter
    for (size_t i = 0; i < oXtXMatrix.size1(); ++i) {
        oXtXMatrix(i, i) += lambda * (T(1) + std::abs(oXtXMatrix(i, i)));
    }

    std::cout << "Debug: Regularization parameter lambda = " << lambda << std::endl;

    // Solve the system using LU decomposition
    permutation_matrix<std::size_t> pm(oXtXMatrix.size1());
    lu_factorize(oXtXMatrix, pm);
    lu_substitute(oXtXMatrix, pm, oXtYMatrix);

    std::cout << "Debug: Coefficients:" << std::endl;
    for (size_t i = 0; i < oXtYMatrix.size1(); ++i) {
        std::cout << oXtYMatrix(i, 0) << " ";
    }
    std::cout << std::endl;

    // Check for near-zero determinant
    T det = T(1);
    for (size_t i = 0; i < oXtXMatrix.size1(); ++i) {
        det *= oXtXMatrix(i, i);
    }
    std::cout << "Debug: Determinant after regularization: " << det << std::endl;

    // Check for singularity after regularization
    det = 1;
    for (size_t i = 0; i < oXtXMatrix.size1(); ++i) {
        det *= oXtXMatrix(i, i);
    }
    std::cout << "Debug: Determinant after regularization: " << det << std::endl;

    // Check input data
    std::cout << "Debug: Input data (X, Y):" << std::endl;
    for (size_t i = 0; i < nCount; ++i) {
        std::cout << "(" << oX[i] << ", " << oY[i] << ") ";
    }
    std::cout << std::endl;

    // Scale input data
    T x_mean = std::accumulate(oX.begin(), oX.end(), T(0)) / nCount;
    T x_std = std::sqrt(std::inner_product(oX.begin(), oX.end(), oX.begin(), T(0)) / nCount - x_mean * x_mean);
    T y_mean = std::accumulate(oY.begin(), oY.end(), T(0)) / nCount;
    T y_std = std::sqrt(std::inner_product(oY.begin(), oY.end(), oY.begin(), T(0)) / nCount - y_mean * y_mean);

    std::vector<T> scaled_oX(oX), scaled_oY(oY);
    for (size_t i = 0; i < nCount; ++i) {
        scaled_oX[i] = (oX[i] - x_mean) / x_std;
        scaled_oY[i] = (oY[i] - y_mean) / y_std;
    }

    // Tikhonov regularization parameter
    // lambda is already declared earlier, so we remove the redeclaration here

    // Add regularization term to oXtXMatrix
    for (size_t i = 0; i < oXtXMatrix.size1(); ++i) {
        oXtXMatrix(i, i) += lambda;
    }
    // lu decomposition
    permutation_matrix_t pert(oXtXMatrix.size1());
    auto singular = lu_factorize(oXtXMatrix, pert);
    std::cout << "Debug: LU factorization result: " << singular << std::endl;

    // Check matrix after LU decomposition
    std::cout << "Debug: oXtXMatrix after LU decomposition:" << std::endl;
    for (size_t i = 0; i < oXtXMatrix.size1(); ++i) {
        for (size_t j = 0; j < oXtXMatrix.size2(); ++j) {
            std::cout << oXtXMatrix(i, j) << " ";
        }
        std::cout << std::endl;
    }

    // must not be singular
    BOOST_ASSERT( singular == 0 );

    std::cout << "Debug: oXtYMatrix before substitution:" << std::endl;
    for (size_t i = 0; i < oXtYMatrix.size1(); ++i) {
        std::cout << oXtYMatrix(i, 0) << " ";
    }
    std::cout << std::endl;
    // backsubstitution
    lu_substitute(oXtXMatrix, pert, oXtYMatrix);

    std::cout << "Debug: Resulting coefficients:" << std::endl;
    for (size_t i = 0; i < oXtYMatrix.size1(); ++i) {
        std::cout << oXtYMatrix(i, 0) << " ";
    }
    std::cout << std::endl;

    // Unscale the coefficients
    for (size_t i = 0; i < oXtYMatrix.size1(); ++i) {
        T x_std_power = T(1);
        for (size_t j = 0; j < i; ++j) {
            x_std_power *= x_std;
        }
        oXtYMatrix(i, 0) = oXtYMatrix(i, 0) * y_std / x_std_power;
    }
    // copy the result to coeff
    return std::vector<T>( oXtYMatrix.data().begin(), oXtYMatrix.data().end() );
}

/*
	Calculates the value of a polynomial of degree n evaluated at x. The input
	argument pCoeff is a vector of length n+1 whose elements are the coefficients
	in incremental powers of the polynomial to be evaluated.

	param:
 oCoeff			polynomial coefficients generated by polyfit() function
 oX				x axis values

	return:
 Fitted Y values.
 */
template<typename T>
auto polyval( const std::vector<T>& oCoeff, const std::vector<T>& oX )
{
    auto nCount =  oX.size();
    auto nDegree = oCoeff.size();
    std::vector<T>	oY( nCount );

    for ( size_t i = 0; i < nCount; i++ )
    {
        T nY = 0;
        T nXT = 1;
        T nX = oX[i];
        for ( size_t j = 0; j < nDegree; j++ )
        {
            // multiply current x by a coefficient
            nY += oCoeff[j] * nXT;
            // power up the X
            nXT *= nX;
        }
        oY[i] = std::move(nY);
    }

    return oY;
}


template <class T>
std::string polystr(const std::vector<T>& coeff)
{
    int p = 0;
    std::stringstream s;
    s << std::setprecision(100) << 0;
    for (auto& c: coeff) {
        s << '+';
        s << '(' << c << ')';
        s << "*x^" << p++;
    }
    return boost::replace_all_copy(s.str(), "e", "*10^");
}
