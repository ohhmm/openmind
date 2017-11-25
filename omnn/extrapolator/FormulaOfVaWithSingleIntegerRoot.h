//
// Created by Сергей Кривонос on 30.09.17.
//

#pragma once
#include "Variable.h"

namespace omnn {
namespace extrapolator {


class FormulaOfVaWithSingleIntegerRoot
        : public ValuableDescendantContract<FormulaOfVaWithSingleIntegerRoot>
{
    using base = ValuableDescendantContract<FormulaOfVaWithSingleIntegerRoot>;
    Valuable sum;
    size_t grade;
    Variable va;
    std::set<Variable> s;
    Valuable Solve(Valuable& v) const;
    
public:
    using base::base;

    FormulaOfVaWithSingleIntegerRoot(const Valuable& s, const Variable v);
    
    void optimize() override { sum.optimize(); }
    
    template<class... T>
    Valuable operator()(const T&... vl) const
    {
        auto copy = sum;
        auto vit = s.begin();
        for(auto v:{vl...})
        {
            auto va = vit++;
            copy.Eval(*va, v);
        }
        copy.optimize();
        return Solve(copy);
    }
};
}}
