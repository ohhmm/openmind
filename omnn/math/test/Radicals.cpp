#define BOOST_TEST_MODULE PrincipalSurd test
#include <boost/test/unit_test.hpp>

#include "PrincipalSurd.h"

#include "generic.hpp"


using namespace omnn::math;
using namespace boost::unit_test;
using namespace std;


BOOST_AUTO_TEST_CASE(PrincipalSurdOrder_test)
{
    DECL_VA(x);
    auto _1 = PrincipalSurd(2);
    auto _2 = PrincipalSurd(x + 2);
    InequalOrderCheck(_1, _2);
}

BOOST_AUTO_TEST_CASE(PrincipalSurd_test)
{
	auto _1 = Valuable(0.000144).sqrt();
	BOOST_TEST(_1 == 0.012);
    auto _2 = 12_v / 1000;
    BOOST_TEST(_1 == _2);

    _1 = -24;
    _1.sqrt();
    _2 = PrincipalSurd(6) * 2 * constants::i;
    BOOST_TEST(_1 == _2);

    _1 = PrincipalSurd(1);
    BOOST_TEST(_1 == constants::one);
    _1 = PrincipalSurd(2);
    _2 = PrincipalSurd(2);
    BOOST_TEST(_1 == _2);
    _1 = PrincipalSurd(3);
    _2 = PrincipalSurd(3);
    BOOST_TEST(_1 == _2);
    _1 = PrincipalSurd(2) * PrincipalSurd(3);
    _2 = PrincipalSurd(6);
    BOOST_TEST(_1 == _2);
    _1 = PrincipalSurd(2) / PrincipalSurd(3);
    _2 = PrincipalSurd(2) / PrincipalSurd(3);
    BOOST_TEST(_1 == _2);
    _1 = PrincipalSurd(2) / 3;
    _2 = PrincipalSurd(2) / 3;
    BOOST_TEST(_1 == _2);
    _1 = PrincipalSurd(2) / 3;
    _2 = PrincipalSurd(2) / 3;
    BOOST_TEST(_1 == _2);
    _1 = PrincipalSurd(2) + 3;
    _2 = PrincipalSurd(2) + 3;
    BOOST_TEST(_1 == _2);
    _1 = PrincipalSurd(2) - 3;
    _2 = PrincipalSurd(2) - 3;
    BOOST_TEST(_1 == _2);
    _1 = PrincipalSurd(2) / 3;
    _2 = PrincipalSurd(2) / 3;
    BOOST_TEST(_1 == _2);
    _1 = PrincipalSurd(2) + 3;
    _2 = PrincipalSurd(2) + 3;
    BOOST_TEST(_1 == _2);
    _1 = PrincipalSurd(2) - 3;
    _2 = PrincipalSurd(2) - 3;
    BOOST_TEST(_1 == _2);
    _1 = PrincipalSurd(2) ^ 3;
    _2 = PrincipalSurd(8);
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
