#include "git-push.h"

#if __has_include(<boost/process/v1.hpp>)
#include <boost/process/v1.hpp>
namespace bp = boost::process::v1;
#else
#include <boost/process.hpp>
namespace bp = boost::process;
#endif
#include <iostream>
#include <sstream>
#include <string>


namespace git {

void push(std::string_view branch) {
    std::stringstream cmd;
    cmd << "\"" GIT_EXECUTABLE_PATH "\" push -f origin HEAD:" << branch;
    auto line = cmd.str();

    bp::child pushing(line);
    std::cout << "Pushing " << branch << " to origin: " << line << std::endl;
    pushing.join();
}

} // namespace git