#define BOOST_TEST_MODULE LevelDbCache test
#include "../CacheBase.h"
#include "../LevelDbCache.h"
#include <boost/test/unit_test.hpp>

using namespace omnn::rt::storage;

#ifdef OPENMIND_STORAGE_LEVELDB
BOOST_AUTO_TEST_CASE(LevelDbStorage_test) {
    LevelDbCache storage("testLevelDb");
    storage.Set("key", "value");
    auto got = storage.GetOne("key");
    BOOST_TEST(got == "value");
}
#endif // OPENMIND_STORAGE_LEVELDB

BOOST_AUTO_TEST_CASE(Empty_test) {
}
