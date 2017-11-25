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
        auto singleWouldBe = *sum->GetFirstOccurence<Integer>() ^ (1_v/grade);
        return singleWouldBe;
    }
    
    std::ostream& FormulaOfVaWithSingleIntegerRoot::print(std::ostream& out) const
    {
        return out << "f(" << getVa() << ") = solve(" << getEx() << ")(x,y)";
    }
}}
