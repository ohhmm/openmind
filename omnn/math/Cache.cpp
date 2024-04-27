//
// Created by Sergej Krivonos on 31.12.2019.
//
#include "Cache.h"

#include <iostream>
#include <thread>

#include <rt/tasq.h>
#include <storage/LevelDbCache.h>

#include <boost/tokenizer.hpp>


using namespace omnn::math;
using namespace omnn::rt;

namespace {


#ifdef OPENMIND_MATH_USE_LEVELDB_CACHE
void DeleteDB(const Cache::path_str_t& path) {
  if(fs::exists(path))
    fs::remove_all(path);
}
#endif
}

void Cache::DbOpen() {
#ifdef OPENMIND_MATH_USE_LEVELDB_CACHE
# if OPENMIND_MATH_CACHE_VOLATILE
  DeleteDB(path);
# endif
  leveldb::Status status;
  auto strPath = path.string();
  const auto& options = omnn::rt::storage::LevelDbCache::GetDbConnectionOptions();
  while (!((status = leveldb::DB::Open(options, strPath, &db)).ok())) {
    auto err = strPath + " DB connection error";
    std::cerr << err << status.ToString() << std::endl;
    if(status.IsIOError()){
      std::cout << "DB is busy. Waiting. Retrying in 5 sec." << std::endl;
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(5s);
    } else {
#ifndef NDEBUG
      DeleteDB(path);
      if (!leveldb::DB::Open(options, strPath, &db).ok())
#endif
      throw err;
    }
  }
#endif
}

Cache::Cache(const Cache::path_str_t &path) {
  this->path = path;
  DbOpen();
}

omnn::math::Cache::~Cache() {
#ifdef OPENMIND_MATH_USE_LEVELDB_CACHE
  if(db)
    delete db; // hopely it has overloaded operator delete inside its image
#endif
}

Cache::Cached Cache::AsyncFetch(const Valuable &v, bool itIsOptimized) {
  using self_t = std::remove_reference<decltype(*this)>::type;
    auto&& task = std::async(std::launch::async, &self_t::GetOne,
                             this, v.str(), v.VaNames(), itIsOptimized);
//    Cache::Cached cached(task);
  return std::move(task);
}

Cache::CheckCacheResult Cache::GetOneUsingVarHost(std::string&& key, VarHost::ptr host, bool itIsOptimized) {
  CheckCacheResult cachedValue;
#ifdef OPENMIND_MATH_USE_LEVELDB_CACHE
  std::string value;
  cachedValue.first = db->Get(leveldb::ReadOptions(), key, &value).ok();
  if (cachedValue.first) {
    cachedValue.second = Valuable(value, host, itIsOptimized);
#ifndef NDEBUG
//    std::cout << "fetched from cache " << key << " => " << cachedValue.second << std::endl;
#endif
    assert(cachedValue.second.is_optimized() == itIsOptimized);
#ifdef ALLOW_CACHE_UPGRADE
    auto gotValStr = cachedValue.second.str();
    if (gotValStr != value) {
#ifndef NDEBUG
      std::cout << "upgrading cached value for key " << key << ": " << value << " => " << gotValStr << std::endl;
#endif
      AsyncSet(std::string(key), std::move(gotValStr));
    }
#endif
  }
#endif
  return cachedValue;
}

Cache::CheckCacheResult Cache::GetOne(std::string&& key,
                                      Valuable::va_names_t&& vaNames,
                                      bool itIsOptimized) {
  CheckCacheResult cachedValue;
#ifdef OPENMIND_MATH_USE_LEVELDB_CACHE
  std::string value;
  cachedValue.first = db->Get(leveldb::ReadOptions(), key, &value).ok();
  if(cachedValue.first){
    cachedValue.second = Valuable(value, vaNames, itIsOptimized);
#ifndef NDEBUG
//    std::cout << "fetched from cache " << key << " => " << cachedValue.second << std::endl;
#endif
    assert(cachedValue.second.is_optimized() == itIsOptimized);
#ifdef ALLOW_CACHE_UPGRADE
    auto gotValStr = cachedValue.second.str();
    if (gotValStr != value) {
    #ifndef NDEBUG
    std::cout << "upgrading cached value for key " << key << ": " << value << " => " << gotValStr << std::endl;
    #endif
      AsyncSet(std::string(key), std::move(gotValStr));
    }
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

namespace {
constexpr auto CachedSetDelimeters = "<|>";
}
Cache::CheckCachedSet Cache::GetSet(const std::string& key, const Valuable::va_names_t& vaNames, bool itIsOptimized){
  CheckCachedSet cachedSet;
#ifdef OPENMIND_MATH_USE_LEVELDB_CACHE
  std::string value;
  cachedSet.first = db->Get(leveldb::ReadOptions(), key, &value).ok();
  if(cachedSet.first){
      auto token = strtok(value.data(), CachedSetDelimeters);
      while(token){
          cachedSet.second.insert(Valuable(std::string_view(token), vaNames, itIsOptimized));
          token = strtok(nullptr, CachedSetDelimeters);
      }
#ifndef NDEBUG
//    std::cout << "fetched from cache " << key << " => " << value << std::endl;
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
  static StoringTasksQueue CacheStoringTasks;

  // add new task
  CacheStoringTasks.AddTask(
      [ This=this, // TODO: This=shared_from_this(),
        k=std::move(key), v=std::move(v)
       ](){
          return This->Set(k,v);
        }
      );
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
  assert(operator bool());
  auto& got = Get();
#ifndef NDEBUG
  assert(got.first);
//  std::cout << "Used from cache: " << got.second << std::endl;
#endif
  assert(got.second.is_optimized()); // if cached value is not optimized then just remove this assert
  return got.second;
}

Cache::CachedSet::operator Cache::val_set_t() {
  assert(operator bool());
  auto got = Get();
  assert(got.first);
#ifndef NDEBUG
//    std::cout << "Used from cache: [ " << got.second << "]" << std::endl;
#endif
  return got.second;
}
