
#include <chrono>

namespace omnn::measure {

template <typename F, typename... Args>
static auto time(F&& func, Args&&... args) {
    auto start = std::chrono::steady_clock::now();
    std::forward<decltype(func)>(func)(std::forward<Args>(args)...);
    auto duration = std::chrono::steady_clock::now() - start;
    return duration;
}

template <typename... Args>
static auto nanoseconds(Args&&... args) {
    auto duration = time(std::forward<Args>(args)...);
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
    return ns.count();
}

template <typename... Args>
static auto microseconds(Args&&... args) {
    auto duration = time(std::forward<Args>(args)...);
    auto ms = std::chrono::duration_cast<std::chrono::microseconds>(duration);
    return ms.count();
}

template <typename... Args>
static auto mills(Args&&... args) {
    auto duration = time(std::forward<Args>(args)...);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    return ms.count();
}

} // namespace omnn::measure
