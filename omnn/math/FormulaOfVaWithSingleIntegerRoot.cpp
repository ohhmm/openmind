//
// Created by Сергей Кривонос on 30.09.17.
//

#include "FormulaOfVaWithSingleIntegerRoot.h"
#include "Integer.h"
#include "Product.h"
#include "Sum.h"
#include <list>
#include <boost/log/trivial.hpp>

namespace omnn {
namespace math {


    Valuable FormulaOfVaWithSingleIntegerRoot::Solve(Valuable& _) const
    {
        Valuable singleIntegerRoot;
        bool haveMin = false;
        BOOST_LOG_TRIVIAL(info) << "Before optimizing _";
        _.optimize();
        BOOST_LOG_TRIVIAL(info) << "After optimizing _";
        if (!_.IsSum()) {
            BOOST_LOG_TRIVIAL(error) << "Not a sum, implementation needed";
            IMPLEMENT
        }

        std::vector<Valuable> coefficients;

        //auto isMultival = IsMultival()== Valuable::YesNoMaybe::Yes;
        auto& sum = _.as<Sum>();
        auto g = sum.FillPolyCoeff(coefficients,getVa());
        if (g<3)
        {
            solutions_t solutions;
            BOOST_LOG_TRIVIAL(info) << "Before solving sum";
            sum.solve(getVa(), solutions, coefficients, g);
            BOOST_LOG_TRIVIAL(info) << "After solving sum, solutions size: " << solutions.size();

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

        BOOST_LOG_TRIVIAL(info) << "Before evaluating fx(min) and fx(max)";
        auto knowNoZ = !(min.IsMInfinity() || max.IsInfinity()) ? fx(min)*fx(max) > 0 : 0;
        BOOST_LOG_TRIVIAL(info) << "After evaluating fx(min) and fx(max), knowNoZ: " << knowNoZ;
        if (sum.size() > 2) {
            auto dx = _;
            BOOST_LOG_TRIVIAL(info) << "Before differentiating dx";
            while (dx.as<Sum>().size()>2) {
                dx.d(getVa());
            }
            BOOST_LOG_TRIVIAL(info) << "After differentiating dx";

            auto solution = dx.Solutions(getVa());
            if (solution.size() != 1) {
                BOOST_LOG_TRIVIAL(error) << "Multiple solutions, implementation needed";
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

                    if(!haveMin || std::abs(_) < std::abs(closestY)) {
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
            return i->Factorization(test, max);
        };

        auto freeMember = sum.begin()->IsExponentiation() ? *sum.rbegin() : _.calcFreeMember();
        if (freeMember.IsFraction()) {
            BOOST_LOG_TRIVIAL(error) << "Free member is a fraction, implementation needed";
            IMPLEMENT
        } else if (!freeMember.IsInt()) {
            freeMember = 0_v;
        }
        if(!freeMember.IsInt())
            IMPLEMENT
        auto& i = freeMember.as<Integer>();

        if(mode!=Strict && haveMin)
            return closest;

        BOOST_LOG_TRIVIAL(info) << "Before calling finder";
        if (finder(&i)) {
            BOOST_LOG_TRIVIAL(info) << "Finder succeeded, singleIntegerRoot: " << singleIntegerRoot;
            return singleIntegerRoot;
        } else if (!min.IsMInfinity() && !max.IsInfinity()){
            for (auto i = min; i <= max; ++i) {
                auto y = fx(i);
                if (y == 0) {
                    return i;
                } else if (std::abs(y) < std::abs(closestY)) {
                    closest = i;
                    closestY = y;
                }
            }
            return closest;
        }

        BOOST_LOG_TRIVIAL(error) << "Implementation needed at the end of Solve method";
        IMPLEMENT
    }

    std::ostream& FormulaOfVaWithSingleIntegerRoot::print(std::ostream& out) const
    {
        return out << "f(" << getVa() << ") = solve(" << getEx() << ")(x,y)";
    }
}}
