#pragma once



#ifdef OPENMIND_MATH_USE_FOUNDATIONDB_CACHE

#include <foundationdb/fdb_c.h>

#include <free_fdb/ffdb.h>

#else

namespace fdb_cli{
	class FDB;
}
#endif


//#include <filesystem>
//#include <future>
#include <memory>
#include <string>

#include <boost/filesystem.hpp>
#include "Variable.h"

namespace omnn::math {

namespace bi = boost::intrusive;
    
class EvictablePage
    {
    public:
      using  path_str_t = bi::path
    	using CheckCacheResult = std::pair<bool,Valuable>;

    template < typename pageCache>
		
  	class Reference : public EvictablePageCache ::future<pageCache> {
			using base = std::future<pageCache>;//creo que lo de abajo es lo mismo
			virtual bool evict() = 0; //  evic=extracted


			//EvictablePage(Reference<EvictablePageCache> pageCache) :  pageCache(pageCache) {} 
       // virtual ~EvictablePage();
    }
       

   		 protected: 
          pageCache& Get() {
          evict()//Extract();
          return pageCache;
            }
       

			 public:
            using bi::bi;

          Reference (bi&& b) : bi(std::move(b))
          {}

          operator bool(){
            return (evict ||
              ( bi::valid()
                && bi::wait_for(std::chrono::seconds(0)) == std::future_status::ready
                )) && Get().first
                ;
          }

          bool NotInCache(){
            return (evict ||
                ( bi::valid()
                  && bi::wait_for(std::chrono::seconds(0)) == std::future_status::ready
                  )) && !Get().first;
          }
        };

class EvictablePageCache :  public ReferenceCounted<EvictablePageCache>
 {
     using List =
         bi::list<EvictablePage, bi::path<EvictablePage, bi::future<>, &EvictablePage::path>>;
 
          public:
            using bi::bi;
            using bi::operator bool;
//            EvictablePageCache (bi&& b);
            operator Valuable();
        };

        using val_set_t = Valuable::solutions_t;
        using CheckCachedSet = std::pair<bool,val_set_t>;






  class CachedSet : public ReferenceCounted <CheckCachedSet> {
        using base = ReferenceCounted <CheckCachedSet>;
      public:
            using bi::bi;
            using bi::operator bool;
//            CachedSet(bi&& b);
            operator val_set_t();
        };

    private:
        fdb_cli::FDB* fdb_c = nullptr;
        path_str_t path;



             // Opens a file that uses the FDB in-memory page cache
     static Future<Reference<IAsyncFile>> open(std::string filename, int flags, int mode) {
         //TraceEvent("AsyncFileCachedOpen").detail("Filename", filename);
         auto itr = openFiles.find(filename);
         if (itr == openFiles.end()) {
             auto f = open_impl(filename, flags, mode);
             if (f.isReady() && f.isError())
                 return f;
 
             auto result = openFiles.try_emplace(filename, f);
 
             // This should be inserting a new entry
             ASSERT(result.second);
             itr = result.first;
 
             // We return here instead of falling through to the outer scope so that we don't delete all references to
             // the underlying file before returning
             return itr->second.get();
         }
         return itr->second.get();
     }


        void DbOpen();
    protected:
        std::string Value(const std::string& key);
        
    public:
        Cachefdb(const path_str_t& path);
        ~Cachefdb();

		public:
			Cachefdb(const path_str_t&amp; path);
			~Cachefdb();
