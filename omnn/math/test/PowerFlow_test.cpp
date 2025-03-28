#define BOOST_TEST_MODULE PowerFlow test
#include <boost/test/unit_test.hpp>

#include "Variable.h"
#include "Sum.h"
#include "Product.h"
#include "System.h"
#include "generic.hpp"

using namespace omnn::math;
using namespace boost::unit_test;

BOOST_AUTO_TEST_CASE(PowerFlow_tests, *disabled())
{
    
    Variable V1, V2, V3;
    
    auto y12 = 0.1_v;
    auto y13 = 0.2_v;
    auto y23 = 0.15_v;
    
    auto Y11 = y12 + y13;
    auto Y22 = y12 + y23;
    auto Y33 = y13 + y23;
    
    auto Y12 = -y12;
    auto Y13 = -y13;
    auto Y23 = -y23;
    
    BOOST_TEST(Y12 == Y21); // Symmetry property
    
    
    auto V1_val = 1.0_v;
    auto V2_val = 0.95_v;
    auto V3_val = 1.05_v;
    
    auto I1 = V1_val * Y11 + Y12 * V2_val + Y13 * V3_val;
    
    auto S1 = V1_val * I1;
    
    BOOST_TEST((S1 == V1_val * (V1_val * Y11 + Y12 * V2_val + Y13 * V3_val)));
}
