//
// Created by Сергей Кривонос on 30.09.17.
//

#pragma once
#include "Formula.h"
#include "Infinity.h"
#include <sstream>
#include <boost/lockfree/queue.hpp>

namespace omnn {
namespace math {


class FormulaOfVaWithSingleIntegerRoot
        : public Formula
{
    using base = Formula;
    using flow = boost::lockfree::queue<Valuable>;
    //flow extrenums, doubleDerivatives;
    
protected:
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
