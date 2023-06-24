#define BOOST_TEST_MODULE Fastest test
#include <boost/test/unit_test.hpp>
#include "fastest.hpp"


using namespace omnn::rt;

BOOST_AUTO_TEST_CASE(GetTheFastestResult_test) {
    TheFastestResult<int> wrapper(
        [] {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            return 42;
        },
        [] {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            return 7;
        });

    std::future<int> result_future = wrapper;
    int result = result_future.get();
    std::cout << "Result: " << result << std::endl;
    BOOST_TEST(result == 7);
}