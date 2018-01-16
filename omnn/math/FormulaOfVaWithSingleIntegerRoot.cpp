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
            std::cout << "searching" << std::endl;
            if(!c.IsProduct())
                c.optimize();
            return i->Factorization([this,c,&singleIntegerRoot](const Integer& i)
                                     {
                                         auto _ = c;
                                         _.Eval(getVa(), i);
                                         _.optimize();
                                         bool found = _ == 0_v;
                                         if (found) {
                                             std::cout << "found " << i << std::endl;
                                             singleIntegerRoot = i;
                                         }
                                         else
                                         {
                                             std::cout << "trying " << i << " got " << _ << std::endl;
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
