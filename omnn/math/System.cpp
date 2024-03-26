//
// Created by Sergej Krivonos on 17.02.18.
//
#include "System.h"
#include "Valuable.h"

#include <algorithm>
#include <future>
#include <iostream>
#include <numeric>
#include <set>

#include <rt/each.hpp>


using namespace omnn;
using namespace math;


bool is_subset(const auto& smaller_set, const auto& larger_set) {
    return std::includes(larger_set.begin(), larger_set.end(), smaller_set.begin(), smaller_set.end());
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
    return !v.IsZero()
        && es[v.Vars()].insert(v).second
        && Add(va.Equals(v));
}

bool System::Has(const Valuable& e) const
{
    if (!e.IsEquation() || !e.is_optimized()) {
        auto copy = e;
        copy.SetView(Valuable::View::Equation); // TODO : start optimizing from Unification
        try {
            Valuable::OptimizeOn o;
            copy.optimize();
            if (e.is_optimized()) {
                return Has(copy);
            }
#if !defined(NDEBUG) && !defined(NOOMDEBUG)
            else
            {
                LOG_AND_IMPLEMENT("Unoptimizable equation: " << e);
            }
#endif
        } catch (...) {
        }
    }
    return e.IsZero() ||
#ifndef __APPLE__
           std::find(std::execution::par, std::begin(equs), std::end(equs), e) != equs.end() ||
           std::find(std::execution::par, std::begin(equs), std::end(equs), -e) != equs.end();
#else
           std::find(std::begin(equs), std::end(equs), e) != equs.end() ||
           std::find(std::begin(equs), std::end(equs), -e) != equs.end();
#endif
}

bool System::Add(const Valuable& v)
{
    auto isNew = !Has(v);
    if (isNew) {
        auto _ = v;
        if (!_.IsEquation() || !Valuable::optimizations) {
            _.SetView(Valuable::View::Equation); // TODO : start optimizing from Unification
            try {
                Valuable::OptimizeOn o;
                _.optimize();
            } catch (...) {
                _ = v;
            }
        }
        auto vars = _.Vars();
        if(vars.size() == 1){
            auto& va = *vars.begin();
            _.solve(va, vEs[va][{}]);
        } 

        equs.emplace(_);
        if (makeTotalEqu) {
            Valuable::OptimizeOn o;
            sqs += _.Sq();
        }
        if (doEarlyFetch)
            for (auto& va : _.Vars())
                for (auto& s : _.Solutions(va))
                    Add(va, s);
    }
    return isNew;
}

Valuable::var_set_t System::Vars() const {
    Valuable::var_set_t _;
    for (auto& e : equs)
        e.CollectVa(_);
    return _;
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
    InProgress AlreadyFetchedBefore(fetched, va);
    if (AlreadyFetchedBefore)
        return modified;

    InProgress FetchingInProgress(fetching, va);
    if (FetchingInProgress)
        return modified;

    bool fetched = {};

    Valuable::var_set_t vars;
    bool again;
    do {
        again = {};
        //std::all_of(equs, [&](auto it) { // todo: multithread System::Eval
            //auto& e = *it;
        for (auto& e : equs) {
            e.CollectVa(vars);
            if (e.HasVa(va)) {
                if (!e.IsSum() || e.as<Sum>().IsPolynomial(va)) {
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
				auto fetchModified = Fetch(v);
				modified = fetchModified || modified;
				return fetchModified;
			});
    }

    if (modified) {
        this->fetched.insert(va);
    }
    return modified;
}

System::solutions_t System::Solve(const Variable& va)
{
    solutions_t solution;
    InProgress SolvingInProgress(solving, va);
    if (SolvingInProgress)
        return solution;

//    auto it = vEs.find(va);
//    if(it!=vEs.end()){
//        auto sit = it->second.find({});
//        if (sit!=it->second.end()) {
//            solution = sit->second;
//            if (solution.size()) {
//                return solution;
//            }
//        }
//    }

    if (makeTotalEqu) {
        auto& solved = Known(va);
        if (solved.size())
            return solved;

        auto vars = sqs.Vars();
        for (auto& v : vars) {
            auto& solved = Known(v);
            if (solved.size()) {
                if (solved.size() == 1) {
                    if (v != va) {
                        sqs.Eval(v, *solved.begin());
                    } else {
                        solution = solved;
                        break;
                    }
                } else {
                    std::stringstream ss;
                    for(auto& s : solved)
                        ss << ' ' << s;
                    LOG_AND_IMPLEMENT(va << " has multiple solutions: " << ss.str());
                }
            }
        }
        sqs.optimize();
        if (sqs.HasVa(va))
			solution = sqs.Solutions(va);
        // else ?
    }
    
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
                        auto numKnownSolutions = Known(v).size();
                        if (Solve(v).size() - numKnownSolutions > 0) {
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
                                auto numKnownSolutions = Known(v).size();
                                auto s = Solve(v);
                                auto cnt = s.size() - numKnownSolutions;
                                --toSolve;
                                if(cnt > 0)
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
                    each(esi.first, [&](auto& v){ return vset.insert(v).second; });
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
    
    return solution;
}

Valuable System::CalculateTotalExpression() const {
    Sum total;
    for (auto& e : equs) {
		total.Add(e.Sq());
	}
    return total;
}
