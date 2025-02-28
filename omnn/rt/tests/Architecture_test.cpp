// Architecture_test.cpp
#define BOOST_TEST_MODULE Architecture test
#include <boost/test/unit_test.hpp>

#include "../architecture.h"

using namespace omnn::rt;

BOOST_AUTO_TEST_CASE(Architecture_detection_test)
{
    // Get the current architecture
    const auto arch = GetCurrentArchitecture();
    
    // Verify that the architecture is not Unknown
    BOOST_TEST(arch != Architecture::Unknown);
    
    // Get the architecture name
    const char* archName = GetArchitectureName(arch);
    
    // Verify that the architecture name is not "Unknown"
    BOOST_TEST(std::string(archName) != "Unknown");
    
    // Test architecture check functions
    if (arch == Architecture::X86 || arch == Architecture::X86_64) {
        BOOST_TEST(IsX86Architecture());
        BOOST_TEST(!IsArmArchitecture());
    } else if (arch == Architecture::ARM || arch == Architecture::ARM64) {
        BOOST_TEST(!IsX86Architecture());
        BOOST_TEST(IsArmArchitecture());
    }
}
