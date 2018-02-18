//
// Created by Sergej Krivonos on 17.02.18.
//
#pragma once
#include "Valuable.h"
#include "Variable.h"

namespace omnn{
namespace math {

class System
{
public:
    using equs_by_vars = std::map<
            std::set<Variable>, // from these vars
            Valuable >;         // this expressions
    using expressions = std::map< // expressions
            Variable, //of this va
            equs_by_vars>; // these expressions

    using solutions = std::map<
            Valuable, // solution id
            std::set<std::pair<
                Variable, // solutions of this var
                Valuable> >
            >; // these solutions, order does metter. common order for all solutions
    
    System& operator<<(const Valuable& v)
    {
        return *this;
    }
    
//    System(std::initializer_list<Valuable> l);
    bool Solve(const Variable& v)
    {
        
    }
    
    equs_by_vars& operator[](const Variable& v)
    {
        return equs[v];
    }

private:
    expressions equs;
    solutions solus;
};

}}
