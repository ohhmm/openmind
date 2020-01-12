#pragma once

#ifdef OPENMIND_MATH_USE_LEVELDB_CACHE
#include <leveldb/db.h>
#else
namespace leveldb{
class DB;
}
#endif
#include <future>
#include <memory>
#include <string>

#include <Variable.h>

namespace omnn::math {
    
    class Cache
    {
    public:
        using path_str_t = std::string;
        using CheckCacheResult = std::pair<bool,Valuable>;

        template <typename ResultT>
        class CachedValueBase : public std::future<ResultT> {
          using base = std::future<ResultT>;
            ResultT result = {};
            bool extracted = {};
            void Extract() {
                if(!extracted){
                    extracted = true;
                    result = base::get();
                }
            }
        protected:
            ResultT& Get() {
                Extract();
                return result;
            }
        public:
          using base::base;

          CachedValueBase(base&& b) : base(std::move(b))
          {}

          operator bool(){
            return (extracted ||
              ( base::valid()
                && base::wait_for(std::chrono::seconds(0)) == std::future_status::ready
                )) && Get().first
                ;
          }

          bool NotInCache(){
            return (extracted ||
                ( base::valid()
                  && base::wait_for(std::chrono::seconds(0)) == std::future_status::ready
                  )) && !Get().first;
          }
        };

        class Cached : public CachedValueBase<CheckCacheResult> {
            using base = CachedValueBase<CheckCacheResult>;
        public:
            using base::base;
            using base::operator bool;
//            Cached(base&& b);
            operator Valuable();
        };
        
        using val_set_t = Valuable::solutions_t;
        using CheckCachedSet = std::pair<bool,val_set_t>;

        class CachedSet : public CachedValueBase<CheckCachedSet> {
            using base = CachedValueBase<CheckCachedSet>;
        public:
            using base::base;
            using base::operator bool;
//            CachedSet(base&& b);
            operator val_set_t();
        };
        
    private:
        leveldb::DB* db = nullptr;
        path_str_t path;
        
        void DbOpen();
        
    protected:
        std::string Value(const std::string& key);
        
    public:
        Cache(const path_str_t& path);
        ~Cache();

        Cached AsyncFetch(const Valuable& v, bool itIsOptimized = false);
        CheckCacheResult GetOne(const std::string& key, const Valuable::va_names_t& vaNames, bool itIsOptimized = false);
        
        CachedSet AsyncFetchSet(const Valuable& v, bool itIsOptimized = false);
        CheckCachedSet GetSet(const std::string& key, const Valuable::va_names_t& vaNames, bool itIsOptimized = false);

        bool Set(const std::string& key, const std::string& v);
        void AsyncSet(std::string&& key, std::string&& v);
        void AsyncSetSet(const Valuable& key, const val_set_t& v);
    };
}

#define CACHE(name) \
    auto cachename##name = std::string(#name) + ".solutions"; \
    Cache name(cachename##name);
