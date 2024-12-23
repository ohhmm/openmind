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

    _1 = -24;
    _1.sqrt();
    _2 = PrincipalSurd(6) * 2 * constants::i;
    BOOST_TEST(_1 == _2);
}

BOOST_AUTO_TEST_CASE(RadicalExpressions_noHang_test
    , *timeout(2)
    )
{
    Valuable _1("((sqrt(111))*((-6)/37) + ((-35)/((sqrt(111))*2)))");
    Valuable _2("((-17)/111)*sqrt(111)");
    auto _ = _1 + _2;
}
