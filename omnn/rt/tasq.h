#pragma once
#include <deque>
#include <future>
#include <memory>
#include <utility>


using StoringTask = std::future<bool>;
class StoringTasksQueue
    : public std::deque<StoringTask>
{
protected:
    void CleanupReadyTasks();
public:
    template<typename... T>
    void AddTask(T&&... p) {
        this->CleanupReadyTasks();
        auto&& task = std::async(std::launch::async, std::forward<T>(p)...);
        emplace_back(std::move(task));
    }
    template <typename... T>
    void AddTask(const std::function<void()>& f, T&&... p) {
        this->CleanupReadyTasks();
        auto&& task = std::async(
            std::launch::async,
            [&]() {
                f(std::forward<T>(p)...);
                return true;
            });
        emplace_back(std::move(task));
    }
};
