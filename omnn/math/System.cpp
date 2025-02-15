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

#include "omnn/rt/antiloop.hpp"
#include "omnn/rt/each.hpp"


using namespace omnn;
using namespace math;

std::ostream& operator<<(std::ostream& os, const System& sys) {
    for (const auto& eq : sys) {
        os << eq << std::endl;
    }
    return os;
}

bool is_subset(const auto& smaller_set, const auto& larger_set) {
    return std::includes(larger_set.begin(), larger_set.end(), smaller_set.begin(), smaller_set.end());
}

System& System::operator()(const Variable& variable)
{
    if(Fetch(variable))
        Solve(variable);
    return *this;
}

System& System::operator<<(const Valuable& expression)
{
    Add(expression);
    return *this;
}

bool System::UnmarkVariablesFetched(const var_set_t& unmark) {
    bool alter = {};
    for (auto& variable : unmark) {
        auto erased = fetched.erase(variable);
        alter = erased || alter;
    }
    return alter;
}

bool System::Inquire(const Variable& va, const Valuable& expression)
{
    auto& es = Yarns(va);
    auto alter = !expression.IsZero();
    if (alter) {
        auto vars = expression.Vars();
        auto it = es.find(vars);
        if (it == es.end()) {
            auto emplaced = es.emplace(vars, expressions_t(vEs));
            it = emplaced.first;
        } 
        auto& expressions = it->second;
        alter = expressions.insert(expression).second;
    }
    if (alter) {
        auto vars = expression.Vars();
        vars.insert(va);
        UnmarkVariablesFetched(vars);
    }
    return alter;
}

bool System::Add(const Variable& va, const Valuable& expression)
{
    auto alter = Inquire(va, expression);
    if (alter) {
        auto equals = va.Equals(expression);
        alter = Add(equals);
        UnmarkVariablesFetched(equals.Vars());
    }
    return alter;
}

bool System::Test(const Valuable::vars_cont_t& values) const {
    auto DoesEquationSatisfyGivenValues = [&](auto equation) {
        equation.eval(values);
        auto satisfied = equation.Optimized().IsZero();
        return satisfied;
    };
    return std::all_of(begin(), end(), DoesEquationSatisfyGivenValues);
}

bool System::Has(const Valuable& equation) const
{
    if (!equation.IsEquation() || !equation.is_optimized()) {
        auto copy = equation;
        copy.SetView(Valuable::View::Equation); // TODO : start optimizing from Unification
        try {
            Valuable::OptimizeOn o;
            try {
                copy.optimize();
            } catch (...) {
                std::cerr << "Exception optimizing: " << equation;
                if (equation != copy) {
                    std::cerr << " -> " << copy;
                }
                std::cerr << std::endl;
            }
            if (copy.is_optimized()) {
                return Has(copy);
            }
#if !defined(NDEBUG) && !defined(NOOMDEBUG)
            else
            {
                LOG_AND_IMPLEMENT("Unoptimizable equation: " << equation);
            }
#endif
        } catch (...) {
        }
    }
    auto b = begin();
    auto e = end();
    return equation.IsZero() ||
           find(equation) != e ||
           find(-equation) != e;
}

bool System::Add(const Valuable& v)
{
    auto isNew = !Has(v);
    if (isNew) {
        Valuable _;
        try {
            _ = v.Optimized(Valuable::View::Equation);
        } catch (...) {
            _ = v;
        }
        
        auto vars = _.Vars();
        if(vars.size() == 1){
            auto& va = *vars.begin();
            _.solve(va, Yarns(va)[{}]);
        } else {
            for (auto& va : vars) {
                auto& es = Yarns(va);
                es[vars].insert(_.Link());
            }
        }

        emplace(_);
        if (makeTotalEqu) {
            Valuable::OptimizeOn o;
            sqs += _.Sq();
        }
        if (doEarlyFetch)
            for (auto& va : _.Vars())
                for (auto& s : _.Solutions(va))
                    Inquire(va, s);
    }
    return isNew;
}

Valuable::var_set_t System::Vars() const {
    Valuable::var_set_t _;
    for (auto& e : Expressions())
        e.CollectVa(_);
    return _;
}

