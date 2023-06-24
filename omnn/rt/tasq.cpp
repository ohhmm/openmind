#include "tasq.h"

#include <iostream>

constexpr size_t MaxThreadsForCacheStoring = 1024;


using namespace omnn::rt;


void StoringTasksQueue::CleanupReadyTasks() {
    std::lock_guard lg(DequeMutEx);
    while (size() &&
           ((front().valid() &&
             front().wait_for(std::chrono::seconds(0)) ==
             std::future_status::ready) ||
            size() >= MaxThreadsForCacheStoring)) {
        if (!front().get())
            std::cerr << "Cache storing task failed" << std::endl;
        pop_front();
    }
}

void StoringTasksQueue::AddTask(const std::function<bool()>& f) {
    
    this->CleanupReadyTasks();
    auto&& task = std::async(std::launch::async, f);
    std::lock_guard lg(DequeMutEx);
    emplace_back(std::move(task));
}

void StoringTasksQueue::AddTask(std::function<bool()>&& f) {
    this->CleanupReadyTasks();
    auto&& task = std::async(std::launch::async, std::move(f));
    std::lock_guard lg(DequeMutEx);
    emplace_back(std::move(task));
}
