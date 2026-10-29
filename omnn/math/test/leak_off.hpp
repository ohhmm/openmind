#pragma once
#include <boost/test/unit_test.hpp>
#include <boost/test/debug.hpp>
#ifdef _MSC_VER
struct OmnnMSVCLeakCheckOffFixture {
    OmnnMSVCLeakCheckOffFixture() { boost::debug::detect_memory_leaks(false); }
    ~OmnnMSVCLeakCheckOffFixture() {}
};
BOOST_TEST_GLOBAL_CONFIGURATION(OmnnMSVCLeakCheckOffFixture);
#endif
