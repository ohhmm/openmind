#pragma once
//pragma says include the file once


#ifdef OPENMIND_MATH_USE_FOUNDATIONDB_CACHE
//check the following function is defined


#include <foundationdb/fdb_c.h>

#include <free_fdb/ffdb.hh>
//copy and paste files into other files

#else

namespace fdb_cli{
class FDB;
}
#endif



//#include <filesystem>
#include <future>
#include <memory>
#include <string>

#include <boost/filesystem.hpp>
//definition and implementation are in the same file, .hpp calls a class

#include "Variable.h"

namespace omnn::math {

namespace fs = boost::filesystem;

    class Cachefdb
    {
    public:
        using path_str_t_fdb = fs::path;

        using CheckCache_fdb_Result = std::pair<bool,Valuable>;

        template <typename ResultT_fdb>

        class Cached_fdb_ValueBase : public std::future<ResultT_fdb> {
          using base_fdb = std::future<ResultT_fdb>;
            ResultT_fdb result_fdb = {};
            bool extracted_fdb = {};
            void Extract_fdb() {
                if(!extracted_fdb){
                    extracted_fdb = true;
                    result_fdb = base::get(); //class base method get
                }
            }
        protected: //once it's gotten the value becomes protected i.e. only member of class' functions can access to it
                   //but it cannot be accesed from other classes
            ResultT_fdb& Get() {
                Extract_fdb();
                return result_fdb;
            }
        public:
          using base::base;

            Cached_fdb_ValueBase(base&& b) : base(std::move(b))
          {}

          operator bool(){
            return (extracted_fdb ||
              ( base::valid_fdb()
                && base::wait_for(std::chrono::seconds(0)) == std::future_status::ready
                )) && Get().first
                ;
          }

          bool NotInCache_fdb(){
            return (extracted_fdb ||
                ( base::valid_fdb()
                  && base::wait_for(std::chrono::seconds(0)) == std::future_status::ready
                  )) && !Get().first;
          }
        };

        class Cached_fdb : public Cached_fdb_ValueBase<CheckCache_fdb_Result> {
            using base = Cached_fdb_ValueBase<CheckCache_fdb_Result>;
        public:
            using base::base;
            using base::operator bool;
//            Cached_fdb(base&& b);
            operator Valuable_fdb();
        };

        using val_set_t_fdb = Valuable::solutions_t;
        using CheckCached_fdb_Set = std::pair<bool,val_set_t>;

        class Cached_fdb_Set : public Cached_fdb_ValueBase<CheckCached_fdb_Set> {
            using base = Cached_fdb_ValueBase<CheckCached_fdb_Set>;
        public:
            using base::base;
            using base::operator bool;
//            CachedSet_fdb(base&& b);
            operator val_set_t();
        };
        
        
        
        
        

   