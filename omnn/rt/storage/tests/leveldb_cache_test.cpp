#define BOOST_TEST_MODULE leveldb_cache_test
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include "../CacheBase.h"
#include "../LevelDbCache.h"

#ifdef OPENMIND_STORAGE_LEVELDB

BOOST_AUTO_TEST_SUITE(leveldb_cache_suite)

using namespace omnn::rt::storage;

BOOST_AUTO_TEST_CASE(LevelDbStorage_test) {
    LevelDbCache storage("testLevelDb");
    storage.Set("key", "value");
    auto got = storage.GetOne("key");
    BOOST_TEST(got == "value");
    BOOST_CHECK(storage.ResetAllDB(""));
    BOOST_CHECK_EQUAL(storage.GetOne("key"), "");
}

BOOST_AUTO_TEST_SUITE_END()

#endif // OPENMIND_STORAGE_LEVELDB

bool init_unit_test() {
    return true;
}

int main(int argc, char* argv[]) {
    return boost::unit_test::unit_test_main(init_unit_test, argc, argv);
}
