#define BOOST_TEST_MODULE TS test
#include <boost/test/unit_test.hpp>
#include "Sum.h"
#include "Variable.h"


using namespace omnn::math;
using namespace boost::unit_test;


BOOST_AUTO_TEST_CASE(TS)
{
	Variable time, price;
	auto points[] = {
		{0,0},
		{1,5},
		{3,8},
	};
	auto bits = 2;

	Variable path;
	Variable move1, move2, move3;

	auto uni = path.equals((move1 << (2 * 2)) + (move2 << 2) + move3);


}
