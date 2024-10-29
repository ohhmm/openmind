//
// Created by Сергей Кривонос on 30.09.17.
//

#include "FormulaOfVaWithSingleIntegerRoot.h"
#include "Integer.h"
#include "Product.h"
#include "Sum.h"
#include <list>

namespace omnn {
namespace math {

// Define thread-local storage
thread_local FormulaOfVaWithSingleIntegerRoot::flow FormulaOfVaWithSingleIntegerRoot::evaluation_cache;
thread_local Valuable FormulaOfVaWithSingleIntegerRoot::thread_closest;
thread_local Valuable FormulaOfVaWithSingleIntegerRoot::thread_closest_y;

    Valuable FormulaOfVaWithSingleIntegerRoot::Solve(Valuable& _) const
    {
        thread_local Valuable singleIntegerRoot;
        thread_local bool haveMin = false;
        std::lock_guard<std::mutex> lock(solve_mutex);
        _.optimize();
        if (!_.IsSum()) {
            IMPLEMENT
        }

        std::vector<Valuable> coefficients;

        //auto isMultival = IsMultival()== Valuable::YesNoMaybe::Yes;
        auto& sum = _.as<Sum>();
        auto g = sum.FillPolyCoeff(coefficients,getVa());
        if (g<3)
        {
            solutions_t solutions;
            sum.solve(getVa(), solutions, coefficients, g);

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
        if (sum.size() > 2) {
            auto dx = _;

            while (dx.as<Sum>().size()>2) {
                dx.d(getVa());
            }

            auto solution = dx.Solutions(getVa());
            if (solution.size() != 1) {
                IMPLEMENT
            } else {
                auto s = *solution.begin();
                if ((s.IsInt() && fx(s).IsZero())
                    || (mode!=Strict && mode!=Closest && mode!=Newton && knowNoZ))
                {
                    return s;
                }
            }
        }

        // Initialize thread-local storage if needed
        if (thread_closest_y.IsZero()) {
            thread_closest_y = fx(thread_closest);
        }

        auto finder = [&](const Integer* i) -> bool
        {
            std::lock_guard<std::mutex> lock(solve_mutex);
            auto c = _;
            if(!c.IsProduct())
                c.optimize();
            auto cdx = c;
            cdx.d(getVa());

            auto nwtn = c / cdx;
            auto& seq = getVaSequenceForOp();
            FormulaOfVaWithSingleIntegerRoot f(getVa(), cdx, &seq);
//            std::cout << "searching: f(" << getVa() << ")=" << _ << "; f'=" << cdx << std::endl;
            Valuable was;
            std::function<bool(const Valuable&)> test = [&](const Valuable& i) -> bool
            {
                auto _ = c;
                _.Eval(getVa(), i);
                _.optimize();

                bool found = _.IsZero();
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
//                            thread_closest = i;
//                            return true;
//                        }
//                        return  test(newton);
//                    }

                    if(!haveMin || std::abs(_) < std::abs(thread_closest_y)) {
                        thread_closest = i;
                        thread_closest_y = _;
                        haveMin = true;
                    }
                    else if(mode==FirstExtrenum)
                    {
                        if (haveMin) {
                            singleIntegerRoot=thread_closest;
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

        auto freeMember = sum.begin()->IsExponentiation() ? *sum.rbegin() : _.calcFreeMember();
        if (freeMember.IsFraction()) {
            IMPLEMENT
        } else if (!freeMember.IsInt()) {
            freeMember = 0_v;
        }
        if(!freeMember.IsInt())
            IMPLEMENT
        auto& i = freeMember.as<Integer>();

        if(mode!=Strict && haveMin)
            return thread_closest;

        if (finder(&i)) {
            return singleIntegerRoot;
        } else if (!min.IsMInfinity() && !max.IsInfinity()){
            for (auto i = min; i <= max; ++i) {
                auto y = fx(i);
                if (y == 0) {
                    return i;
                } else if (std::abs(y) < std::abs(thread_closest_y)) {
                    thread_closest = i;
                    thread_closest_y = y;
                }
            }
            return thread_closest;
        }

        IMPLEMENT
    }
    
    std::ostream& FormulaOfVaWithSingleIntegerRoot::print(std::ostream& out) const
    {
        return out << "f(" << getVa() << ") = solve(" << getEx() << ")(x,y)";
    }
}}
