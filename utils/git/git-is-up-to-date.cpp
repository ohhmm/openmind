#include "git-list-branches.h"

#include <boost/process.hpp>
#include <iostream>
#include <string>

#define GIT_IS_UPTODATE_WITH_ORIGIN_MAIN "\"" GIT_EXECUTABLE_PATH "\" merge-base --is-ancestor HEAD origin/main"


namespace git {

bool is_uptodate_with_origin_main() {
    boost::process::child is_uptodate(GIT_IS_UPTODATE_WITH_ORIGIN_MAIN);
    std::cout << "Checking if HEAD is up-to-date with origin/main: " GIT_IS_UPTODATE_WITH_ORIGIN_MAIN << std::endl;
    is_uptodate.join();
    auto code = is_uptodate.exit_code();
    std::cout << "exit code: " << code << std::endl;
    return code == 0;
}

} // namespace git