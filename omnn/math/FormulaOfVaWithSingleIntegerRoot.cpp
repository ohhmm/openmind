//
// Created by Сергей Кривонос on 30.09.17.
//

#include "FormulaOfVaWithSingleIntegerRoot.h"
#include "Integer.h"
#include "Product.h"
#include "Sum.h"
#include "Fraction.h"
#include <deque>
#include <list>
#include <ranges>


namespace omnn {
namespace math {


    Valuable FormulaOfVaWithSingleIntegerRoot::Solve(Valuable& _) const
    {
        Valuable singleIntegerRoot;
        bool haveMin = false;
        _.optimize();

        if (!_.IsSum()) {
            // Handle non-sum case by converting to sum form
            _ = Sum{_};
        }

        std::vector<Valuable> coefficients;

        //auto isMultival = IsMultival()== YesNoMaybe::Yes;
        auto& sum = _.as<Sum>();
        auto g = sum.FillPolyCoeff(coefficients,getVa());
        if (g<3)
        {
            solutions_t solutions;
            sum.solve(getVa(), solutions);

            if(solutions.size() == 1)
            {
                singleIntegerRoot = std::move(*solutions.begin());
                return singleIntegerRoot;
            }
            else if(solutions.size()) {
                if (min.IsMInfinity() && max.IsInfinity()) {
                    auto intSolutionsView = solutions | std::views::filter([](const auto& s) {
                        return s.IsInt();
                    });
                    auto numIntSolutions = std::ranges::distance(intSolutionsView.begin(), intSolutionsView.end());
                    if(numIntSolutions == 1) {
                        singleIntegerRoot = *intSolutionsView.begin();
                        return singleIntegerRoot;
                    } else if(numIntSolutions) {
                        singleIntegerRoot = *std::min_element(intSolutionsView.begin(), intSolutionsView.end(),
                            [](const Valuable& a, const Valuable& b) {
                                return std::abs(a) < std::abs(b);
                            });
                        return singleIntegerRoot;
                    } else {
                        IMPLEMENT
                    }
                }

                // filter solutions >= min and <= max
                for (auto idx = solutions.size()-1; idx-->0 ;)
                { 
                    auto itMin = std::min_element(solutions.begin(), solutions.end());
                    auto itMax = std::max_element(solutions.begin(), solutions.end());
                    if(itMin == itMax) {
                        return *itMin;
                    }

                    if(itMin == solutions.end() || itMax == solutions.end()) {
                        IMPLEMENT
                    }

                    auto minDistance = *itMin - min;
                    auto maxDistance = max - *itMax;
                    if(minDistance < 0 && maxDistance < 0) {
                        if(minDistance < maxDistance) {
                            solutions.erase(itMax);
                        } else {
                            solutions.erase(itMin);
                        }
                    } else if(minDistance < 0) {
                        solutions.erase(itMin);
                    } else if(maxDistance < 0) {
                        solutions.erase(itMax);
                    } else {
                        break;
                    }
                } while(solutions.size() > 1);

                // Handle multiple solutions by finding the one closest to zero, prefering positive value over negative with same distance to zero
                singleIntegerRoot = *std::min_element(solutions.begin(), solutions.end(),
                    [](const Valuable& a, const Valuable& b) {
                        return std::abs(a) < std::abs(b);
                    });
                return singleIntegerRoot;
            }
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
                // Handle multiple or no solutions
                if (solution.empty()) {
                    return mode != Strict ? closest : Valuable();
                }
                // Take the solution closest to zero
                singleIntegerRoot = *std::min_element(solution.begin(), solution.end(),
                    [](const Valuable& a, const Valuable& b) {
                        return std::abs(a) < std::abs(b);
                    });
                return singleIntegerRoot;
            } else {
                auto s = *solution.begin();
                if ((s.IsInt() && fx(s).IsZero())
                    || (mode!=Strict && mode!=Closest && mode!=Newton && knowNoZ))
                {
                    return s;
                }
            }
        }

        if (closest_y.IsZero()) {
            closest_y = fx(closest);
        }

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

                    if(!haveMin || std::abs(_) < std::abs(closest_y)) {
                        closest = i;
                        closest_y = _;
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

        auto freeMember = sum.begin()->IsExponentiation() ? *sum.rbegin() : _.calcFreeMember();
        if (freeMember.IsFraction()) {
            // Convert fraction to integer by multiplying both sides
            const auto& fraction = dynamic_cast<const Fraction&>(freeMember);
            auto denom = fraction.getDenominator();
            _ *= denom;
            _.optimize();
            freeMember = freeMember * denom;
        } else if (!freeMember.IsInt()) {
            freeMember = 0_v;
        }

        auto& i = freeMember.as<Integer>();

        if(mode!=Strict && haveMin)
            return closest;

        if (finder(&i)) {
            return singleIntegerRoot;
        } else if (!min.IsMInfinity() && !max.IsInfinity()){
            for (auto i = min; i <= max; ++i) {
                auto y = fx(i);
                if (y == 0) {
                    return i;
                } else if (std::abs(y) < std::abs(closest_y)) {
                    closest = i;
                    closest_y = y;
                }
            }
            return closest;
        }

        // If no solution found and not in strict mode, return closest
        return mode != Strict ? closest : Valuable();
    }
    
    std::ostream& FormulaOfVaWithSingleIntegerRoot::print(std::ostream& out) const
    {
        return out << "f(" << getVa() << ") = solve(" << getEx() << ")(x,y)";
    }
}}
