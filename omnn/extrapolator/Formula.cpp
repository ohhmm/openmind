//
// Created by Сергей Кривонос on 30.09.17.
//

#include "Formula.h"

#include <map>

#include "Sum.h"


namespace omnn {
namespace extrapolator {

    Formula::Formula(const Variable& va, const Valuable& ex)
    : v(va), e(ex)
    {
        e.CollectVa(s);
    }

    //Formula::Formula(const Valuable& ex, const f_t& fn)
    //: e(ex), f(fn)
    //{ }
    
    Formula Formula::DeduceFormula(const Valuable& e, const Variable& v)
    {
        //todo : once iterator ready
        throw "Implement!";
    }
    
    Formula Formula::DeclareFormula(const Variable& v, const Valuable& e)
    {
        return Formula(v,e);
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
            bool is = std::all_of(std::begin(vaVals), std::end(vaVals), [](auto& _){
                return *_.second > 0;
            });
            
            if(is)
            {
                // check right gauge
                auto it =p->rbegin();
                if (*it==1)
                    ++it;
                auto& l = *it;
                auto s = Sum::cast(l);
                is = std::all_of(s->begin(), s->end(), [&](auto& _){
                        bool is = {};
                        auto e = Exponentiation::cast(_);
                        if (e) {
                            auto s = Sum::cast(e->getBase());
                            if (s) {
                                if (s->size() != 2) {
                                    IMPLEMENT
                                }
                                auto va = s->template GetFirstOccurence<Variable>();
                                if (va != s->end())
                                {
                                    auto vaVal = vaVals.at(*Variable::cast(*va));
                                    if (va == s->begin())
                                        ++va;
                                    else --va;
                                    is = *vaVal <= -*va;
                                }
                                else
                                    IMPLEMENT
                            }
                            else
                                IMPLEMENT
                        }
                        else
                            IMPLEMENT
                        return is;
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
                bool match={};
                for(auto sm: *s)
                {
                    auto e = Exponentiation::cast(sm);
                    if(e)
                    {
                        auto es = Sum::cast(e->getBase());
                        if (es) {
                            if (es->size() != 2) {
                                IMPLEMENT
                            }
                            
                        }
                    }
                    else
                        IMPLEMENT;
                }
                if(match)
                    return root; // found
            }
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
        v.optimize();
        return v;
    }

    
}}
