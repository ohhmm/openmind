#pragma once
#include <string_view>

namespace git {
extern bool silent;
extern bool rebase(std::string_view branch, std::string_view onto = "origin/main");
extern void rebase_remote_branches();
extern void rebase_local_branches();
} // namespace git
