#include "git-branch-delete.h"

#include <boost/process.hpp>
#include <iostream>
#include <sstream>


namespace git {

void delete_remote_branch(std::string_view branch) {
    std::stringstream cmd;
    cmd << "\"" GIT_EXECUTABLE_PATH "\" push origin --delete " << branch;
    auto line = cmd.str();

    boost::process::child deleting(line);
    std::cout << "Deleting " << branch << " from origin: " << line << std::endl;
    deleting.join();
}

} // namespace git