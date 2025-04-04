#pragma once

#include <string>
#include <string_view>

#include <boost/filesystem.hpp>

namespace omnn::storage {

    namespace fs = boost::filesystem;

class CacheBase {
public:
    using path_str_t = fs::path;

    virtual std::string GetOne(const std::string_view& key) = 0;
    virtual bool Set(const std::string_view& key, const std::string_view& v) = 0;
    virtual bool Clear(const std::string_view& key) = 0;
    virtual bool ResetAllDB(const path_str_t& path) = 0;
    virtual ~CacheBase() {}
};

}
