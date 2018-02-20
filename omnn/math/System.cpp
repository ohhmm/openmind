//
// Created by Sergej Krivonos on 17.02.18.
//

#include "System.h"

using namespace omnn;
using namespace math;

System& System::operator<<(const Valuable& v)
{
    equs.push_back(v);
    equs[equs.size()-1].optimize();
    return *this;
}

System::solution_t System::Solve(const Variable& v)
{
    solution_t solution;
    if(Validate()){
        std::set<const Valuable*> withV;
        for(auto& e : equs)
        {
            if (e.HasVa(v))
            {
                withV.insert(&e);
            }
        }
        
        std::map<
            std::set<Variable>,
            std::vector<Valuable>
        > es;
        
        for(auto w : withV)
        {
            for(auto& e : (*w)(v))
            {
                es[e.Vars()].push_back(e);
            }
        }
        
        for(auto& esi : es)
        {
            
        }
    }
    else
    {
        throw "Contradiction!";
    }
    return solution;
}
