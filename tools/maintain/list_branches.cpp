module git.list_branches;
#include "stl-waitwrap-generator.hpp"
#include <string_view>

#include "list_branches.h"

#include <boost/process.hpp>

#include <string>

#define CMD_LIST_LOCAL_BRANCHES                                                                                        \
    "\"" GIT_EXECUTABLE_PATH "\" for-each-ref --format=%(refname:short) refs/heads/ --no-contains main"
#define CMD_LIST_ORIGIN_BRANCHES                                                                                       \
    "\"" GIT_EXECUTABLE_PATH "\" for-each-ref --format=%(refname:short) refs/remotes/origin/ --no-contains main"


std::generator<std::string_view> list_local_branches() {
    boost::process::ipstream pipe; // Create a pipe for stdout
    boost::process::child branches(CMD_LIST_LOCAL_BRANCHES, boost::process::std_out > pipe);
    std::string line;
    while (std::getline(pipe, line)) {
        co_yield line;
    }
    pipe.close();
    branches.join();
}

std::generator<std::string_view> list_origin_branches() {
    boost::process::ipstream pipe; // Create a pipe for stdout
    boost::process::child branches(CMD_LIST_ORIGIN_BRANCHES, boost::process::std_out > pipe);
    std::string line;
    while (std::getline(pipe, line)) {
        if (line != "origin" && line != "origin/main") {
            co_yield line;
        }
    }
    pipe.close();
    branches.join();
}
