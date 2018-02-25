//
// Created by Sergej Krivonos on 17.02.18.
//
#pragma once
#include "Sum.h"
#include "Valuable.h"
#include "Variable.h"

namespace omnn{
namespace math {

class System
{
    std::set<Variable> solving;
    
public:
    using solutions_t = Valuable::solutions_t;
    using es_t = std::map<std::set<Variable>,solutions_t>;
    using v_es_t = std::map<Variable, es_t>;
    using expressions = std::unordered_set<Valuable>;
    
    System& operator<<(const Valuable& v);
    bool Add(const Variable&, const Valuable& v);
    bool Eval(const Variable&, const Valuable& v);
    solutions_t Solve(const Variable& v);
    
    bool Validate() {
        return true;
    }
    
private:
    expressions equs;
    v_es_t vEs;
//    solutions solus;
};

}}
