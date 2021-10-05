#pragma once
//pgrama says include the file once


#ifdef OPENMIND_MATH_USE_FOUNDATIONDB_CACHE
//check the following function is defined


#include <foundationdb/db.h>
//copy and paste files into other files

#else
//namespace creates a block where all functions inside of it are asociated to the namespace
namespace fdb_cli{
class DB;
}
#endif
//if OPENMIND.. is defined then erases the else part, otherwise it keeps it


//#include <filesystem>
#include <future>
#include <memory>
#include <string>

#include <boost/filesystem.hpp>
//definition and implementation are in the same file, .hpp calls a class

#include "Variable.h"

namespace omnn::math {

namespace fs = boost::filesystem; // permite referenciar (acceder) a un tipo, objeto, función o enumerador incluso si su identificador está oculto
//define la primera gran clase
    class Cache
    {
    public:   //public declares a public member of a class. It allows access to a member inside and outside the class
        using path_str_t = fs::path;  -//define la variable path_srt_t como  path de la clase fs class fs and method path (?) función path dentro de clase fs
        // :: =  permite referenciar (acceder) a un tipo, objeto, función o enumerador incluso si su identificador está oculto
        using CheckCacheResult = std::pair<bool,Valuable>;

        template <typename ResultT>

        class CachedValueBase : public std::future<ResultT> {
          using base = std::future<ResultT>;
            ResultT result = {};
            bool extracted = {};
            void Extract() {     //void is a function that doesn't return any value
                if(!extracted){
                    extracted = true;
                    result = base::get(); //class base method get
                }
            }
        protected: //once it's gotten the value becomes protected i.e. only member of class' functions can access to it
                   //but it cannot be accesed from other classes
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
