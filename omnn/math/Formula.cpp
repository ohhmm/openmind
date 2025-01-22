//
// Created by Сергей Кривонос on 30.09.17.
//

#include "Formula.h"

#include <list>
#include <map>

#include "Sum.h"
#include "Variable.h"

namespace omnn {
namespace math {

    void Formula::CollectVarSequence() {
        std::set<Variable> vars;
        e.CollectVa(vars);
        s.clear();
        for(auto& v : vars)
            s.push_back(v);
    }

    Formula::Formula(const Variable& va, const Valuable& ex, const std::list<Variable>* sequence)
    : v(va), e(ex)
    {
        if(sequence)
            s = *sequence;
        else {
            CollectVarSequence();
        }
    }

    Formula Formula::DeduceFormula(const Valuable& e, const Variable& v)
    {
        //todo : once iterator ready
        throw "Implement!";
    }

    Formula Formula::DeclareFormula(const Variable& v, const Valuable& e)
    {
        return Formula(v,e);
    }

    bool Formula::coordMatch(const VaValMap& vaVals, const Valuable& _,
                             std::function<bool(const Valuable& /*vaValsV*/, const Valuable& /*sumV*/)> predicate,
                             Valuable const ** value)
    {
        bool is = {};
        if (_.IsExponentiation()) {
            auto& e = _.as<Exponentiation>();
            bool isSum = e.ebase().IsSum();
            Valuable sum = Sum{e.ebase(), constants::zero};
            auto& s = (isSum ? e.ebase() : sum).as<Sum>();
            if (s) {
                if (s.size() > 2) {
                    LOG_AND_IMPLEMENT("Formula::coordMatch not implemented for sum size > 2");
                }
                auto va = s.GetFirstOccurence<Variable>();
                if (va != s.end())
                {
                    // Check if we have a Variable in the sum
                    if (!va->IsVa()) {
                        LOG_AND_IMPLEMENT("Formula::coordMatch not implemented for non-variable value");
                        return false;
                    }
                    auto& var = va->as<Variable>();
                    auto it = vaVals.find(var);
                    auto isValue = it == vaVals.end();
                    is = isValue;
                    auto isCoord = !isValue;

                    if (va == s.begin())
                        ++va;
                    else --va;
                    auto maxCoord = va != s.end() ? -*va : constants::zero;
                    if (isCoord) {
                        auto& vaVal = *it;
                        is = predicate(*vaVal.second, maxCoord);
                    }
                    else if (value) {
                        *value = isSum && s.size() != 1 ? &*va : &constants::zero;
                    }
                }
                else
                    LOG_AND_IMPLEMENT("Formula::coordMatch not implemented for non-variable sum");
            }
            else
                LOG_AND_IMPLEMENT("Formula::coordMatch not implemented for non-sum");
        }
        else
            LOG_AND_IMPLEMENT("Formula::coordMatch not implemented for non-exponentiation");
        return is;
    }

    bool Formula::InCoordFactorization(const VaValMap& vaVals) const
    {
        bool is = {};
        if (e.IsProduct())
        {
            auto& p = e.as<Product>();
            // if it in range of product - then just find it by coordinates
            // check left gauge
            //auto first = p->begin();
            is = std::all_of(std::begin(vaVals), std::end(vaVals), [](auto& _){
                return *_.second >= 0;
            });

            if(is)
            {
                // check right gauge
                auto it =p.crbegin();
                if (*it==1)
                    ++it;
                auto& l = *it;
                if (!l.IsSum()) {
                    LOG_AND_IMPLEMENT("Formula::InCoordFactorization not implemented for non-sum");
                }
                auto& s = l.as<Sum>();
                is = std::all_of(std::begin(s), std::end(s),
                                 [&](auto& _) {
                                     return coordMatch(vaVals, _,
                                                      [](auto& v, auto& c){
                                                          return v <= c;
                                                      });
                                 });
            }
        }
        return is;
    }

    Valuable Formula::GetProductRootByCoordinates(const VaValMap& vaVals) const
    {
        Valuable root;
        auto vaCount = vaVals.size()+1;
        assert(e.IsProduct());
        auto& p = e.as<Product>();
        for(auto& m : p)
        {
            const Sum* s = m.IsSum() ? (&m.as<Sum>()) : nullptr;
            if(s && s->size() == vaCount)
            {
                Valuable const * value = {};
                bool is = std::all_of(std::begin(*s), std::end(*s),
                                      [&](auto& _) {
                                          return coordMatch(vaVals, _,
                                                            [](auto& v, auto& c){
                                                                return v == c;
                                                            },
                                                            &value
                                                            );
                                        }
                                      );
                if(is) {
                    if (!value)
                        LOG_AND_IMPLEMENT("Formula::GetProductRootByCoordinates value not found");
                    root = -*value;
                    return root; // found
                }
            }
            else if (m==1)
                continue;
            else
                LOG_AND_IMPLEMENT("Formula::GetProductRootByCoordinates not implemented for non-sum/non-one");
        }
        LOG_AND_IMPLEMENT("Formula::GetProductRootByCoordinates root not found");
    }

    std::ostream& Formula::print(std::ostream& out) const
    {
        return out << "f(" << v << ")=" << e;
    }

    Valuable Formula::Solve(Valuable& v) const
    {
        LOG_AND_IMPLEMENT("Formula::Solve not implemented");
        v.optimize();
        return v;
    }


}}
