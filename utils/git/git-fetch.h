#pragma once
#ifdef _WIN32
#include <winsock2.h>
#endif // _WIN32

#if __has_include(<boost/process/v1.hpp>)
#include <boost/process/v1.hpp>
namespace bp = boost::process::v1;
#else
#include <boost/process.hpp>
namespace bp = boost::process;
#endif

namespace git {

class GitFetchScope : bp::child {
public:
    static void fetch();
    static void fetch_start();

    GitFetchScope();
    ~GitFetchScope();
};
        
} // namespace git
