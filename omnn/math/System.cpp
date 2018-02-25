//
// Created by Sergej Krivonos on 17.02.18.
//
#include "System.h"
#include "Valuable.h"

using namespace omnn;
using namespace math;

System& System::operator<<(const Valuable& v)
{
    auto _ = v;
    _.optimize();
    equs.insert(_);
    return *this;
}

bool System::Add(const Variable& va, const Valuable& v)
{
    auto& es = vEs[va];
    auto vars = v.Vars();
    bool isNew = es[vars].insert(v).second;
    if (isNew)
    {
        equs.insert(v - va);
    }
    return isNew;
}

bool System::Eval(const Variable& va, const Valuable& v)
{
    bool modified = {};
    for(auto& e : equs)
    {
        if (e.HasVa(va))
        {
            auto eva = e;
            eva.Eval(va, v);
            eva.optimize();
            modified = modified || Add(va, eva);
        }
    }
    return modified;
}

System::solutions_t System::Solve(const Variable& va)
{
    solutions_t solution;
    
    if (solving.find(va) == solving.end()) {
        solving.insert(va);
    }
    else {
        return solution;
    }
        
    if(Validate()){
        auto& es = vEs[va];
        
        auto equsSzWas = equs.size();
        bool modified = {};
        Valuable::var_set_t vars;
        for(auto& e : equs)
        {
            e.CollectVa(vars);
            if (e.HasVa(va))
            {
                for(auto& _ : e(va))
                {
                    modified = modified || Add(va, _);
                }
            }
        }
        modified = modified || equsSzWas < equs.size();
        
        if (modified) {
            
            do{
                modified = {};
                for(auto& v : vars)
                {
                    if (v == va) {
                        continue;
                    }
                    
                    auto& vaFuncs = vEs[v];
                    int toSolve = vaFuncs.size();
                    if (!toSolve) {
                        if(Solve(v).size()) {
                            modified = true;
                        }
                        continue;
                    }
                    
                    auto mm = std::minmax(vaFuncs.begin(), vaFuncs.end(), [](auto& _1, auto& _2){
                        return _1->first.size() < _2->first.size();
                    });
                    --toSolve;
                    for(auto nParams = 0; toSolve; ++nParams)
                    {
                        for(auto& f : vaFuncs)
                        {
                            if (f.first.size() == nParams)
                            {
                                auto s = Solve(v);
                                auto cnt = s.size();
                                --toSolve;
                                if(cnt)
                                {
                                    modified = true;
                                }
                                break;
                            }
                        }
                    }
                }
            } while(modified);
            
            bool modified = {};
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
                    auto& v = *esi.first.begin();
                    singleVars.push_back(v);
                    for(auto& toEval : esi.second)
                    {
                        modified = modified || Eval(v, toEval);
                    }
                }
            }
            if (modified) {
                solving.erase(va);
                return Solve(va);
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
