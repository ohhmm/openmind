#include "build.h"

#if __has_include(<boost/process/v1.hpp>)
#include <boost/process/v1.hpp>
namespace bp = boost::process::v1;
#else
#include <boost/process.hpp>
namespace bp = boost::process;
#endif

#include <iostream>
#include <string>


#define CMAKE_BUILD_COMMAND "\"" CMAKE_COMMAND "\" --build \"" BUILD_DIR "\""


namespace cmake {


bool build() {
    bp::child build(CMAKE_BUILD_COMMAND);
    std::cout << "Building" << std::endl;
    build.wait();
    return build.exit_code() == 0;
}

} // namespace cmake
