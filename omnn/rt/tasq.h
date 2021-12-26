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
    template<class FnT, class... T>
    void AddTask(const FnT& f, T&& ...p){
        this->CleanupReadyTasks();
        auto&& task = std::async(std::launch::async, std::function<bool()>{f, std::forward(p)...});
        emplace_back(std::move(task));
    }
};
