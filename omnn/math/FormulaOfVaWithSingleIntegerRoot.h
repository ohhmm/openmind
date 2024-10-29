//
// Created by Сергей Кривонос on 30.09.17.
//

#pragma once
#include "Formula.h"
#include "Infinity.h"
#include <sstream>
#include <mutex>
#include <thread>
#include <vector>

namespace omnn {
namespace math {


class FormulaOfVaWithSingleIntegerRoot
        : public Formula
{
    using base = Formula;

protected:
    // Thread-safe evaluation cache and synchronization
    mutable std::mutex solve_mutex;
    mutable std::vector<Valuable> evaluation_cache;
    mutable Valuable closest;
    mutable Valuable closest_y;

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
