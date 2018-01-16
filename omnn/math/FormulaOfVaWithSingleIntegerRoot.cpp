//
// Created by Сергей Кривонос on 30.09.17.
//

#include "FormulaOfVaWithSingleIntegerRoot.h"
#include "Integer.h"
#include "Product.h"
#include "Sum.h"


namespace omnn {
namespace math {


    Valuable FormulaOfVaWithSingleIntegerRoot::Solve(Valuable& _) const
    {
        Valuable singleIntegerRoot;
        auto finder = [&](const Integer* i) -> bool
        {
            auto c = _;
            if(!Product::cast(c))
                c.optimize();
            return i->Factorization([this,c,&singleIntegerRoot](const Integer& i)
                                     {
                                         auto _ = c;
                                         _.Eval(getVa(), i);
                                         _.optimize();
                                         bool found = _ == 0_v;
                                         if (found) {
                                             singleIntegerRoot = i;
                                         }
                                         return found;
                                     });
        };

        auto freeMember = _.calcFreeMember();
        auto i = Integer::cast(freeMember);
        if(!i)
            IMPLEMENT
        if (finder(i)) {
            return singleIntegerRoot;
        }
        IMPLEMENT
    }
    
    std::ostream& FormulaOfVaWithSingleIntegerRoot::print(std::ostream& out) const
    {
        return out << "f(" << getVa() << ") = solve(" << getEx() << ")(x,y)";
    }
}}
