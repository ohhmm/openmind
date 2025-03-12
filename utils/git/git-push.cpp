#include "git-push.h"

#include <boost/process.hpp>
#include <iostream>
#include <sstream>
#include <string>


namespace git {

void push(std::string_view branch) {
    std::stringstream cmd;
    cmd << "\"" GIT_EXECUTABLE_PATH "\" push -f origin HEAD:" << branch;
    auto line = cmd.str();

    boost::process::child pushing(line);
    std::cout << "Pushing " << branch << " to origin: " << line << std::endl;
    pushing.join();
}

} // namespace git