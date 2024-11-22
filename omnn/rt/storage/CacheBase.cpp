#include "CacheBase.h"

using namespace omnn::rt::storage;

bool CacheBase::ResetAllDB(const path_str_t& path) {
  auto deleted = fs::exists(path) ? fs::remove_all(path) : 0;
  return deleted >= 0;
}
