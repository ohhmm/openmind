//
// Created by Сергей Кривонос on 30.09.17.
//

#include "FormulaOfVaWithSingleIntegerRoot.h"
#include "Integer.h"
#include "Sum.h"


namespace omnn {
namespace extrapolator {

    FormulaOfVaWithSingleIntegerRoot::FormulaOfVaWithSingleIntegerRoot(const Valuable& sumValuable, const Variable& v)
    : base(v,sumValuable)
    {
        auto sum = Sum::cast(sumValuable);
        std::vector<Valuable> coefficients;
        grade = sum->FillPolyCoeff(coefficients, v);
    }

    Valuable FormulaOfVaWithSingleIntegerRoot::Solve(Valuable& v) const
    {
        auto sum = Sum::cast(v);
        return *sum->GetFirstOccurence<Integer>() ^ (1_v/grade);
    }
    
    std::ostream& Formula::print(std::ostream& out) const
    {
        return out << "f(" << v << ") = solve(" << e << ")(x,y)";
    }
}}
