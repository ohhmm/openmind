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
    std::vector<Variable*> ev;
    
    Formula(const Variable&, const Valuable&);
    Formula(Valuable&& ex) : e(std::move(ex)){}
    Formula(int i) : e(i){}
    
public:
    using f_t = std::function<Valuable&&(Valuable&&)>;
    using base::base;
    static Formula DeduceFormula(const Valuable& e, const Variable& v);
    static Formula DeclareFormula(const Variable& v, const Valuable& e);

    Formula(const Valuable& e, const f_t& f);
    
    void optimize() override { e.optimize(); }
    
    template<class... T>
    Valuable operator()(const T&... vl) const
    {
        auto o = *this;
        auto va = o.ev.begin();
        for(auto v:{vl...})
        {
            auto copy = v;
            (*va)->Become(std::move(copy));
            ++va;
        }
        o.optimize();

        return f ? const_cast<f_t&>(f)(std::move(o.e)) : std::move(o.e);
    }
    
private:
    f_t f;
};
}}
