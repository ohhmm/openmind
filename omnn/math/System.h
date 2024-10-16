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

class System // TODO: resolve current problem, it mixes-up conjunction with disjunction and sees not all roots, if you are dealing with serious system, consider using total equation instead or  fix this  class first   
{
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
    using solutions_t = Valuable::solutions_t;
    using es_t = std::map<std::set<Variable>,solutions_t>;
    using v_es_t = std::map<Variable, es_t>;
    using expressions = std::unordered_set<Valuable>;
    
    System& operator()(const Variable& v);
    System& operator<<(const Valuable& v);
    bool Add(const Variable&, const Valuable& v);
    bool Add(const Valuable& v);
    Valuable::var_set_t Vars() const;
    Valuable::var_set_t CollectVa(const Variable& v) const;
    bool Eval(const Variable&, const Valuable& v);
    bool Fetch(const Variable&);
    bool Has(const Valuable&) const;
    solutions_t Solve(const Variable& v);
    
    constexpr 
	virtual bool Validate() const {
        return true;
    }
    
    [[deprecated("Please, switch to MakesTotalEqu instead.")]]
    void MakeTotalEqu(bool makeTotalEqu) {
        this->makeTotalEqu = makeTotalEqu;
    }
    [[deprecated("Please, switch to MakesTotalEqu instead.")]]
    constexpr auto MakeTotalEqu() const { return makeTotalEqu; }

    void MakesTotalEqu(bool makeTotalEqu) { this->makeTotalEqu = makeTotalEqu; }
    constexpr auto MakesTotalEqu() const { return makeTotalEqu; }

    constexpr const Valuable& Total() const {
        if (makeTotalEqu)
            return sqs;
        else
            IMPLEMENT
    }

    Valuable CalculateTotalExpression() const;

	constexpr const auto& Expressions() const { return equs; }
    auto IsEmpty() const { return equs.empty(); }

    es_t& Yarns(const Variable& v) { return vEs[v]; }
    const solutions_t& Known(const Variable& v) { return Yarns(v)[{}]; }

private:
    expressions equs;
    v_es_t vEs;
    Valuable sqs;
    bool makeTotalEqu = {};
    bool doEarlyFetch = {};
//    solutions solus;
};

}}
