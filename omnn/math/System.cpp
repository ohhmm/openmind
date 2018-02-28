//
// Created by Sergej Krivonos on 17.02.18.
//
#include "System.h"
#include "Valuable.h"
#include <numeric>

using namespace omnn;
using namespace math;

template<class T, class F>
void each(const T& t, const F& f)
{
    std::for_each(t.begin(), t.end(), f);
}


bool is_subset(const std::set<std::string>& subs, const std::set<std::string>& set)
{
    auto it = subs.begin();
    auto se = set.end();
    for(auto& m : subs)
    {
        it = std::find(it, se, m);
        if (it == se) {
            return false;
        }
    }
    return true;
}

System& System::operator()(const Variable& v)
{
    if(Fetch(v))
        Solve(v);
    return *this;
}

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

bool System::Add(const Valuable& v)
{
    bool isNew = equs.insert(v).second;
    if (isNew) {
        for(auto& va : v.Vars())
        {
            for(auto& s : v(va))
                Add(va, s);
        }
    }
    return isNew;
}

Valuable::var_set_t System::CollectVa(const Variable& va) const
{
    Valuable::var_set_t _;
    for(auto& e : equs)
        if (e.HasVa(va))
            e.CollectVa(_);
    _.erase(va);
    return _;
}

bool System::Eval(const Variable& va, const Valuable& v)
{
    bool modified = {};
    if(!v.HasVa(va))
        for(auto& e : equs)
        {
            if (e.HasVa(va))
            {
                auto eva = e;
                eva.Eval(va, v);
                eva.optimize();
                modified = modified || Add(eva);
            }
        }
    return modified;
}

bool System::Fetch(const Variable& va)
{
    bool modified = {};
    
    if (fetching.find(va) == fetching.end()) {
        fetching.insert(va);
    }
    else {
        return modified;
    }
    
    Valuable::var_set_t vars;
    for(auto& e : equs)
    {
        e.CollectVa(vars);
        if (e.HasVa(va))
            for(auto& _ : e(va))
                modified = modified || Add(va, _);
    }
    
    vars.erase(va);
    std::for_each(vars.begin(), vars.end(),
                  std::bind(&System::Fetch, this, std::placeholders::_1));
    
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
        
    if(Validate())
    {
        if (Fetch(va))
        {
            bool modified;
            do
            {
                modified = {};
                for(auto& v : CollectVa(va))
                {
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
            
            auto& es = vEs[va];
            modified = {};
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
                else
                {
                    Valuable::var_set_t vset;
                    each(esi.first, [&](auto& v){vset.insert(v);});
                    vset.erase(va);
                    for(auto& v: vset)
                        Solve(v);
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
