#define BOOST_TEST_MODULE School test
#include <boost/test/unit_test.hpp>
#include "Sum.h"
#include "Variable.h"


using namespace omnn::math;
using namespace boost::unit_test;

BOOST_AUTO_TEST_CASE(Perimetr)
{
    std::cout <<"zadacha_perimetr" <<std::endl;
    Variable x, P, a, b;
    auto formula = (a+b)*2 - P;
    formula.eval({
        { P, 20 },
        { a, x },
        { b, 6 }
    });
    
    auto solutions = formula.Solutions();
    for(const Valuable& solution : solutions){
        std::cout << solution.str() << std::endl;
        formula.Eval(x, solution);
        BOOST_TEST(formula == 0);
    }
    
    BOOST_TEST(solutions.size() == 1);
    BOOST_TEST(solutions.begin()->str() == "4");
}
