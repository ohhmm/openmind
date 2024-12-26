//
// Created by Sergej Krivonos on 17.02.18.
//
#pragma once
#include "Sum.h"
#include "Valuable.h"
#include "Variable.h"

#include <map>

namespace omnn{
namespace math {

    using system_base_t = Valuable::expressions_t;

class System
    : public system_base_t
{
    using base = system_base_t;

    std::set<Variable> solving;
    std::set<Variable> fetching;
    std::set<Variable> fetched;

    class InProgress {
        bool wasInProgress = {};
        Variable v;
        std::set<Variable>& varsInProgress;

    public:
        InProgress(std::set<Variable>& varset, const Variable& v)
            : varsInProgress(varset)
        {
            this->v = v;
            auto insertion = varsInProgress.insert(v);
            auto inserted = insertion.second;
            wasInProgress = !inserted;
        }

        operator bool() const { return wasInProgress; }

        ~InProgress() {
            if (!wasInProgress)
                varsInProgress.erase(v);
        }
    };

public:
    using base::const_iterator;
    using base::const_pointer;
    using base::const_reference;
    using base::difference_type;
    using base::iterator;
    using base::pointer;
    using base::reference;
    using base::value_type;

    using expressions_t = System;
    using solutions_t = Valuable::solutions_t;
    using es_t = std::map<std::set<Variable>, expressions_t>; // expressions set: set of variables => set of expressions
    using es_ptr_t = ptrs::shared_ptr<es_t>;
    using v_es_t = std::map<Variable, es_ptr_t>; // variable => [(set of other variables in expression) => expressions of the key variable from equations of the system]
                                      // NOTE: empty set means the key variable is expressed therefore has known values
    
    using base::begin;
    using base::end;
    using base::emplace;
    using base::empty;
    using base::erase;
    using base::find;
    using base::size;


    System& operator()(const Variable& v);
    System& operator<<(const Valuable& v);
    bool Add(const Variable&, const Valuable& v);
    bool Add(const Valuable& v);
    Valuable::var_set_t Vars() const;
    Valuable::var_set_t CollectVarsFromEquationsWithThisVar(const Variable& v) const;
    bool Eval(const Variable&, const Valuable& v);
    bool Fetch(const Variable&);
    bool Has(const Valuable&) const;
    solutions_t Solve(const Variable& v);
    
    constexpr 
	virtual bool Validate() const {
        return true;
    }

    bool Test(const Valuable::vars_cont_t&) const;

    void MakesTotalEqu(bool makeTotalEqu) { this->makeTotalEqu = makeTotalEqu; }
    constexpr auto MakesTotalEqu() const { return makeTotalEqu; }

    constexpr const Valuable& Total() const {
        if (makeTotalEqu)
            return sqs;
        else
            IMPLEMENT
    }

    Valuable CalculateTotalExpression() const;

	constexpr const Valuable::expressions_t& Expressions() const { return *this; }

    auto IsEmpty() const { return empty(); }

    es_t& Yarns(const Variable& v);
    const solutions_t& Known(const Variable& v) { return Yarns(v)[{}]; }

private:
    v_es_t vEs;
    Valuable sqs;
    bool makeTotalEqu = {};
    bool doEarlyFetch = {};
    Valuable totalEqusComplexity;
};

}}
