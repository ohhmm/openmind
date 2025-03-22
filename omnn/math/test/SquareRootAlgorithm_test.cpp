#define BOOST_TEST_MODULE SquareRootAlgorithm_test
#include <boost/test/unit_test.hpp>

#include <omnn/math/SquareRootAlgorithm.h>
#include <omnn/math/Integer.h>
#include <cmath>
#include <string>

using namespace omnn::math;

BOOST_AUTO_TEST_CASE(SquareRootAlgorithm_test)
{
    // Test cases with known square roots
    struct TestCase {
        a_int input;
        std::string expected_prefix; // First few digits of the expected result
        double expected_double;
    };
    
    std::vector<TestCase> testCases = {
        {4, "2", 2.0},
        {9, "3", 3.0},
        {16, "4", 4.0},
        {25, "5", 5.0},
        {100, "10", 10.0},
        {2685, "51.8", 51.8169},
        {12345, "111.1", 111.1080},
        {1000000, "1000", 1000.0}
    };
    
    for (const auto& test : testCases) {
        std::string result = SquareRootAlgorithm::calculate(test.input, 6);
        
        // Check that the result starts with the expected prefix
        BOOST_TEST(result.substr(0, test.expected_prefix.length()) == test.expected_prefix,
                  "Square root of " << test.input << " should start with " << test.expected_prefix);
        
        // Check that the result is close to the expected double value
        double resultDouble = std::stod(result);
        BOOST_TEST(std::abs(resultDouble - test.expected_double) < 0.001,
                  "Square root of " << test.input << " should be close to " << test.expected_double);
        
        // Compare with std::sqrt for validation
        double stdSqrt = std::sqrt(static_cast<double>(test.input));
        BOOST_TEST(std::abs(resultDouble - stdSqrt) < 0.001,
                  "Square root of " << test.input << " should match std::sqrt result");
    }
}
