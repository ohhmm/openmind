#pragma once
#include <queue>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>


namespace omnn::rt {

template <class ResultT>
class Task {
    std::future<ResultT> task;
    std::function<void(ResultT&&)> cb;

public:

};

template <typename... Args>
class ParamPackStorage {
public:
    // Use std::ref to store references in the constructor
    ParamPackStorage(Args&&... args)
        : data(std::forward_as_tuple(args...))
    {}

    // Access the data using a helper function
    template <std::size_t I>
    decltype(auto) get() const {
        return std::get<I>(data);
    }

private:
    std::tuple<Args...> data;
};

template <typename ResultT = bool,
    typename TaskT = std::shared_future<ResultT>,
    typename BaseContainerT = std::queue<TaskT>,
    size_t Threads = 1024,
    size_t CleanUpGauge = Threads / 16
>
class StoringTasksQueue
    : public BaseContainerT
{
    std::mutex queueMutEx;
    bool CleanUp;
    std::mutex cleaning;

protected:
    bool CleanupReadyTasks() {
        if (CleanUp && this->size() > CleanUpGauge)
        {
            auto lock = std::unique_lock(cleaning, std::try_to_lock);
            if (lock.owns_lock()) {
                bool overburdened = {};
                do {
                    {
                        std::lock_guard lg(queueMutEx);
                        while (this->size()
                            && this->front().valid()
                            && this->front().wait_for(std::chrono::seconds(0)) == std::future_status::ready
                            ) {
                            this->pop();
                        }
                    }
                    overburdened = this->size() >= Threads;
                    if (overburdened)
                        std::this_thread::yield();
                } while (overburdened);
            }
        }
        return this->size() == 0;
    }

public:
    using task_type = TaskT;

    StoringTasksQueue(bool autoCleanUp = true)
        : CleanUp(autoCleanUp)
    {}

    ~StoringTasksQueue() {
        while (!this->empty())
            PopCurrentTask(); // delete chronologically rather then base implementation backward-chronological deletion
    }

    template <class FnT, class ...ParamsT>
    auto& AddTask(FnT&& call, ParamsT&&... params) {
        this->CleanupReadyTasks();
        auto task = std::async(std::launch::async, std::bind(std::forward<FnT>(call), std::forward<ParamsT>(params)...));
        std::lock_guard lg(queueMutEx);
        return this->emplace(std::move(task));
    }

    template<typename C, class FnT, class ...ParamsT>
    void AddIteratorTasks(C&& cont, FnT&& f, ParamsT&&... params) {
        auto e = cont.end();
        for (auto i = cont.begin(); i!=e; ++i)
            AddTask(std::forward<FnT>(f), i, std::forward<ParamsT>(params)...);
    }

    template<typename C, class FnT, class ...ParamsT>
    void AddTasks(C&& cont, FnT&& f, ParamsT&&... params) {
        auto e = cont.end();
        for (auto i = cont.begin(); i!=e; ++i)
            AddTask(std::forward<FnT>(f), *i, std::forward<ParamsT>(params)...);
    }

    auto PopCurrentTask() {
        std::lock_guard lg(queueMutEx);
        auto future = std::move(this->front());
        this->pop();
        return future;
    }

    auto PeekNextResult() {
        return PopCurrentTask().get();
	}
};

} // namespace omnn::rt
