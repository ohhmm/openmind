//
// Created by Sergej Krivonos on 31.12.2019.
//
#include "Cache.h"
#include <deque>
#include <future>

using namespace omnn::math;

void Cache::DbOpen() {
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

Cache::Cache(const Cache::path_str_t &path) {
  this->path = path;
  DbOpen();
}

omnn::math::Cache::~Cache() {
  if(db)
    delete db; // hopely it has overloaded operator delete inside its image
}

Cache::Cached Cache::AsyncFetch(const Valuable &v, bool itIsOptimized) {
  using self_t = std::remove_reference<decltype(*this)>::type;
    auto&& task = std::async(std::launch::async, &self_t::GetOne,
                             this, v.str(), v.VaNames(), itIsOptimized);
//    Cache::Cached cached(task);
  return std::move(task);
}

Cache::CheckCacheResult Cache::GetOne(const std::string &key,
                                      const Valuable::va_names_t &vaNames,
                                      bool itIsOptimized) {
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

bool Cache::Set(const std::string &key, const std::string &v) {
  return db
#ifdef OPENMIND_MATH_USE_LEVELDB_CACHE
         && db->Put(leveldb::WriteOptions(), key, v).ok()
#endif
      ;
}

void Cache::AsyncSet(std::string &&key, std::string &&v) {
#ifdef OPENMIND_MATH_USE_LEVELDB_CACHE
  constexpr size_t MaxThreadsForCacheStoring = 1024;
  using CacheStoringTask = std::future<bool>;
  static std::deque<CacheStoringTask> CacheStoringTasks;

  // cleanup ready tasks:
  while(CacheStoringTasks.size() &&
        ((CacheStoringTasks.front().valid() &&
        CacheStoringTasks.front().wait_for(std::chrono::seconds()) == std::future_status::ready)
        || CacheStoringTasks.size() >= MaxThreadsForCacheStoring)
        ) {
    if(!CacheStoringTasks.front().get())
      std::cerr << "Cache storing task failed" << std::endl;
    CacheStoringTasks.pop_front();
  }

  // add new task
  CacheStoringTasks.emplace_back(std::async(std::launch::async,
      [ This=this, // TODO: This=shared_from_this(),
        k=std::move(key), v=std::move(v)
       ](){
          return This->Set(k,v);
        }
      ));
#endif
}

Cache::Cached::operator bool() {
  return valid()
         && wait_for(std::chrono::seconds()) == std::future_status::ready
         && get().first
      ;
}

Cache::Cached::operator Valuable() {
  auto got = get();
  assert(got.first);
#ifndef NDEBUG
  std::cout << "Used from cache: " << got.second << std::endl;
#endif
  return got.second;
}

Cache::Cached::Cached(std::future<Cache::CheckCacheResult> && b)
    :base(std::move(b))
{}

bool Cache::Cached::NotInCache(){
    return valid()
        && wait_for(std::chrono::seconds()) == std::future_status::ready
        && !get().first;
}
