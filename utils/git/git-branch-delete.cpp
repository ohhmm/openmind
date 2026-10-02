#include "git-branch-delete.h"

#if __has_include(<boost/process/v1.hpp>)
#include <boost/process/v1.hpp>
namespace bp = boost::process::v1;
#else
#include <boost/process.hpp>
namespace bp = boost::process;
#endif
#include <iostream>
#include <sstream>


namespace git {

void delete_remote_branch(std::string_view branch) {
    std::stringstream cmd;
    cmd << "\"" GIT_EXECUTABLE_PATH "\" push origin --delete " << branch;
    auto line = cmd.str();

    bp::child deleting(line);
    std::cout << "Deleting " << branch << " from origin: " << line << std::endl;
    deleting.join();
}

} // namespace git