//
// Created by Sergej Krivonos on 17.02.18.
//

#include "System.h"

using namespace omnn;
using namespace math;

System& System::operator<<(const Valuable& v)
{
    auto _ = v;
    _.optimize();
    equs.insert(_);
    return *this;
}

System::solutions_t System::Solve(const Variable& v)
{
    solutions_t solution;
    if(Validate()){
        auto& es = vEs[v];
        
        auto equsSzWas = equs.size();
        bool modified = {};
        for(auto& e : equs)
        {
            if (e.HasVa(v))
            {
                for(auto& _ : e(v))
                {
                    auto vars = _.Vars();
                    modified = modified || es[vars].insert(_).second;
                    equs.insert(_ - v);
                }
            }
        }
        modified = modified || equsSzWas < equs.size();
        
        if (modified) {
            std::vector<Variable> singleVars;
            for(auto& esi : es)
            {
                if(esi.first.size() == 0)
                {
                    for(auto& s : esi.second)
                        solution.insert(s);
                    break;
                }
                else if(esi.first.size() == 1)
                {
                    singleVars.push_back(*esi.first.begin());
                }
            }
            
            if(!solution.size())
            {
                for(auto& v : singleVars)
                {
                    auto s = Solve(v);
                }
                
                for(auto& esi : es)
                {
                    if(esi.first.size() == 0)
                    {
                        for(auto& _ : esi.second)
                            solution.insert(_);
                    }
                }
            }
        }
    }
    else
    {
        throw "Contradiction!";
    }
    return solution;
}
