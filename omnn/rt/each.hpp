#pragma once
#if __has_include(<execution>)
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
    auto b = std::begin(c), e = std::end(c);
    std::for_each(
#ifndef __APPLE__
        PAR
#endif
        b, e, f);
#endif
}

template <class T, class F>
void each(T&& t, F&& f) {
    if (std::size(t) > 100)
        peach(std::forward<T>(t), std::forward<F>(f));
    else {
        auto b = std::begin(t), e = std::end(t);
        std::for_each(b, e, f);
    }
}
