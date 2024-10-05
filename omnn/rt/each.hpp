#pragma once
#ifdef __APPLE__
#define SEQ
#define PAR
#elif __has_include(<execution>)
#include <execution>
#define SEQ std::execution::seq,
#define PAR std::execution::par,
#elif __has_include(<experimental/execution>)
#include <experimental/execution>
#define SEQ std::execution::seq,
#define PAR std::execution::par,
#else
#define SEQ
#define PAR
#endif

#include <omnn/rt/tasq.h>

template <class T, class F>
void peach(T&& c, F&& f) {
#ifdef __APPLE__
    omnn::rt::StoringTasksQueue().AddTasks(std::forward<T>(c), std::forward<F>(f));
#else
    std::for_each(PAR
        std::begin(c), std::end(c), f);
#endif
}

template <class T, class F>
void each(T&& t, F&& f) {
    if (std::size(t) > 100)
        peach(std::forward<T>(t), std::forward<F>(f));
    else {
        std::for_each(std::begin(t), std::end(t), f);
    }
}
