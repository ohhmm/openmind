#include "git-list-branches.h"

#if __has_include(<boost/process/v1.hpp>)
#include <boost/process/v1.hpp>
namespace bp = boost::process::v1;
#else
#include <boost/process.hpp>
namespace bp = boost::process;
#endif
#include <iostream>
#include <string>

#define CMD_LIST_LOCAL_BRANCHES                                                                                        \
    "\"" GIT_EXECUTABLE_PATH "\" for-each-ref --format=%(refname:short) refs/heads/ --no-contains main"
#define CMD_LIST_ORIGIN_BRANCHES                                                                                       \
    "\"" GIT_EXECUTABLE_PATH "\" for-each-ref --format=%(refname:short) refs/remotes/origin/ --no-contains main"


namespace git {

std::generator<std::string_view> list_local_branches() {
    bp::ipstream pipe; // Create a pipe for stdout
    bp::child branches(CMD_LIST_LOCAL_BRANCHES, bp::std_out > pipe);
    std::string line;
    while (std::getline(pipe, line)) {
        co_yield line;
    }
    pipe.close();
    branches.join();
}

std::generator<std::string_view> list_origin_branches() {
    bp::ipstream pipe; // Create a pipe for stdout
    bp::child branches(CMD_LIST_ORIGIN_BRANCHES, bp::std_out > pipe);
    std::string line;
    while (std::getline(pipe, line)) {
        if (line != "origin" && line != "origin/main") {
            co_yield line;
        }
    }
    pipe.close();
    branches.join();
}

} // namespace git