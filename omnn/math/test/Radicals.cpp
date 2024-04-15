#define BOOST_TEST_MODULE PrincipalSurd test
#include <boost/test/unit_test.hpp>

#include "PrincipalSurd.h"


using namespace omnn::math;
using namespace boost::unit_test;
using namespace std;

BOOST_AUTO_TEST_CASE(PrincipalSurd_test) {
	auto _1 = Valuable(0.000144).sqrt();
	BOOST_TEST(_1 == 0.012);
    auto _2 = 12_v / 1000;
    BOOST_TEST(_1 == _2);
}
