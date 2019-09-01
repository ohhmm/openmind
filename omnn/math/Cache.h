#pragma once
#include <leveldb/db.h>

namespace omnn::math {
    template<const char* TName>
    class Data
    {
        static leveldb::DB* db;
        static leveldb::DB* DbOpen() {
            leveldb::Options options;
            options.create_if_missing = true;
            if (leveldb::DB::Open(options, TName, &db).ok())
                atexit([]() {delete db});
            return db;
        }
    protected:
        std::string Value(const std::string& key);
    };

    template<const char* TName>
    leveldb::DB* Data<TName>::db = DbOpen();

    template<const char* TName>
    class Cache
        : public Data<TName>
    {
    public:
        std::shared_ptr<Valuable> Get(const Valuable& va) {
            return Valuable(Value(va.str(), {}));
        }
    };
}