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
