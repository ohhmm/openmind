#pragma once
#include <omnn/rt/stl-waitwrap-generator.hpp>
#include <string_view>

namespace git {
extern std::generator<std::string_view> list_local_branches();
extern std::generator<std::string_view> list_origin_branches();
} // namespace git