Valuable::var_set_t System::CollectVarsFromEquationsWithThisVar(const Variable& va) const
{
    Valuable::var_set_t _;
    for (auto& e : Expressions())
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
        auto e = end();
        bool again;
        do {
            again = {};
            auto prev = begin();
            for(auto it = prev; !again && it != e; ++it)
            {
                auto& e = *it;
                if (e.HasVa(va))
                {
                    auto wasVars = e.Vars();
                    auto eva = e;
                    eva.Eval(va, v);
                    eva.optimize();
                    if (!eva.IsZero()) {
                        auto becameVars = eva.Vars();
                        if (becameVars.size()) {
                            if (subst
                                || std::includes(wasVars.begin(), wasVars.end(), becameVars.begin(), becameVars.end())
                                ) {
                                auto del = it;
                                auto b = begin();
                                again = prev==b;
                                erase(del);
                                modified = true;
                                Add(eva);
                                it = again ? begin() : prev;
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

    ::omnn::rt::LoopDetectionGuard<Variable> fetching(va);
    if (fetching.isLoopDetected()) {
#if !defined(NDEBUG) && !defined(NOOMDEBUG)
        std::cout << "System::Fetch loop for " << va << std::endl;
#endif
        return modified;
    };

    bool fetched = {};

    Valuable::var_set_t vars;
    bool again;
    do {
        again = {};
        for (auto& e : Expressions()) {
            try {
                e.CollectVa(vars);
                if (e.HasVa(va)) {



                    //if (!e.IsSum() || e.as<Sum>().IsPolynomial(va)) {
                    //    auto _ = e(va);
                    //    modified = Add(va, _) || modified;
                    //    if (_.FindVa() == nullptr) {
                    //        fetched = true;
                    //    }

                    //    auto evaluated = Eval(va, _);
                    //    modified = evaluated || modified;
                    //    if (evaluated) {
                    //        again = !fetched;
                    //        break;
                    //    }
                    //}




                    bool evaluated = {};
                    for (auto& sol : e.solve(va)) {
                        auto addedNewOne = Inquire(va, sol);
                        modified = addedNewOne || modified;
                        if (sol.FindVa() == nullptr) {
                            fetched = true;
                        }
                        evaluated = (addedNewOne && Eval(va, sol)) || evaluated;
                        modified = evaluated || modified;
                    }
                    if (evaluated) {
                        again = !fetched;
                        break;
                    }
                }
            } catch (...) {

            }
        }

        for (auto& [subSystemVars, subsystem] : Yarns(va)) {
            if (subsystem.Fetch(va)) {
                modified = true; // this means that subsystem obtained info on new yarns
                // synchronize new yarns into this system
                for (auto& yarn : subsystem.Yarns(va)) {
                    auto& thisYarn = Yarns(va)[yarn.first];
                    thisYarn.insert(yarn.second.begin(), yarn.second.end());
                }

            }

        }
		//);
    } while (again);
    
    if (!fetched) {
        vars.erase(va);
        rt::each(vars,
			[&](auto& v){
				auto fetchModified = Fetch(v);
				modified |= fetchModified;
				return fetchModified;
			});
    }

    if (modified) {
        modified = this->fetched.insert(va).second;
    }
    return modified;
}

System::solutions_t System::Solve(const Variable& va)
{
    auto solutions = Known(va);
    InProgress SolvingInProgress(solving, va);
    if (SolvingInProgress)
        return solutions;

    if (makeTotalEqu) {
        auto vars = sqs.Vars();
        for (auto& v : vars) {
            auto& solved = Known(v);
            if (solved.size()) {
                if (solved.size() == 1) {
                    if (v != va) {
                        sqs.Eval(v, *solved.begin());
                    } else {
                        solutions = solved;
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
			solutions = sqs.Solutions(va);
        // else ?
    }
    
    if (solutions.size()) {
        Valuable::var_set_t vars;
        for(auto& s : solutions)
        {
            auto vaSzWas = vars.size();
            s.CollectVa(vars);
            if (vaSzWas!=vars.size()) {
                Inquire(va, s);
            }
        }
        
        if (!vars.size()) {
            return solutions;
        } else {
            solutions.clear();
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
                auto otherVars = CollectVarsFromEquationsWithThisVar(va);
                if (otherVars.empty()) {
                    auto& solved = Known(va);
                    if (solved.size()) {
                        return solved;
                    }
                }
                else for(auto& v : otherVars)
                {
                    auto ves = *vEs;
                    auto& vaFuncs = *ves[v];
                    auto toSolve = vaFuncs.size();
                    if (!toSolve) {
                        auto numKnownSolutions = Known(v).size();
                        if (Solve(v).size() - numKnownSolutions > 0) {
                            modified = true;
                        }
                        continue;
                    }
                    
                    auto mm = std::minmax_element(vaFuncs.begin(), vaFuncs.end(),
                                                  [](auto& _1, auto& _2) { return _1.first.size() < _2.first.size(); });
                    auto nParamsFrom = mm.first != vaFuncs.end() ? mm.first->first.size() : 0;
                    auto nParamsTo = mm.second != vaFuncs.end() ? mm.second->first.size() : 0;
                    --toSolve;
                    for (auto nParams = nParamsFrom;
                        toSolve > 0 && nParams <= nParamsTo;
                        ++nParams)
                    {
                        for(auto& func : vaFuncs)
                        {
                            if (func.first.size() == nParams)
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
            
            auto& es = Yarns(va);
            modified = {};
            std::vector<Variable> singleVars;
            for(auto& esi : es)
            {
                if(esi.first.size() == 0)
                {
                    for(auto& s : esi.second)
                        solutions.insert(s);
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
                    for (auto& variable : esi.first) {
                        vset.insert(variable);
                    }
                    vset.erase(va);
                    for(auto& v: vset)
                        Solve(v);
                }
                
            }
            
            if(!solutions.size())
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
                            solutions.insert(_);
                    }
                }
            }
        }
    }
    else
    {
        throw "Contradiction!";
    }
    
    if (solutions.empty()) {
        solutions = Known(va);
    }

    if (solutions.empty() && !makeTotalEqu) {
        auto total = CalculateTotalExpression();
        auto start = true;

        while (EvalInvariantKnowns(total) || start) {
            start = {};
            total.optimize();
            auto vars = total.Vars();
            if (vars.size() == 1 && vars.begin()->Same(va)) {
                total.solve(va, solutions);
                for (auto& solution : solutions) {
                    Inquire(va, solution);
                }
            } else {
                for (auto& variable : vars) {
                    if (variable != va) {
                        try {
                            if (total.IsPolynomial(variable)) {
                                auto coefficients = total.Coefficients(variable);
                                if (coefficients.size()<=3) {
                                    auto sol = total(variable);
                                    Add(variable, std::move(sol));
                                } else if(total.IsSum()) {
                                    decltype(solutions) sols;
                                    total.as<Sum>().solve(variable, sols, coefficients);
                                    for (auto& sol : sols) {
                                        Add(variable, std::move(sol));
                                    }
                                }
                                continue;
                            }
                        } catch (...) {
                        }
                        try {
                            for (auto& sol : total.Solutions(variable)) {
                                Inquire(variable, sol);
                            }
                        } catch (...) {
                        }
                    }
                }
            }
        }
    }

    return solutions;
}

Valuable System::CalculateTotalExpression() const {
    Sum total; // TODO: use intersection for same varset expressions
    for (auto& e : Expressions()) {
		total.Add(e.Sq());
	}
    return total;
}

System::es_t& System::Yarns(const Variable& variable) {
    auto& pExpressedVariable = vEs->operator[](variable);
    if (!pExpressedVariable) {
        pExpressedVariable = ptrs::make_shared<es_t>();
    }
    return *pExpressedVariable;
}

bool System::EvalInvariantKnowns(Valuable& expression) {
    Valuable::vars_cont_t evaluate;
    auto vars = expression.Vars();
    for (auto& variable : vars) {
        auto& known = Known(variable);
        if (known.size() == 1) {
            evaluate.emplace(variable, known.begin()->Link());
        }
    }
    return evaluate.size() > 0
        && expression.eval(evaluate);
}

System::InProgress::InProgress(std::set<Variable>& varset, const Variable& v)
    : varsInProgress(varset)
{
    this->v = v;
    auto insertion = varsInProgress.insert(v);
    auto inserted = insertion.second;
    wasInProgress = !inserted;
}

System::InProgress::~InProgress() {
    if (!wasInProgress)
        varsInProgress.erase(v);
}
