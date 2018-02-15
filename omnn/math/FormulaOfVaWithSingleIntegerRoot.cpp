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
        bool haveMin = false;
        _.optimize();
        auto sum = Sum::cast(_);
        if (!sum) {
            IMPLEMENT
        }
        auto extrenums = sum->extrenums(getVa());
        auto zz = sum->get_zeros_zones(getVa(), extrenums);
        
        Valuable min;
        Valuable closest;
        auto finder = [&](const Integer* i) -> bool
        {
            auto c = _;
            if(!c.IsProduct())
                c.optimize();
            auto cdx = c;
            cdx.d(getVa());
            auto& seq = getVaSequanceForOp();
            FormulaOfVaWithSingleIntegerRoot f(getVa(), cdx, &seq);
            std::cout << "searching: f(" << getVa() << ")=" << _ << "; f'=" << cdx << std::endl;
            return i->Factorization([&,c](const Integer& i)
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
                                             auto d_ = cdx;
                                             d_.Eval(getVa(), i);
                                             d_.optimize();
                                             std::cout << "trying " << i << " got " << _ << " f'(" << i << ")=" << d_ << std::endl;
                                             if(!haveMin || _.abs() < min.abs()) {
                                                 closest = i;
                                                 min = _;
                                                 haveMin = true;
                                             }
                                             else if(mode==FirstExtrenum)
                                             {
                                                 if (haveMin) {
                                                     singleIntegerRoot=closest;
                                                     return true;
                                                 }
                                             }
                                         }
                                         return found;
                                     },
                                    zz);
        };

        auto freeMember = _.calcFreeMember();
        auto i = Integer::cast(freeMember);
        if(!i)
            IMPLEMENT
        if (finder(i)) {
            return singleIntegerRoot;
        }
        
        if(mode!=Strict && haveMin)
            return closest;
        
        IMPLEMENT
    }
    
    std::ostream& FormulaOfVaWithSingleIntegerRoot::print(std::ostream& out) const
    {
        return out << "f(" << getVa() << ") = solve(" << getEx() << ")(x,y)";
    }
}}
