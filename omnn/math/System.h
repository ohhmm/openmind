//
// Created by Sergej Krivonos on 17.02.18.
//
#pragma once
#include "Sum.h"
#include "Valuable.h"
#include "Variable.h"
#include "Product.h"
#include "Fraction.h"
#include "Exponentiation.h"

#include <map>
#include <set>
#include <memory>

#include <boost/numeric/ublas/matrix.hpp>


namespace omnn{
namespace math {

class System;
std::ostream& operator<<(std::ostream& os, const System& sys);

    using system_base_t = Valuable::expressions_t;

class System
    : public system_base_t
{
    using base = system_base_t;

    using var_set_t = Valuable::var_set_t;

    var_set_t solving;
    var_set_t fetched;

    class InProgress {
        bool wasInProgress = {};
        Variable v;
        var_set_t& varsInProgress;

    public:
        InProgress(var_set_t& varset, const Variable& v);
        ~InProgress();
        constexpr operator bool() const { return wasInProgress; }
    };

    bool UnmarkVariablesFetched(const var_set_t& unmark);

public:
    using base::const_iterator;
    using base::const_pointer;
    using base::const_reference;
    using base::difference_type;
    using base::iterator;
    using base::pointer;
    using base::reference;
    using base::value_type;

    using expressions_t = System;
    using solutions_t = Valuable::solutions_t;
    using es_t = std::map<var_set_t, expressions_t>; // expressions set: set of variables => set of expressions
    using es_ptr_t = ptrs::shared_ptr<es_t>;
    using v_es_t = std::map<Variable, es_ptr_t>; // variable => [(set of other variables in expression) => expressions of the key variable from equations of the system]
                                      // NOTE: empty set means the key variable is expressed therefore has known values
    using v_es_ptr_t = ptrs::shared_ptr<v_es_t>;
    
    using base::begin;
    using base::end;
    using base::emplace;
    using base::empty;
    using base::erase;
    using base::find;
    using base::size;

    System() : vEs(ptrs::make_shared<v_es_t>()) {}
    System(v_es_ptr_t yarns) : vEs(std::move(yarns)) {}

    /**
     * Construct a system from a matrix where columns represent variables
     * @param matrix The matrix to construct the system from
     */
    System(const boost::numeric::ublas::matrix<Valuable>& matrix);


    System& operator()(const Variable&);
    System& operator<<(const Valuable&);
    bool Add(const Variable&, const Valuable&);
    bool Add(const Valuable&);
    Valuable::var_set_t Vars() const;
    Valuable::var_set_t CollectVarsFromEquationsWithThisVar(const Variable&) const;
    bool Eval(const Variable&, const Valuable&);
    bool Fetch(const Variable&);
    bool Has(const Valuable&) const;
    solutions_t Solve(const Variable&);
    
    constexpr 
	virtual bool Validate() const {
        return true;
    }

    bool Test(const Valuable::vars_cont_t&) const;

    void MakesTotalEqu(bool makeTotalEqu) { this->makeTotalEqu = makeTotalEqu; }
    constexpr auto MakesTotalEqu() const { return makeTotalEqu; }

    constexpr const Valuable& Total() const {
        if (makeTotalEqu)
            return sqs;
        else
            IMPLEMENT
    }

    Valuable CalculateTotalExpression() const;

	constexpr const Valuable::expressions_t& Expressions() const { return *this; }

    auto IsEmpty() const { return empty(); }

    es_t& Yarns(const Variable&);
    bool Inquire(const Variable&, const Valuable&);
    const solutions_t& Known(const Variable& v) { return Yarns(v)[{}]; }
    bool EvalInvariantKnowns(Valuable&);

private:
    v_es_ptr_t vEs;
    Valuable sqs;
    bool makeTotalEqu = {};
    bool doEarlyFetch = {};
    Valuable totalEqusComplexity;
};

}}
