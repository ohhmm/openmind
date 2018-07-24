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
        auto e = Exponentiation::cast(_);
        if (e) {
            static const Valuable z = 0_v;
            bool isSum = e->getBase().IsSum();
            Valuable sum = Sum{e->getBase(), z};
            auto s = Sum::cast(isSum ? e->getBase() : sum);
            if (s) {
                if (s->size() != 2) {
                    IMPLEMENT
                }
                auto va = s->template GetFirstOccurence<Variable>();
                if (va != s->end())
                {
                    auto it = vaVals.find(*Variable::cast(*va));
                    auto isValue = it == vaVals.end();
                    is = isValue;
                    auto isCoord = !isValue;
                    
                    if (va == s->begin())
                        ++va;
                    else --va;
                    auto maxCoord = -*va;
                    if (isCoord) {
                        auto& vaVal = *it;
                        is = predicate(*vaVal.second, maxCoord);
                    }
                    else if (value) {
                        *value = isSum ? &*va : &z;
                    }
                }
                else
                    IMPLEMENT
                    }
            else
                IMPLEMENT
                }
        else
            IMPLEMENT;
        return is;
    }
    
    bool Formula::InCoordFactorization(const VaValMap& vaVals) const
    {
        bool is = {};
        auto p = Product::cast(e);
        if (p)
        {
            // if it in range of product - then just find it by coordinates
            // check left gauge
            //auto first = p->begin();
            is = std::all_of(std::begin(vaVals), std::end(vaVals), [](auto& _){
                return *_.second >= 0;
            });
            
            if(is)
            {
                // check right gauge
                auto it =p->rbegin();
                if (*it==1)
                    ++it;
                auto& l = *it;
                auto s = Sum::cast(l);
                if (!s) {
                    IMPLEMENT
                }
                is = std::all_of(std::begin(*s), std::end(*s),
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
        auto p = Product::cast(e);
        assert(p);
        for(auto& m : *p)
        {
            auto s = Sum::cast(m);
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
                        IMPLEMENT
                    root = -*value;
                    return root; // found
                }
            }
            else if (m==1)
                continue;
            else
                IMPLEMENT
        }
        IMPLEMENT
    }
    
    std::ostream& Formula::print(std::ostream& out) const
    {
        return out << "f(" << v << ")=" << e;
    }
    
    Valuable Formula::Solve(Valuable& v) const
    {
        IMPLEMENT
        v.optimize();
        return v;
    }

    
}}
