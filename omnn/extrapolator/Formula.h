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
    //std::vector<Variable*> ev;
    
    Formula(const Variable&, const Valuable&);
    Formula(Valuable&& ex) : e(std::move(ex)){}
    Formula(int i) : e(i){}
    
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
        for(auto v:{vl...})
        {
            auto va = copy.FindVa();
            if (va)
            {
                auto vaCopy = *va;
                copy.Eval(vaCopy, v);
            }
            else
                throw "no more vars. optimized out? store vars vector to handle evaluation order and such cases";
        }
        copy.optimize();
        return copy;
    }
    
private:
    //f_t f;
};
}}
