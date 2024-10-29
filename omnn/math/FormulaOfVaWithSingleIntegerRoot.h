//
// Created by Сергей Кривонос on 30.09.17.
//

#pragma once
#include "Formula.h"
#include "Infinity.h"
#include <sstream>
#include <boost/lockfree/queue.hpp>
#include <mutex>
#include <thread>

namespace omnn {
namespace math {


class FormulaOfVaWithSingleIntegerRoot
        : public Formula
{
    using base = Formula;
    using flow = boost::lockfree::queue<Valuable>;

protected:
    // Thread-safe evaluation cache and synchronization
    thread_local static flow evaluation_cache;
    thread_local static Valuable thread_closest;
    thread_local static Valuable thread_closest_y;
    mutable std::mutex solve_mutex;

    size_t grade;
    Valuable Solve(Valuable& v) const override;
    std::ostream& print(std::ostream& out) const override;

public:
    using base::base;
    enum Mode {
        Strict,
        Closest,
        FirstExtrenum,
        Newton,
        Some
    };

    void SetMode(Mode m) { mode = m; }
    void SetMax(const Valuable& m) { max = m; }
    void SetMin(const Valuable& m) { min = m; }

private:
    Mode mode = Strict;
    Valuable max = Infinity();
    Valuable min = MInfinity();
};
}}
