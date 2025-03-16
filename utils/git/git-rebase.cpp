#include "git-rebase.h"

#include "git-list-branches.h"
#include "git-is-up-to-date.h"
#include "git-branch-delete.h"
#include "git-push.h"

#include "build.h"

#include <boost/process.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <algorithm>


#define GIT_REBASE_ABORT "\"" GIT_EXECUTABLE_PATH "\" rebase --abort"
#define GIT_REBASE_CONTINUE "\"" GIT_EXECUTABLE_PATH "\" rebase --continue"


namespace git {
bool silent = {};

bool rebase(std::string_view branch, std::string_view onto) {
    std::stringstream cmd;
    cmd << "\"" GIT_EXECUTABLE_PATH "\" rebase --autostash ";
    cmd << onto << ' ' << branch;
    auto line = cmd.str();

    boost::process::child rebase(line);
    std::cout << "Rebasing " << branch << " onto " << onto << std::endl;
    rebase.join();
    auto code = rebase.exit_code();
    std::cout << "exit code: " << code << ' ' << line << std::endl;
    auto resolved = code == 0;
    auto built = (resolved && !silent) ? cmake::build() : false;
    while (!(resolved && built) && !silent) {
        if (!resolved)
            std::cout << "There are conflicts" << std::endl;
        if (!built)
            std::cout << "Build isn't successful" << std::endl;
        std::cout << "Would you like to resolve?" << std::endl;
        std::string line;
        if (std::getline(std::cin, line)) {
            std::for_each(line.begin(), line.end(), [](auto ch) { return std::tolower(ch); });
            std::string_view response = line;
            response.remove_prefix(::std::min(response.find_first_not_of(" \t\r\v\n"), response.size()));
            response.remove_suffix((response.size() - 1) -
                                   ::std::min(response.find_last_not_of(" \t\r\v\n"), response.size() - 1));
            if (response != "n" && response != "no" && response != "skip") {
                boost::process::child continius(GIT_REBASE_CONTINUE);
                std::cout << "Continue rebasing " << branch << " onto " << onto << std::endl;
                continius.join();
                code = continius.exit_code();
                std::cout << "exit code: " << code << ' ' << line << std::endl;
                resolved = code == 0;
                built = resolved && cmake::build();
            } else {
                std::cerr << "Build failed after resolving conflict" << std::endl;
            }
        } else {
            break;
        }
    }

    if (!resolved) {
        boost::process::child abort(GIT_REBASE_ABORT);
        abort.join();
        code = abort.exit_code();
        std::cout << "exit code: " << code << ' ' << GIT_REBASE_ABORT << std::endl;

        if (code != 0) {
            std::cerr << "Failed to abort rebase" << std::endl;
            std::exit(1);
        }
    }
    return resolved;
}

void rebase_remote_branches() {
    std::cout << "rebasing origin branches: " << std::endl;
    constexpr std::string_view origin_prefix = "origin/";
    constexpr size_t origin_prefix_len = origin_prefix.size();
    for (auto branch : list_origin_branches()) {
        if (rebase(branch)) {
            branch.remove_prefix(origin_prefix_len);
            if (is_uptodate_with_origin_main()) {
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

} // namespace git