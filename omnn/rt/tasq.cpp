#include "tasq.h"

#include <iostream>


constexpr size_t MaxThreadsForCacheStoring = 1024;


void StoringTasksQueue::CleanupReadyTasks() {
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
    emplace_back(std::move(task));
}

void StoringTasksQueue::AddTask(std::function<bool()>&& f) {
    this->CleanupReadyTasks();
    auto&& task = std::async(std::launch::async, std::move(f));
    emplace_back(std::move(task));
}
