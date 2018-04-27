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
        
        std::vector<Valuable> coefficients;
        auto g = sum->FillPolyCoeff(coefficients,getVa());
        if (g<3)
        {
            solutions_t solutions;
            sum->solve(getVa(), solutions, coefficients, g);
            
            if(solutions.size() == 1)
            {
                singleIntegerRoot = std::move(*solutions.begin());
                return singleIntegerRoot;
            }
            else if(solutions.size())
                IMPLEMENT
                
        }
        
        auto fx = [&](auto& x) {
            auto t = _;
            t.Eval(getVa(), x);
            t.optimize();
            return t;
        };
        
        auto knowNoZ = !(min.IsMInfinity() || max.IsInfinity()) ? fx(min)*fx(max) > 0 : 0; // strictly saying this may mean >1
        if (sum->size() > 2) {
            auto dx = _;

            while (Sum::cast(dx)->size()>2) {
                dx.d(getVa());
            }
            
            auto solution = dx(getVa());
            if (solution.size() != 1) {
                IMPLEMENT
            } else {
                auto s = *solution.begin();
                if ((s.IsInt() && fx(s) == 0_v)
                    || (mode!=Strict && mode!=Closest && mode!=Newton && knowNoZ))
                {
                    return s;
                }
            }
        }
        
        
        //auto extrenums = sum->extrenums(getVa());
        //sum->get_zeros_zones(getVa(), extrenums);
        
        Valuable closest;
        auto closestY = fx(closest);
        auto finder = [&](const Integer* i) -> bool
        {
            auto c = _;
            if(!c.IsProduct())
                c.optimize();
            auto cdx = c;
            cdx.d(getVa());
            
            auto nwtn = c / cdx;
            auto& seq = getVaSequanceForOp();
            FormulaOfVaWithSingleIntegerRoot f(getVa(), cdx, &seq);
//            std::cout << "searching: f(" << getVa() << ")=" << _ << "; f'=" << cdx << std::endl;
            Valuable was;
            std::function<bool(const Valuable&)> test = [&](const Valuable& i) -> bool
            {
                auto _ = c;
                _.Eval(getVa(), i);
                _.optimize();
                
                bool found = _ == 0_v;
                if (found) {
//                    std::cout << "found " << i << std::endl;
                    singleIntegerRoot = i;
                }
                else
                {
                    auto d_ = cdx;
                    d_.d(getVa());
                    d_.Eval(getVa(), i);
                    d_.optimize();
//                    std::cout << "trying " << i << " got " << _ << " f'(" << i << ")=" << d_ << std::endl;
                    
//                    if (mode == Newton && i!=0)
//                    {
//                        auto newton = i - _/d_; //_ - i / d_;
//                        if (!newton.IsInt())
//                            newton = static_cast<a_int>(newton);
//                        if (newton == i) {
//                            singleIntegerRoot = i;
//                            closest = i;
//                            return true;
//                        }
//                        return  test(newton);
//                    }
                    
                    if(!haveMin || _.abs() < closestY.abs()) {
                        closest = i;
                        closestY = _;
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
            };
            return i->Factorization(test, max
//                                    ,zz
                                    );
        };

        auto freeMember = sum->begin()->IsExponentiation() ? *sum->rbegin() : _.calcFreeMember();
        if (freeMember.IsFraction()) {
            IMPLEMENT
        } else if (!freeMember.IsInt()) {
            freeMember = 0_v;
        }
        auto i = Integer::cast(freeMember);
        if(!i)
            IMPLEMENT
        
        if(mode!=Strict && haveMin)
            return closest;

        if (finder(i)) {
            return singleIntegerRoot;
        } else if (!min.IsMInfinity() && !max.IsInfinity()){
            for (auto i = min; i <= max; ++i) {
                auto y = fx(i);
                if (y == 0) {
                    return i;
                } else if (y.abs() < closestY.abs()) {
                    closest = i;
                    closestY = y;
                }
            }
            return closest;
        }
        
        IMPLEMENT
    }
    
    std::ostream& FormulaOfVaWithSingleIntegerRoot::print(std::ostream& out) const
    {
        return out << "f(" << getVa() << ") = solve(" << getEx() << ")(x,y)";
    }
}}
