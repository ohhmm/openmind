//
// Created by Sergej Krivonos on 17.02.18.
//
#include "System.h"
#include "Valuable.h"
#include <execution>
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
    Add(v);
    return *this;
}

bool System::Add(const Variable& va, const Valuable& v)
{
    auto& es = vEs[va];
    auto vars = v.Vars();
    bool isNew = v != 0_v
        ? es[vars].insert(v).second
        : false;
    if (isNew)
    {
        *this << v - va;
    }
    return isNew;
}

bool System::Add(const Valuable& v)
{
    auto _ = v;
    _.SetView(Valuable::View::Equation);
    _.optimize();
    bool isNew = _ != 0_v
        ? std::find(std::execution::par, std::begin(equs), std::end(equs), -_) == equs.end() && equs.insert(_).second
        : false;
    
    if (isNew) {
        if (makeTotalEqu)
            sqs += _.Sq();
        if (doEarlyFetch)
        {
            for (auto& va : _.Vars())
            {
                for (auto& s : _(va))
                    Add(va, s);
            }
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
                modified = Add(eva) || modified;
            }
        }
    return modified;
}

bool System::Fetch(const Variable& va)
{
    bool modified = {};
    bool fetched = {};
    
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
            {
                modified = Add(va, _) || modified;
                if (_.Vars().size() == 0) {
                    fetched = true;
                }
                modified = Eval(va, _) || modified;
            }
    }
    
    if (!fetched) {
        vars.erase(va);
        each(vars,
             [&](auto& v){
                 modified = modified || Fetch(v);
             });
        
        fetching.erase(va);
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

    //solution = sqs(va);
    if (solution.size()) {
        Valuable::var_set_t vars;
        for(auto& s : solution)
        {
            auto vaSzWas = vars.size();
            s.CollectVa(vars);
            if (vaSzWas!=vars.size()) {
                Add(va, s);
            }
        }
        
        if (!vars.size()) {
            return solution;
        } else {
            solution.clear();
        }
//        
//   -     if(vars.size()==1 && solution.size()==2)
//        {
//            auto v = *vars.begin();
//            auto s = solution.begin();
//   -         auto sum = *s; ++s; sum -= *s;
//            auto vo = sum(v);
//            if (vo.size() != 1) {
//                IMPLEMENT
//            } else {
//                auto vaVal = *vo.begin();
//                if (vaVal.FindVa()) {
//                    IMPLEMENT
//                } else {
//                    solutions_t news;
//                    for(auto s : solution)
//                    {
//                        s.Eval(v, vaVal);
//                        s.optimize();
//                        news.insert(s);
//                    }
//                    return news;
//                }
//            }
//        }
//        else
//        {
//            for(auto& v : vars)
//            {
//                solutions_t solutions;
//                for(auto& s : Solve(v))
//                {
//                    
//                    for(auto& ss : solution)
//                    {
//                        auto _ = ss;
//                        _.Eval(v, s);
//                        auto fv = _.FindVa();
//                        if (fv) {
//                            auto t = Solve(*fv);
//                            if(t.size()!=1)
//                                IMPLEMENT
//                            _.Eval(*fv, *t.begin());
//                            if (_.FindVa()) {
//                                IMPLEMENT
//                            }
//                        }
//                        
//                        solutions.insert(_);
//                    }
//                }
//            }
//        }
    }
    
    if(Validate())
    {
        while (Fetch(va))
        {
            bool modified;
            do
            {
                modified = {};
                for(auto& v : CollectVa(va))
                {
                    auto& vaFuncs = vEs[v];
                    auto toSolve = vaFuncs.size();
                    if (!toSolve) {
                        if(Solve(v).size()) {
                            modified = true;
                        }
                        continue;
                    }
                    
                    //auto mm = std::minmax(vaFuncs.begin(), vaFuncs.end(), [](auto& _1, auto& _2){
                    //    return _1->first.size() < _2->first.size();
                    //});
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
    
    solving.erase(va);
    return solution;
}
