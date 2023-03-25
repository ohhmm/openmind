#pragma once
#include "tasq.h"

template <class T, class F>
void peach(T&& c, F&& f) {
#ifdef __APPLE__
    StoringTasksQueue().AddTasks(std::forward<T>(c), std::forward<F>(f));
#else
    auto b = std::begin(c), e = std::end(c);
    std::for_each(
#ifndef __APPLE__
        std::execution::par,
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
