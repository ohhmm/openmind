//
// Created by Сергей Кривонос on 30.09.17.
//

#include "FormulaOfVaWithSingleIntegerRoot.h"
#include "Integer.h"
#include "Sum.h"


namespace omnn {
namespace extrapolator {

    static void implement()
    {
        throw "Implement!";
    }
    
#define IMPLEMENT { implement(); throw; }
    
    FormulaOfVaWithSingleIntegerRoot::FormulaOfVaWithSingleIntegerRoot(const Valuable& sumValuable, const Variable& v)
    : base(v,sumValuable)
    {
        auto sum = Sum::cast(sumValuable);
        std::vector<Valuable> coefficients;
        grade = sum->FillPolyCoeff(coefficients, v);
    }

    Valuable FormulaOfVaWithSingleIntegerRoot::Solve(Valuable& _) const
    {
        Valuable singleIntegerRoot;
        auto sum = Sum::cast(_);
        if (sum) {
            auto ii = sum->GetFirstOccurence<Integer>();
            if (ii != sum->end()) {
                auto i = Integer::cast(*ii);
                if (i) {
                    bool found = i->Factorization([&](const Integer& i)
                    {
                        auto c = _;
                        c.Eval(getVa(), i);
                        c.optimize();
                        bool found = c == 0_v;
                        if (found) {
                            singleIntegerRoot = i;
                        }
                        return found;
                    });
                    if (found) {
                        return singleIntegerRoot;
                    }
                }
            }
        }
        
        IMPLEMENT
    }
    
    std::ostream& FormulaOfVaWithSingleIntegerRoot::print(std::ostream& out) const
    {
        return out << "f(" << getVa() << ") = solve(" << getEx() << ")(x,y)";
    }
}}
