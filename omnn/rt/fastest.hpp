#pragma once
#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <type_traits>
#include <vector>

#include "tasq.h"


namespace omnn::rt {

template <typename ResultType>
class TheFastestResult {

    static StoringTasksQueue<ResultType> storingTasksQueue;

public:
    using task_type = std::future<ResultType>;
    using task_reference = std::reference_wrapper<task_type>;

    template <typename... Callables>
    TheFastestResult(Callables... tasks) {
        static_assert(CheckReturnTypes<ResultType, Callables...>::value,
                      "All callables must have the same return type");
        launch_tasks({tasks...});
    }

    operator std::future<ResultType>() { return first_finished(); }
    operator ResultType() { return first_finished().get(); }

private:
    std::vector<task_reference> futures;

    template <typename T, typename... Callables>
    struct CheckReturnTypes;

    template <typename T, typename Callable, typename... Callables>
    struct CheckReturnTypes<T, Callable, Callables...> {
        static constexpr bool value =
            std::is_same_v<T, std::invoke_result_t<Callable>> && CheckReturnTypes<T, Callables...>::value;
    };

    template <typename T>
    struct CheckReturnTypes<T> {
        static constexpr bool value = true;
    };

    template <typename... Callables>
    void launch_tasks(std::initializer_list<std::function<ResultType()>> tasks) {
        for (const auto& task : tasks) {
            futures.push_back(storingTasksQueue.AddTask(task));
        }
    }

    std::future<ResultType> first_finished() {
        return std::async(std::launch::async, [this]() {
            auto first_ready = std::find_if(futures.begin(), futures.end(), [](const auto& future) {
                return future.get().wait_for(std::chrono::seconds(0)) == std::future_status::ready;
            });

            while (first_ready == futures.end()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                first_ready = std::find_if(futures.begin(), futures.end(), [](const auto& future) {
                    return future.get().wait_for(std::chrono::seconds(0)) == std::future_status::ready;
                });
            }

            return first_ready->get().get();
        });
    }
};

template <typename ResultType>
StoringTasksQueue<ResultType> TheFastestResult<ResultType>::storingTasksQueue;
} // namespace omnn::rt
