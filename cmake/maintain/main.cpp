#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/process.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <system_error>
#include <utility>
#include <coroutine>
#include "stl-waitwrap-generator.hpp"

// used for maintain target
#define GIT_STASH "\"" GIT_EXECUTABLE_PATH "\" stash"
#define GIT_STASH_POP "\"" GIT_EXECUTABLE_PATH "\" stash pop"
#define GIT_REBASE_ABORT "\"" GIT_EXECUTABLE_PATH "\" rebase --abort"
#define GIT_FETCH_ALL "\"" GIT_EXECUTABLE_PATH "\" fetch --all"
#define CMD_LIST_LOCAL_BRANCHES "\"" GIT_EXECUTABLE_PATH "\" for-each-ref --format=%(refname:short) refs/heads/ --no-contains main"
#define CMD_LIST_ORIGIN_BRANCHES "\"" GIT_EXECUTABLE_PATH "\" for-each-ref --format=%(refname:short) refs/remotes/origin/ --no-contains main"


using namespace std;


generator<std::string_view> list_local_branches() {
    boost::process::ipstream pipe; // Create a pipe for stdout
    boost::process::child branches(CMD_LIST_LOCAL_BRANCHES,
                                   boost::process::std_out > pipe);
    std::string line;
    while (std::getline(pipe, line)) {
        co_yield line;
    }
    pipe.close();
    branches.join();
}

generator<std::string_view> list_origin_branches() {
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

bool rebase(std::string_view branch, std::string_view onto = "origin/main") {
    std::stringstream cmd;
    cmd << "\"" GIT_EXECUTABLE_PATH "\" rebase --autostash ";
    cmd << onto << ' ' << branch;
    auto line = cmd.str();

    boost::process::child rebase(line);
    std::cout << "Rebasing " << branch << " onto " << onto << std::endl;
    rebase.join();
    auto code = rebase.exit_code();
    std::cout << "exit code: " << code << ' ' << line << std::endl;
    auto ok = code == 0;
    if (!ok) {
        boost::process::child abort(GIT_REBASE_ABORT);
        abort.join();
        code = abort.exit_code();
        std::cout << "exit code: " << code << ' ' << GIT_REBASE_ABORT << std::endl;

        if (code != 0) {
            std::cerr << "Failed to abort rebase" << std::endl;
            std::exit(1);
        }
    }
    return ok;
}

void push(std::string_view branch) {
    std::stringstream cmd;
    cmd << "\"" GIT_EXECUTABLE_PATH "\" push -f origin HEAD:" << branch;
    auto line = cmd.str();

    boost::process::child pushing(line);
    std::cout << "Pushing " << branch << " to origin: " << line << std::endl;
    pushing.join();
}

void rebase_remote_branches() {
    std::cout << "rebasing origin branches: " << std::endl;
    constexpr std::string_view origin_prefix = "origin/";
    constexpr size_t origin_prefix_len = origin_prefix.size();
    for (auto branch : list_origin_branches()) {
        if (rebase(branch)) {
            push(branch.substr(origin_prefix_len));
        }
    }
}

void rebase_local_branches() {
    std::cout << "rebasing local branches: " << std::endl;
    for (auto branch : list_local_branches()) {
        rebase(branch);
    }
}

int main(int argc, char* argv[]) {
    boost::process::child fetch(GIT_FETCH_ALL);
    boost::process::child(GIT_STASH).join();
    fetch.join();

    rebase_remote_branches();
    rebase_local_branches();

    rebase("main");
    boost::process::child(GIT_STASH_POP).join();
    return 0;
}
