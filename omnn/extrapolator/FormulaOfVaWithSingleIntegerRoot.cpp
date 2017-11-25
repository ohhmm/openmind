//
// Created by Сергей Кривонос on 30.09.17.
//

#include "FormulaOfVaWithSingleIntegerRoot.h"
#include "Integer.h"
#include "Sum.h"


namespace omnn {
namespace extrapolator {

    FormulaOfVaWithSingleIntegerRoot::FormulaOfVaWithSingleIntegerRoot(const Valuable& sumValuable, const Variable v)
    : sum(sumValuable), va(v)
    {
        auto sum = Sum::cast(sumValuable);
        std::vector<Valuable> coefficients;
        grade = sum->FillPolyCoeff(coefficients, v);
        sum->CollectVa(s);
    }

    Valuable FormulaOfVaWithSingleIntegerRoot::Solve(Valuable& v) const
    {
        auto sum = Sum::cast(v);
        return *sum->GetFirstOccurence<Integer>() ^ (1_v/grade);
    }
    
    std::ostream& Formula::print(std::ostream& out) const
    {
        return out << "f(" << v << ")=" << e;
    }
}}
