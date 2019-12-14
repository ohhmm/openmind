#define BOOST_TEST_MODULE School test
#include <boost/test/unit_test.hpp>
#include "Sum.h"
#include "Variable.h"


using namespace omnn::math;
using namespace boost::unit_test;

BOOST_AUTO_TEST_CASE(Perimeter)
{
    std::cout
        << std::endl
        << "perimeter"
        << std::endl;
    
    DECL_VA(x);
    DECL_VA(P);
    DECL_VA(a);
    DECL_VA(b);

    auto formula = (a+b)*2 - P;
    std::cout <<"formula: " << formula.str() << " = 0" << std::endl;
    
    formula.eval({
        { P, 20 },
        { a, x },
        { b, 6 }
    });
    std::cout <<"evaluated: " << formula.str() << " = 0" << std::endl;
    
    auto root = formula(x);
    std::cout <<"x = " << root << std::endl;
    
    BOOST_TEST(root == 4);
    
    formula.eval({ { x, root }});
    std::cout << "check: " << formula << " = 0" << std::endl;
    BOOST_TEST(formula == 0);
}
