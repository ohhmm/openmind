//
// Created by Сергей Кривонос on 30.09.17.
//

#include "Formula.h"


namespace omnn {
namespace extrapolator {

    Formula::Formula(const Variable& va, const Valuable& ex)
    : v(va), e(ex)
    {
        e.CollectVa(s);
    }

    //Formula::Formula(const Valuable& ex, const f_t& fn)
    //: e(ex), f(fn)
    //{ }
    
    Formula Formula::DeduceFormula(const Valuable& e, const Variable& v)
    {
        //todo : once iterator ready
        throw "Implement!";
    }
    
    Formula Formula::DeclareFormula(const Variable& v, const Valuable& e)
    {
        return Formula(v,e);
    }

    std::ostream& Formula::print(std::ostream& out) const
    {
        return out << "f(" << v << ")=" << e;
    }
}}
