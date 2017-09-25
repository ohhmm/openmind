/*
 * VarHost_test.cpp
 *
 *  Created on: 25 авг. 2017 г.
 *      Author: sergejkrivonos
 */

#include "VarHost.h"

#define BOOST_TEST_MODULE Varhost test
#include <boost/test/unit_test.hpp>
#include "SymmetricDouble.h"
#include "Polyfit.h"

using namespace std;
using namespace boost::unit_test;
using namespace omnn::extrapolator;


BOOST_AUTO_TEST_CASE(Varhost_test)
{
	VarHost<int> vars_int {{ 0, 1 }};
	BOOST_TEST(vars_int.size() == 2);
	VarHost<std::string> vars {{ "x1", "x2" }};
	BOOST_TEST(vars.size() == 2);
}

