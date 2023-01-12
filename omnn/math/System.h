//
// Created by Sergej Krivonos on 17.02.18.
//
#pragma once
#include "Sum.h"
#include "Valuable.h"
#include "Variable.h"

namespace omnn{
namespace math {

class System // TODO: resolve current problem, it mixes-up conjunction with disjunction and sees not all roots, if you are dealing with serious system, consider using total equation instead or  fix this  class first   
{
    std::set<Variable> solving;
    std::set<Variable> fetching;
    
    struct VaData
    {
        
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
    Valuable::var_set_t CollectVa(const Variable& v) const;
    bool Eval(const Variable&, const Valuable& v);
    bool Fetch(const Variable&);
    solutions_t Solve(const Variable& v);
    
    bool Validate() {
        return true;
    }
    
    void MakeTotalEqu(bool makeTotalEqu) {
        this->makeTotalEqu = makeTotalEqu;
    }

    Valuable Total() const {
        if (makeTotalEqu)
            return sqs;
        else
            IMPLEMENT
    }
    
private:
    expressions equs;
    v_es_t vEs;
    Valuable sqs;
    bool makeTotalEqu = {};
    bool doEarlyFetch = {};
//    solutions solus;
};

}}
