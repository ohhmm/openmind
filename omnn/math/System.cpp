//
// Created by Sergej Krivonos on 17.02.18.
//
#include "System.h"
#include "Valuable.h"
#include <algorithm>
#if __has_include(<execution>)
#include <execution>
#endif
#include <numeric>

using namespace omnn;
using namespace math;

template<class T, class F> void peach(const T& t, const F& f) {
    auto b = std::begin(t), e = std::end(t);
    std::for_each(std::execution::par, b, e, f);
}

template <class T, class F> void each(const T& t, const F& f) {
    if (std::size(t) > 100)
        peach(t, f);
    else {
        auto b = std::begin(t), e = std::end(t);
        std::for_each(b, e, f);
    }
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
    return v != 0_v
        && es[v.Vars()].insert(v).second
        && Add(va.Equals(v));
}

bool System::Add(const Valuable& v)
{
    auto _ = v;
    _.SetView(Valuable::View::Equation);
    _.optimize();
    auto isNew = _ != 0_v &&
#if __has_include(<execution>)
        std::find(std::execution::par, std::begin(equs), std::end(equs), _) == equs.end()
        && std::find(std::execution::par, std::begin(equs), std::end(equs), -_) == equs.end();
#else
        std::find(std::begin(equs), std::end(equs), _) == equs.end()
        && std::find(std::begin(equs), std::end(equs), -_) == equs.end();
#endif

    if (isNew) {
        auto vars = v.Vars();
        if(vars.size() == 1){
            auto& va = *vars.begin();
            vEs[va][{}].emplace(v(va));
        } 

        equs.emplace(_);
        if (makeTotalEqu)
            sqs += _.Sq();
        if (doEarlyFetch)
            for (auto& va : _.Vars())
                for (auto& s : _.Solutions(va))
                    Add(va, s);
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
    if(!v.HasVa(va)) {
        auto subst = v.IsInt() || v.IsSimpleFraction();
        auto e = equs.end();
        bool again;
        do {
            again = {};
            auto prev = equs.begin();
            for(auto it = prev; !again && it != e; ++it)
            {
                auto& e = *it;
                if (e.HasVa(va))
                {
                    auto wasVars = e.Vars();
                    auto eva = e;
                    eva.Eval(va, v);
                    eva.optimize();
                    if (eva != 0) {
                        auto becameVars = eva.Vars();
                        if (becameVars.size()) {
                            if (subst
                                || std::includes(wasVars.begin(), wasVars.end(), becameVars.begin(), becameVars.end())
                                ) {
                                auto del = it;
                                auto b = equs.begin();
                                again = prev==b;
                                equs.erase(del);
                                modified = true;
                                Add(eva);
                                it = again ? equs.begin() : prev;
                            } else
                                modified = Add(eva) || modified;
                        }
                    }
                }
                prev = it;
            }
        } while (again);
        
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
    bool again;
    do {
        again = {};
        //std::all_of(equs, [&](auto it) { // todo: multithread System::Eval
            //auto& e = *it;
        for (auto& e : equs) {
            e.CollectVa(vars);
            if (e.HasVa(va)) {
                if (!e.IsSum() || e.as<Sum>().IsNormalizedPolynomial(va)) {
                    auto _ = e(va);
                    modified = Add(va, _) || modified;
                    if (_.Vars().size() == 0) {
                        fetched = true;
                    }

                    auto evaluated = Eval(va, _);
                    modified = evaluated || modified;
                    if (evaluated) {
                        again = !fetched;
                        break;
                    }
                }
            }
        }
		//);
    } while (again);
    
    if (!fetched) {
        vars.erase(va);
        each(vars,
             [&](auto& v){
                 modified = Fetch(v) || modified;
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

//    auto it = vEs.find(va);
//    if(it!=vEs.end()){
//        auto sit = it->second.find({});
//        if (sit!=it->second.end()) {
//            solution = sit->second;
//            if (solution.size()) {
//                solving.erase(va);
//                return solution;
//            }
//        }
//    }
    
    if(makeTotalEqu)
        solution = sqs.Solutions(va);
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
                auto otherVars = CollectVa(va);
                if (otherVars.empty()) {
                    auto& solved = vEs[va][{}];
                    if (solved.size()) {
                        return solved;
                    }
                }
                else for(auto& v : otherVars)
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
                        modified = Eval(v, toEval) || modified;
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
