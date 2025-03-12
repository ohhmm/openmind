#include "build.h"

#include <boost/process.hpp>
#include <iostream>
#include <string>


#define CMAKE_BUILD_COMMAND "\"" CMAKE_COMMAND "\" --build \"" SRC_DIR "\""


namespace cmake {


bool build() {
    boost::process::child build(CMAKE_BUILD_COMMAND);
    std::cout << "Building" << std::endl;
    build.join();
    return build.exit_code() == 0;
}

} // namespace cmake