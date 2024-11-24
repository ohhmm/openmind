#include "exit.h"

#include <cstdlib>


namespace {

bool ProcessIsExiting = {};

auto CancelAtExit = []() -> decltype(nullptr) {
    atexit([]() { ProcessIsExiting = true; });
    return {};
}();

} // namespace


namespace omnn::rt {

bool IsProcessExit() { return ProcessIsExiting; }

} // namespace omnn::rt
