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
#include "pi.h"
#include "SymmetricDouble.h"
#include "Polyfit.h"

using namespace std;
using namespace boost::unit_test;
using namespace omnn::math;

using f128 = Valuable;

const f128 TestPrecision = 0.000001;


BOOST_AUTO_TEST_CASE(Polyfit_test_sinus, *tolerance(TestPrecision))
{
    const int SZ = 73;
    const f128  PI2 = constant::pi * 2,
                SZf = SZ;
    
    // generate the data
    std::vector<f128> oX(SZ), oY(SZ);
    for (auto i = 0; i < SZ; ++i)
    {
        oX[i] = i;
        oY[i] = (PI2 * i / SZf).Sin();
    }
    
    // polynomial fitting
    auto oCoeff = polyfit( oX, oY, SZ );
    auto oFittedY = polyval( oCoeff, oX );
    auto s = polystr(oCoeff);
    std::cout << s << std::endl;
    for ( auto i = 0; i < SZ; i++ )
    {
        BOOST_TEST(oY[i]==oFittedY[i]);
    }
}

BOOST_AUTO_TEST_CASE(Polyfit_test_classification, *tolerance(TestPrecision))
{
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
        000,
        // verticals
        100,
        100,
        // horizontals
        010,
        010,
        // diagonals
        001,
        001,
    }};
    BOOST_TEST(oX.size() == oY.size());
    auto SZ = oX.size();

    // polynomial fitting
    auto oCoeff = polyfit( oX, oY, SZ ),
        oFittedY = polyval( oCoeff, oX );
    for ( unsigned int i = 0; i < SZ; i++ )
    {
        BOOST_TEST(oY[i] == oFittedY[i]);
    }
    
    oX = {{
        0000,
        0001,
        0010,
        0011, // horizontal
        0100,
        0101, // vertical
        0110, // diagonal
        0111,
        1000,
        1001, // diagonal
        1010, // vertical
        1011,
        1100, // horizontal
        1101,
        1110,
        1111,
    }};
    oY = {{
        000,
        000,
        000,
        010, // horizontal
        000,
        100, // vertical
        001, // diagonal
        000,
        000,
        001, // diagonal
        100, // vertical
        000,
        010, // horizontal
        000,
        000,
        000,
    }};
    
    oFittedY = polyval( oCoeff, oX );
    for ( unsigned int i = 0; i < SZ; i++ )
    {
        BOOST_WARN_MESSAGE(oY[i] == oFittedY[i], "i");
    }
}

BOOST_AUTO_TEST_CASE(Polyfit_test_classification_symmetric_double, *disabled())
{
    std::vector<SymmetricDouble> oX = {{
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
    std::vector<SymmetricDouble> oY = {{
        000,
        // verticals
        100,
        100,
        // horizontals
        010,
        010,
        // diagonals
        001,
        001,
    }};
    BOOST_TEST(oX.size() == oY.size());
    auto SZ = oX.size();
    
    // polynomial fitting
    auto oCoeff = polyfit( oX, oY, SZ );
    auto oFittedY = polyval( oCoeff, oX );
    for ( unsigned int i = 0; i < SZ; i++ )
    {
        BOOST_TEST(oY[i] == oFittedY[i]);
    }
    
    oX = {{
        0000,
        0001,
        0010,
        0011, // horizontal
        0100,
        0101, // vertical
        0110, // diagonal
        0111,
        1000,
        1001, // diagonal
        1010, // vertical
        1011,
        1100, // horizontal
        1101,
        1110,
        1111,
    }};
    oY = {{
        000,
        000,
        000,
        010, // horizontal
        000,
        100, // vertical
        001, // diagonal
        000,
        000,
        001, // diagonal
        100, // vertical
        000,
        010, // horizontal
        000,
        000,
        000,
    }};
    
    oFittedY = polyval( oCoeff, oX );
    for ( unsigned int i = 0; i < SZ; i++ )
    {
        BOOST_WARN_MESSAGE(oY[i] == oFittedY[i], "i");
    }
}


BOOST_AUTO_TEST_CASE(Polyfit_test_full_classification
//, *tolerance(0.1) 
, *disabled()
)
{
    // generate the data
    std::vector<f128> oX = {{
        0000,
        0001,
        0010,
        0011, // horizontal
        0100,
        0101, // vertical
        0110, // diagonal
        0111,
        1000,
        1001, // diagonal
        1010, // vertical
        1011,
        1100, // horizontal
        1101,
        1110,
        1111,
    }};
    std::vector<f128> oY = {{
        000,
        000,
        000,
        010, // horizontal
        000,
        100, // vertical
        001, // diagonal
        000,
        000,
        001, // diagonal
        100, // vertical
        000,
        010, // horizontal
        000,
        000,
        000,
    }};
    BOOST_TEST(oX.size() == oY.size());
    auto SZ = oX.size();
    
    // polynomial fitting
    auto oCoeff = polyfit( oX, oY, SZ ),
        oFittedY = polyval( oCoeff, oX );
    for ( unsigned int i = 0; i < SZ; i++ )
    {
        BOOST_TEST(oY[i] == oFittedY[i]);
    }
}
