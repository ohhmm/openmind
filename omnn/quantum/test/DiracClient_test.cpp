#define BOOST_TEST_MODULE DiracClient test
#include <omnn/quantum/DiracClient.h>

#include <boost/test/unit_test.hpp>

#include <cstdlib>
#include <iostream>

BOOST_AUTO_TEST_CASE (constructs_when_secrets_are_available) {
    if (!std::getenv("QCI_TOKEN") || !std::getenv("QCI_API_URL")) {
        std::cout << "Skipping DiracClient construction test: QCI_TOKEN/QCI_API_URL not set\n";
        return;
    }

    BOOST_REQUIRE_NO_THROW(omnn::quantum::DiracClient client);
}
