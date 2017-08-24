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

BOOST_AUTO_TEST_CASE(Polyfit_test_sinus)
{
    const int SZ = 73;
    const f128  PI2 = 6.283185307179586476925286766559,
                SZf = SZ,
                TestPrecision = 0.0000001;
    
    
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
        f128 s = sin( (f128)i/SZf * PI2 );
        BOOST_TEST(fabs(s-oFittedY[i]) < TestPrecision);
    }
}

BOOST_AUTO_TEST_CASE(Polyfit_test_verticals)
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
    const f128 TestPrecision = 0.0000001;

    // polynomial fitting
    std::vector<f128> oCoeff = polyfit( oX, oY, SZ );
    std::vector<f128> oFittedY = polyval( oCoeff, oX );
    for ( unsigned int i = 0; i < SZ; i++ )
    {
        auto r = round(oFittedY[i]);
        auto diff = oY[i]-r;
        auto absolute = fabs(diff);
        auto fit = oY[i] == oFittedY[i];
        BOOST_TEST(absolute < TestPrecision);
    }
}
