#include "LevelDbCache.h"
#ifdef OPENMIND_STORAGE_LEVELDB
#include <leveldb/db.h>

#include <mutex>
#include <stdexcept>
#include <string>


using namespace omnn::rt::storage;


LevelDbCache::LevelDbCache(const std::string_view& path)
	: name(path)
{
    auto _status = leveldb::DB::Open(GetDbConnectionOptions(), name, &_db);
	if (!_status.ok())
		throw std::runtime_error(_status.ToString());
}

std::string LevelDbCache::GetOne(const std::string_view &key) {
	std::string v;
	auto _status = _db->Get(leveldb::ReadOptions(), static_cast<const std::string>(key), &v);
	if (!_status.ok())
		throw std::runtime_error(_status.ToString());
	return v;
}

bool LevelDbCache::Set(const std::string_view &key, const std::string_view &v) {
	auto _status = _db->Put(leveldb::WriteOptions(), static_cast<const std::string>(key), static_cast<const std::string>(v));
	return _status.ok();
}

bool LevelDbCache::Clear(const std::string_view &key) {
	auto _status = _db->Delete(leveldb::WriteOptions(), static_cast<const std::string>(key));
	return _status.ok();
}

bool LevelDbCache::ResetAllDB(const path_str_t& path) {
    // Delete all keys in the database
    auto it = _db->NewIterator(leveldb::ReadOptions());
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        auto status = _db->Delete(leveldb::WriteOptions(), it->key());
        if (!status.ok()) {
            delete it;
            return false;
        }
    }
    delete it;
    return true;
}

LevelDbCache::~LevelDbCache() {
	delete _db;
}

namespace{
	std::once_flag dbConnectionOptionsInitializedFlag;
}
const leveldb::Options& omnn::rt::storage::LevelDbCache::GetDbConnectionOptions() {
	static leveldb::Options options;
	std::call_once(dbConnectionOptionsInitializedFlag, [] {
		options.create_if_missing = true;
		options.compression = leveldb::kSnappyCompression;
		options.max_file_size = 512*1024*1024;
		options.block_size = 16*1024;
		options.paranoid_checks=true;
		});
	return options;
}

#endif
