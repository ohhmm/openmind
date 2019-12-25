#pragma once

#ifdef OPENMIND_MATH_USE_LEVELDB_CACHE
#include <leveldb/db.h>
#else
namespace leveldb{
class DB;
}
#endif
#include <memory>
#include <string>

#include <Variable.h>

namespace omnn::math {
    
    
    class Cache
    {
    public:
        using path_str_t = std::string;
        
    private:
        leveldb::DB* db = nullptr;
        path_str_t path;
        
        void DbOpen() {
#ifdef OPENMIND_MATH_USE_LEVELDB_CACHE
            leveldb::Options options;
            options.create_if_missing = true;
            options.compression = leveldb::kSnappyCompression;
            
            if (!leveldb::DB::Open(options, path.c_str(), &db).ok()) {
                auto err = path + " db connection error";
                std::cerr << err << std::endl;
                throw err;
            }
#endif
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
        
        std::pair<bool, Valuable> GetOne(const std::string& key, std::shared_ptr<VarHost> host)
        {
#ifdef OPENMIND_MATH_USE_LEVELDB_CACHE
            std::string value;
            
            leveldb::Status s = db->Get(leveldb::ReadOptions(), key, &value);
            if(s.ok()){
                return { true, Valuable(value, host)};
            }
            else
#endif
                return {};
        }
        
        bool Set(const std::string& key, const std::string& v)
        {
            return db
#ifdef OPENMIND_MATH_USE_LEVELDB_CACHE
                && db->Put(leveldb::WriteOptions(), key, v).ok()
#endif
                ;
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
