export module git.list_branches;
#include "stl-waitwrap-generator.hpp"
#include <string_view>
export std::generator<std::string_view> list_local_branches();
export std::generator<std::string_view> list_origin_branches();
