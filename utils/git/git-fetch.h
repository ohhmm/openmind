#pragma once
#include <boost/process.hpp>

namespace git {

class GitFetchScope
    : boost::process::child
{
public:
    static void fetch();
    static void fetch_start();

    GitFetchScope();
    ~GitFetchScope();
};
        
} // namespace git
