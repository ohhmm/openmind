#define BOOST_TEST_MODULE Fraction test
#include <boost/test/unit_test.hpp>

#include "Fraction.h"

BOOST_AUTO_TEST_CASE(Fraction_tests)
{
    using f_t = omnn::extrapolator::Fraction;
    const f_t a (1, 2);
	f_t c(3, 1);
    f_t b = a * 4;
	f_t d(2, 4);
    
    BOOST_TEST(a*b==f_t(1,1));
	BOOST_TEST((c += b) == f_t(5,1));
	BOOST_TEST((c *= a) == f_t(5,2));
	BOOST_TEST((c /= a) == f_t(5,1));
	BOOST_TEST((c--) == f_t(5,1));
	BOOST_TEST((c++) == f_t(4,1));
	BOOST_TEST((c>a) == true);
	BOOST_TEST((a==d) == true);

}
