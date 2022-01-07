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
    template<typename F, typename... T>
    void AddTask(F&& f, T&&... p) {
        this->CleanupReadyTasks();
        auto&& task = std::async(std::launch::async, std::forward<F>(f), std::forward<T>(p)...);
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

    template<typename C, typename F, typename... T>
    void AddTasks(const C& cont, F&& f, T&&... p) {
        for(auto& item : cont)
            AddTask(std::forward<F>(f), std::forward<T>(p)...);
    }
};
