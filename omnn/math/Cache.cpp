//
// Created by Sergej Krivonos on 31.12.2019.
//
#include "Cache.h"
#include <deque>
#include <future>

using namespace omnn::math;

namespace {
constexpr size_t MaxThreadsForCacheStoring = 1024;
using CacheStoringTask = std::future<bool>;
using CacheStoringTasksQueue = std::deque<CacheStoringTask>;

void CleanupReadyTasks(CacheStoringTasksQueue &CacheStoringTasks) {
  while (CacheStoringTasks.size() &&
         ((CacheStoringTasks.front().valid() &&
           CacheStoringTasks.front().wait_for(std::chrono::seconds()) ==
               std::future_status::ready) ||
          CacheStoringTasks.size() >= MaxThreadsForCacheStoring)) {
    if (!CacheStoringTasks.front().get())
      std::cerr << "Cache storing task failed" << std::endl;
    CacheStoringTasks.pop_front();
  }
}
}

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

Cache::CachedSet Cache::AsyncFetchSet(const Valuable& v, bool itIsOptimized){
  using self_t = std::remove_reference<decltype(*this)>::type;
  auto&& task = std::async(std::launch::async, &self_t::GetSet,
                           this, v.str(), v.VaNames(), itIsOptimized);
  return std::move(task);
}

Cache::CheckCachedSet Cache::GetSet(const std::string& key, const Valuable::va_names_t& vaNames, bool itIsOptimized){
  CheckCachedSet cachedSet;
#ifdef OPENMIND_MATH_USE_LEVELDB_CACHE
  std::string value;
  cachedSet.first = db->Get(leveldb::ReadOptions(), key, &value).ok();
  if(cachedSet.first){
      char* token = nullptr;
      while((token = strtok(const_cast<char*>(value.c_str()), " "))){
          cachedSet.second.insert(Valuable(token, vaNames, itIsOptimized));
      }
#ifndef NDEBUG
    std::cout << "fetched from cache " << key << " => " << value << std::endl;
#endif
  }
#endif
  return cachedSet;
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
  static CacheStoringTasksQueue CacheStoringTasks;

  CleanupReadyTasks(CacheStoringTasks);

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

void Cache::AsyncSetSet(const Valuable& key, const val_set_t& v) {
  std::stringstream ss;
  ss << v;
  auto s = ss.str();
  auto l = s.length();
  if (s[l-1] == ' ')
    s[l-1] = 0;
  AsyncSet(key.str(), std::move(s));
}

Cache::Cached::operator Valuable() {
  auto got = get();
  assert(got.first);
#ifndef NDEBUG
  std::cout << "Used from cache: " << got.second << std::endl;
#endif
  return got.second;
}

Cache::CachedSet::operator val_set_t() {
  auto got = get();
  assert(got.first);
#ifndef NDEBUG
    std::cout << "Used from cache: [ " << got.second << "]" << std::endl;
#endif
  return got.second;
}
