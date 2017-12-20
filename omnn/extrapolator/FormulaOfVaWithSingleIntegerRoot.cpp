//
// Created by Сергей Кривонос on 30.09.17.
//

#include "FormulaOfVaWithSingleIntegerRoot.h"
#include "Integer.h"
#include "Product.h"
#include "Sum.h"


namespace omnn {
namespace extrapolator {

    FormulaOfVaWithSingleIntegerRoot::FormulaOfVaWithSingleIntegerRoot(const Valuable& sumValuable, const Variable& v)
    : base(v,sumValuable)
    {
    }

    Valuable FormulaOfVaWithSingleIntegerRoot::Solve(Valuable& _) const
    {
        Valuable singleIntegerRoot;
        auto finder = [&](const Integer* i) -> bool
        {
            return i->Factorization([&](const Integer& i)
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
        };
        auto sum = Sum::cast(_);
        if (sum) {
            _.optimize();
            sum = Sum::cast(_);
            if(!sum)
                IMPLEMENT
            auto ii = sum->GetFirstOccurence<Integer>();
            if (ii != sum->end()) {
                auto i = Integer::cast(*ii);
                if (i && finder(i)) {
                    return singleIntegerRoot;
                }
            }
            IMPLEMENT
        }
        
        auto p = Product::cast(_);
        if(p)
        {
            Product free_members_product;
            for(auto& m : *p)
            {
                auto s = Sum::cast(m);
                if (s)
                {
                    auto i = s->GetFirstOccurence<Integer>();
                    if (i != s->end()) {
                        free_members_product.Add(*i);
                    }
                }
                else if (Integer::cast(m))
                {
                    free_members_product.Add(m);
                }
                else
                {
                    auto e = Exponentiation::cast(m);
                    if(e)
                    {
                        s = Sum::cast(e->getBase());
                        if (s)
                        {
                            auto i = s->GetFirstOccurence<Integer>();
                            if (i != s->end()) {
                                free_members_product.Add(*i ^ e->getExponentiation());
                            }
                        }
                        else
                            IMPLEMENT
                    }
                    else
                        IMPLEMENT
                }
                
            }
            Valuable free_member = std::move(free_members_product);
            free_member.optimize();
            auto i = Integer::cast(free_member);
            if(!i)
                IMPLEMENT
            if (finder(i)) {
                return singleIntegerRoot;
            }
            IMPLEMENT
        }
        
        IMPLEMENT
    }
    
    std::ostream& FormulaOfVaWithSingleIntegerRoot::print(std::ostream& out) const
    {
        return out << "f(" << getVa() << ") = solve(" << getEx() << ")(x,y)";
    }
}}
