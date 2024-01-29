#pragma once
#include <deque>
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

using StoringTask = std::future<bool>;
template <typename ResultT = bool>
class StoringTasksQueue : public std::deque < std::future<ResultT>> {
    std::mutex DequeMutEx;
    static constexpr size_t MaxThreadsForCacheStoring = 1024;
    bool CleanUp;

protected:
    bool CleanupReadyTasks() {
        if (CleanUp) {
            bool overburdened = {};
            do {
                {
                    std::lock_guard lg(DequeMutEx);
                    while (this->size()
                        && this->front().valid()
                        && this->front().wait_for(std::chrono::seconds(0)) == std::future_status::ready
                        ) {
                        this->pop_front();
                    }
                }
                overburdened = this->size() >= MaxThreadsForCacheStoring;
                if (overburdened)
                    std::this_thread::yield();
            } while (overburdened);
        }
        return this->size() == 0;
    }

public:
    StoringTasksQueue(bool autoCleanUp = true)
        : CleanUp(autoCleanUp)
    {}
    
    template <class FnT, class ...ParamsT>
    void AddTask(FnT&& f, ParamsT&&... params) {
        this->CleanupReadyTasks();
        auto task = std::async(std::launch::async, std::bind(std::forward<FnT>(f), std::forward<ParamsT>(params)...));
        std::lock_guard lg(DequeMutEx);
        this->emplace_back(std::move(task));
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
};

} // namespace omnn::rt