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
#ifndef NDEBUG
                std::system(("rm -rf " + path).c_str());
                if (!leveldb::DB::Open(options, path.c_str(), &db).ok())
#endif
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

      using CheckCacheResult = std::pair<bool,Valuable>;
      std::future<CheckCacheResult> AsyncFetch(const Valuable& v, bool itIsOptimized = false){
          using self_t = std::remove_reference<decltype(*this)>::type;
          return std::async(std::launch::async,
                            &self_t::GetOne,
                            this, v.str(), v.VaNames(), itIsOptimized);
      }

      CheckCacheResult GetOne(const std::string& key, const Valuable::va_names_t& vaNames, bool itIsOptimized = false)
        {
            CheckCacheResult cachedValue;
#ifdef OPENMIND_MATH_USE_LEVELDB_CACHE
            std::string value;
            cachedValue.first = db->Get(leveldb::ReadOptions(), key, &value).ok();
            if(cachedValue.first){
                cachedValue.second = Valuable(value, vaNames, itIsOptimized);
#ifndef NDEBUG
                std::cout << "fetched from cache " << key << " => " << cachedValue.second << std::endl;
#endif
            }
#endif
            return cachedValue;
        }
        
        bool Set(const std::string& key, const std::string& v)
        {
            return db
#ifdef OPENMIND_MATH_USE_LEVELDB_CACHE
                && db->Put(leveldb::WriteOptions(), key, v).ok()
#endif
                ;
        }
            
    };
}
