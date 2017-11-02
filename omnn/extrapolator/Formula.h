//
// Created by Сергей Кривонос on 30.09.17.
//

#pragma once
#include "Variable.h"
#include <functional>

namespace omnn {
namespace extrapolator {


class Formula
        : public ValuableDescendantContract<Formula>
{
    using base = ValuableDescendantContract<Formula>;
    friend base;
    
    Variable v;
    Valuable e;
    std::set<Variable> s;
    
    Formula(const Variable&, const Valuable&);
    Formula(Valuable&& ex) : e(std::move(ex)) { e.CollectVa(s); }
    Formula(int i) : e(i) { e.CollectVa(s); }
    
protected:
    std::ostream& print(std::ostream& out) const override;

public:
    //using f_t = std::function<Valuable&&(Valuable&&)>;
    using base::base;
    static Formula DeduceFormula(const Valuable& e, const Variable& v);
    static Formula DeclareFormula(const Variable& v, const Valuable& e);

    //Formula(const Valuable& e, const f_t& f);
    
    void optimize() override { e.optimize(); }
    
    template<class... T>
    Valuable operator()(const T&... vl) const
    {
        auto copy = e;
        auto vit = s.begin();
        for(auto v:{vl...})
        {
            auto va = vit++;
            copy.Eval(*va, v);
        }
        copy.optimize();
        return copy;
    }
    
private:
    //f_t f;
};
}}
