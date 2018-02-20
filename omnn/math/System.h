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
    using solution_t = Valuable::solutions_t;
//    using by_vars_set = Valuable::var_set_t;
//    using equs_by_vars = std::map<
//            by_vars_set, // from these vars
//            solutions_t >;  // this expressions
//
//    using expressions = std::map< // expressions
//            Variable, //of this va
//            std::set<std::shared_ptr<equs_by_vars> >// these expressions
//        >;
//
//    using solutions = std::map<
//            Valuable, // solution id
//            std::set<std::pair<
//                Variable, // solutions of this var
//                Valuable> >
//            >; // these solutions, order does metter. common order for all solutions
//
    using expressions = std::vector<Valuable>;
    
    System& operator<<(const Valuable& v);
    
    solution_t Solve(const Variable& v);
    
    bool Validate() {
        return true;
    }
    
private:
    expressions equs;
//    solutions solus;
};

}}
