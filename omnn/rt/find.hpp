//
// Created by serge on 10/4/2024.
//
#pragma once
#include <omnn/rt/each.hpp>

namespace omnn::rt {

template <class T, class F>
auto find(T&& t, F&& f) {
    return std::size(t) > USE_PAR_GAUGE
        ? std::find(PAR
            std::begin(t), std::end(t),
            std::forward<F>(f))
        : std::find(
            std::begin(t), std::end(t),
            std::forward<F>(f));
}


template <class T, class F>
auto find_if(T&& t, F&& f) {
    return std::size(t) > USE_PAR_GAUGE
        ? std::find_if(PAR
            std::begin(t), std::end(t),
            std::forward<F>(f))
        : std::find_if(
            std::begin(t), std::end(t),
            std::forward<F>(f));
}

} // namespace omnn::rt
