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

#include <boost/filesystem.hpp>

#include <omnn/rt/tasq.h>
#include <omnn/storage/CacheBase.h>

#include <omnn/math/VarHost.h>


namespace omnn::math {

namespace fs = boost::filesystem;

    class Cache
    // TODO : public storage::CacheBase
    {
    public:
        using path_str_t = fs::path;
        using CheckCacheResult = std::pair<bool,Valuable>;
        class Cached;
        class CachedSet;
        using cache_get_value_task_queue_t = ::omnn::rt::StoringTasksQueue<CheckCacheResult, Cached>;

        static Cached TaskNoCache;
        static CachedSet TaskCachedSetNoCache;

        template <typename ResultT>
        class CachedValueBase
            : public ::omnn::rt::StoringTasksQueue<ResultT>::task_type
        {
            using base = ::omnn::rt::StoringTasksQueue<ResultT>::task_type;
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
            operator Valuable&();
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
        CheckCacheResult GetOneUsingVarHost(std::string&& key, VarHost::ptr host, bool itIsOptimized = false);
        CheckCacheResult GetOne(std::string&& key, Valuable::va_names_t&& vaNames, bool itIsOptimized = false);
        
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

#define USE_CACHE(name) auto& cache = name ; \
    auto cached = doCheckCache ? cache.AsyncFetch(*this, true) : Cache::TaskNoCache; \
    auto s = str();

#define CHECK_CACHED_READY if (cached) { \
    return Become(std::move(static_cast<Valuable&>(cached)));                                                      \
    }

#define CHECK_OPTIMIZATION_CACHE if (cached) { \
    Become(std::move(static_cast<Valuable&>(cached))); \
    return *this; \
}

#define CHECK_IN_CACHE if (doCheckCache && cached.NotInCache())

#define STORE_TO_CACHE \
    CHECK_IN_CACHE \
        cache.AsyncSet(std::move(s), str());
