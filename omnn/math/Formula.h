//
// Created by Сергей Кривонос on 30.09.17.
//

#pragma once
#include <functional>
#include <map>
#include "Product.h"
#include "Variable.h"

namespace omnn {
namespace math {

    class Sum;
    
class Formula
        : public ValuableDescendantContract<Formula>
{
    using base = ValuableDescendantContract<Formula>;
    friend base;
    
    Variable v;
    Valuable e;
    std::list<Variable> s; // sequence for operator()
    
    void CollectVarSequence();
    
    Formula(Valuable&& ex) : e(std::move(ex)) { CollectVarSequence(); }
    Formula(int i) : e(i) { CollectVarSequence(); }
    
    using VaValMap = ::std::map<const Variable,const Valuable*>;
    Valuable GetProductRootByCoordinates(const VaValMap& vaVals) const;
    bool InCoordFactorization(const VaValMap& vaVals) const;
    
    static bool coordMatch(const VaValMap& vaVals, const Valuable& _,
                           std::function<bool(const Valuable& /*vaValsV*/, const Valuable& /*sumV*/)> predicate,
                           Valuable const ** value = {}
                           );
    
protected:
    const Valuable& getE() const { return e; }
    std::ostream& print(std::ostream& out) const override;
    virtual Valuable Solve(Valuable& v) const;
public:
    Formula(const Variable& v, const Valuable& e, const std::list<Variable>* sequence = {});
    bool IsFormula() const override { return true; }
    //using f_t = std::function<Valuable&&(Valuable&&)>;
    using base::base;
    static Formula DeduceFormula(const Valuable& e, const Variable& v);
    static Formula DeclareFormula(const Variable& v, const Valuable& e);

    //Formula(const Valuable& e, const f_t& f);
    
    const Variable& getVa() const { return v; }
    const Valuable& getEx() const { return e; }
    const std::list<Variable>& getVaSequanceForOp() const { return s; }
    
    void optimize() override { e.optimize(); }
    Valuable calcFreeMember() const override { return e.calcFreeMember(); }
    
    template<class... T>
    Valuable operator()(const T&... vl) const
    {
        Valuable root;
        auto copy = e;
        std::initializer_list<Valuable> args = {vl...};
        
        // va values map
        VaValMap vaVals;
        auto vit = s.begin();
        for(auto& a:args){
            auto va = vit++;
            vaVals[*va] = &a;
        }

        if (InCoordFactorization(vaVals))
        {
            root = GetProductRootByCoordinates(vaVals);
            return root;
        }

        vit = s.begin();
        for(auto v:args)
        {
            auto va = vit++;
            copy.Eval(*va, v);
        }
        auto roots = copy.solutions(v);
        if(roots.size() == 0)
        {
            root = Solve(copy);
        }
        if(roots.size() == 1)
        {
            root = std::move(*roots.begin());
        }
        else
            IMPLEMENT
            
        return root;
    }
    
private:
    //f_t f;
};
}}
