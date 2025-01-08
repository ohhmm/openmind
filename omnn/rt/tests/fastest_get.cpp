#define BOOST_TEST_MODULE Fastest test
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "fastest.hpp"
#include <atomic>

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

    static std::atomic_bool done = false;
    result = TheFastestResult<int>(
        [] {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            done = true; 
            return 42;
        },
        [] {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            return 7;
        });
    BOOST_TEST(done == false); // control returns once the fastest is done, other are expected to be still running
    std::cout << "Result: " << result << std::endl;
    BOOST_TEST(result == 7);
}
