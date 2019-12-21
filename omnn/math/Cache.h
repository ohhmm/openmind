#pragma once

//#ifdef OPENMIND_MATH_USE_LEVELDB_CACHE

#include <leveldb/db.h>
#include <memory>
#include <string>

namespace omnn::math {
    
    
    class Cache
    {
    public:
        using path_str_t = std::string;
        
    private:
        leveldb::DB* db = nullptr;
        path_str_t path;
        
        void DbOpen() {
            leveldb::Options options;
            options.create_if_missing = true;
            options.compression = leveldb::kSnappyCompression;
            
            if (!leveldb::DB::Open(options, path.c_str(), &db).ok())
                throw path + " db connection error";
        }
        
    protected:
        std::string Value(const std::string& key);
        
    public:
        Cache(const path_str_t& path)
        {
            this->path = path;
            DbOpen();
        }
        
        ~Cache() {
            if(db)
                delete db; // hopely it has overloaded operator delete inside its image
        }
        
        std::pair<bool, Valuable> GetOne(const std::string& key)
        {
          std::string value;
          leveldb::Status s = db->Get(leveldb::ReadOptions(), key, &value);
            if(s.ok()){
              return { true, Valuable(value, {})};
            }
            else
              return {{},{}};
        }
        
        bool Set(const std::string& key, const std::string& v)
        {
            return db->Put(leveldb::WriteOptions(), key, v).ok();
        }
//
//        auto Get(const Valuable& key)
//        {
//            std::set<Valuable> values;
//            auto it = db->NewIterator({});
//            for (it->Seek(start);
//                 it->Valid() && it->key() == key.str();
//                 it->Next()) {
//            return values;
//        }
            
            
    };
}

//#endif
