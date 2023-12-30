#pragma once
#include "CacheBase.h"

namespace leveldb {
class DB;
}

namespace omnn::rt::storage {

#ifdef 	OPENMIND_STORAGE_LEVELDB
class LevelDbCache
    : public CacheBase
{
    leveldb::DB* _db = nullptr;

public:
    LevelDbCache(const std::string_view& path);
    std::string GetOne(const std::string_view& key) override;
    bool Set(const std::string_view& key, const std::string_view& v) override;
    bool Clear(const std::string_view& key) override;
    ~LevelDbCache() override;
};
#endif // OPENMIND_STORAGE_LEVELDB

}
