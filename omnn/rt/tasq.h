#pragma once
#include <deque>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <utility>


using StoringTask = std::future<bool>;
class StoringTasksQueue
    : public std::deque<StoringTask>
{
    std::mutex DequeMutEx;

protected:
    void CleanupReadyTasks();

public:
    void AddTask(const std::function<bool()>& f);
    void AddTask(std::function<bool()>&& f);

    template<typename C, typename F>
    void AddTasks(const C& cont, const std::function<bool()>& f) {
        for(auto& item : cont)
            AddTask(f);
    }

    template<typename C, typename F>
    void AddTasks(C&& cont, F&& f) {
        for(auto& item : cont) {
            std::function<bool()> fn = std::bind(std::forward<F>(f), item);
            AddTask(std::move(fn));
        }
    }
};
