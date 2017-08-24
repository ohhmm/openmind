/*
 * Polyfit_test.cpp
 *
 *  Created on: 23 авг. 2017 г.
 *      Author: sergejkrivonos
 */
//#define BOOST_MP_USE_FLOAT128
//#include <boost/multiprecision/float128.hpp>
#define BOOST_TEST_MODULE Polyfit test
#include <boost/test/unit_test.hpp>
#include "Polyfit.h"

using namespace std;
using f128 = long double;
using namespace boost::unit_test;

const f128 TestPrecision = 0.000001;


BOOST_AUTO_TEST_CASE(Polyfit_test_sinus, *tolerance(TestPrecision))
{
    const int SZ = 73;
    const f128  PI2 = 6.283185307179586476925286766559,
                SZf = SZ;
    
    
    // generate the data
    std::vector<f128> oX(SZ);
    std::vector<f128> oY(SZ);
    for ( unsigned int i = 0; i < SZ; i++ )
    {
        oX[i] = i;
        oY[i] = sin( (f128)i/SZf * PI2 );
    }
    
    // polynomial fitting
    std::vector<f128> oCoeff = polyfit( oX, oY, SZ );
    std::vector<f128> oFittedY = polyval( oCoeff, oX );
    for ( unsigned int i = 0; i < SZ; i++ )
    {
        BOOST_TEST(oY[i]==oFittedY[i]);
    }
}

BOOST_AUTO_TEST_CASE(Polyfit_test_classification, *tolerance(TestPrecision))
{
    // generate the data
    std::vector<f128> oX = {{
        0000,
        // verticals
        1010,
        0101,
        // horizontals
        1100,
        0011,
        // diagonals
        0110,
        1001,
    }};
    std::vector<f128> oY = {{
        0,
        // verticals
        100,
        100,
        // horizontals
        10,
        10,
        // diagonals
        1,
        1,
    }};
    BOOST_TEST(oX.size() == oY.size());
    auto SZ = oX.size();

    // polynomial fitting
    std::vector<f128> oCoeff = polyfit( oX, oY, SZ );
    std::vector<f128> oFittedY = polyval( oCoeff, oX );
    for ( unsigned int i = 0; i < SZ; i++ )
    {
        BOOST_TEST(oY[i] == oFittedY[i]);
    }
}
