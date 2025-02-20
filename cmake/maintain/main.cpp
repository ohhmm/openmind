#include <boost/process.hpp>

#include <algorithm>
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
#define GIT_REBASE_CONTINUE "\"" GIT_EXECUTABLE_PATH "\" rebase --continue"
#define GIT_IS_UPTODATE_WITH_ORIGIN_MAIN "\"" GIT_EXECUTABLE_PATH "\" merge-base --is-ancestor HEAD origin/main"
#define GIT_FETCH_ALL "\"" GIT_EXECUTABLE_PATH "\" fetch --all --prune"
#define CMD_LIST_LOCAL_BRANCHES "\"" GIT_EXECUTABLE_PATH "\" for-each-ref --format=%(refname:short) refs/heads/ --no-contains main"
#define CMD_LIST_ORIGIN_BRANCHES "\"" GIT_EXECUTABLE_PATH "\" for-each-ref --format=%(refname:short) refs/remotes/origin/ --no-contains main"
#define CMAKE_BUILD_COMMAND "\"" CMAKE_COMMAND "\" --build \"" SRC_DIR "\""


using namespace std;


namespace {
bool silent = {};
}

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

bool build() {
    boost::process::child build(CMAKE_BUILD_COMMAND);
    std::cout << "Building" << std::endl;
    build.join();
    return build.exit_code() == 0;
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

    while (!ok && !silent) {
        std::cout << "Would you like to resolve?" << std::endl;
        std::string line;
        if (std::getline(std::cin, line))
        {
            std::for_each(line.begin(), line.end(), [](auto ch) { return std::tolower(ch); });
            std::string_view response = line;
            response.remove_prefix(::std::min(response.find_first_not_of(" \t\r\v\n"), response.size()));
            response.remove_suffix((response.size() - 1) - ::std::min(response.find_last_not_of(" \t\r\v\n"), response.size() - 1));
            if (response != "n" && response != "no" && response != "skip")
            {
                boost::process::child continius(GIT_REBASE_CONTINUE);
                std::cout << "Continue rebasing " << branch << " onto " << onto << std::endl;
                continius.join();
                code = continius.exit_code();
                std::cout << "exit code: " << code << ' ' << line << std::endl;
                ok = code == 0;
            } else {
                break;
            }
        } else {
            break;
        }
    }

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

bool is_uptodate_with_origin_main() {
    boost::process::child is_uptodate(GIT_IS_UPTODATE_WITH_ORIGIN_MAIN);
    std::cout << "Checking if HEAD is up-to-date with origin/main: " GIT_IS_UPTODATE_WITH_ORIGIN_MAIN << std::endl;
    is_uptodate.join();
    auto code = is_uptodate.exit_code();
    std::cout << "exit code: " << code << std::endl;
    return code == 0;
}

void delete_remote_branch(std::string_view branch) {
    std::stringstream cmd;
    cmd << "\"" GIT_EXECUTABLE_PATH "\" push origin --delete " << branch;
    auto line = cmd.str();

    boost::process::child deleting(line);
    std::cout << "Deleting " << branch << " from origin: " << line << std::endl;
    deleting.join();
}

void rebase_remote_branches() {
    std::cout << "rebasing origin branches: " << std::endl;
    constexpr std::string_view origin_prefix = "origin/";
    constexpr size_t origin_prefix_len = origin_prefix.size();
    for (auto branch : list_origin_branches()) {
        if (rebase(branch)) {
            branch.remove_prefix(origin_prefix_len);
            if(is_uptodate_with_origin_main())
            {
                delete_remote_branch(branch);
            } else {
                push(branch);
            }
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
    boost::process::child stash(GIT_STASH);
    silent = argc > 1;
    fetch.join();
    stash.join();

    rebase_remote_branches();
    rebase_local_branches();

    rebase("main");
    boost::process::child(GIT_STASH_POP).join();
    return 0;
}
