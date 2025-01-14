//
// Created by Сергей Кривонос on 25.09.17.
//
#include "Variable.h"
#include "Exponentiation.h"
#include "Integer.h"
#include "Modulo.h"
#include "Product.h"
#include "Sum.h"
#include "VarHost.h"

namespace omnn{
namespace math {

    Variable::Variable()
    : varSetHost(&VarHost::Global<>(), [](auto){})
    , varId(VarHost::Global<>().NewVarId())
    {
        hash = varSetHost->Hash(varId);
        maxVaExp=1;
    }
    
    Variable::Variable(const Variable& v)
    : varSetHost(v.varSetHost)
    , varId(v.varSetHost->CloneId(v.varId))
    {
        hash = v.Hash();
        maxVaExp=1;
    }
    
    void Variable::SetId(::std::any id) {
        hash = varSetHost->Hash(id);
        varId = std::move(id);
    }

    const ::std::any& Variable::GetId() const {
        return varId;
    }
    
    Variable::Variable(const VarHost::ptr varHost)
    : varSetHost(varHost)
    {
        if(!varHost)
            throw "the varHost is mandatory parameter";
        varId = varHost->NewVarId();
        maxVaExp=1;
    }
    
    Valuable Variable::operator -() const
    {
        return Product{-1, *this};
    }
    
    Valuable& Variable::operator +=(const Valuable& v)
    {
        if(operator==(v))
        {
            Become(Product{2, *this});
        }
        else
            Become(Sum{*this, v});

        return *this;
    }
    
    Valuable& Variable::operator *=(const Valuable& v)
    {
        if (Same(v)) {
            return Become(Exponentiation(*this, 2));
        }
        else if (v.IsExponentiation()
                 || v.IsProduct()
                 || v.IsSum()
                 )
            return Become(v**this);
        return Become(Product{*this, v});
    }
    
    bool Variable::MultiplyIfSimplifiable(const Valuable& v)
    {
        auto is = v.IsVa();
        if (is) {
            is = operator==(v);
            if (is) {
                sq();
            }
        } else if (v.IsSimple()) {
        } else {
            auto s = v.IsMultiplicationSimplifiable(*this);
            is = s.first;
            if (is) {
                Become(std::move(s.second));
            }
        }
        return is;
    }

    std::pair<bool,Valuable> Variable::IsMultiplicationSimplifiable(const Valuable& v) const
    {
        std::pair<bool,Valuable> is;
        auto vIsVa = v.IsVa();
        is.first = vIsVa && operator==(v);
        if (is.first) {
            is.second = Sq();
        } else if (v.IsSimple()
			|| vIsVa
			) {
        } else {
            is = v.IsMultiplicationSimplifiable(*this);
        }
        return is;
    }

    bool Variable::SumIfSimplifiable(const Valuable& v)
    {
        std::pair<bool,Valuable> is;
        is.first = v.IsVa() && operator==(v);
        if (is.first) {
            Become(*this*2);
        } else {
            is = v.IsSummationSimplifiable(*this);
            if(is.first)
                Become(std::move(is.second));
        }
        return is.first;
    }

    std::pair<bool,Valuable> Variable::IsSummationSimplifiable(const Valuable& v) const
    {
        std::pair<bool,Valuable> is;
        is.first = v.IsVa();
        if (is.first) {
            is.first = operator==(v);
            if (is.first) {
                is.second = v * 2;
            }
        } else if (v.IsSimple()) {
        } else {
            is = v.IsSummationSimplifiable(*this);
            if(is.first)
            {
                if (is.second.Complexity() > v.Complexity())
                    IMPLEMENT;
            }
        }
        return is;
    }

    Valuable& Variable::operator /=(const Valuable& v)
    {
        if (operator==(v))
        {
            Become(Integer(1));
        }
        else
        {
            *this *= 1 / v;
        }

        return *this;
    }

    Valuable& Variable::operator %=(const Valuable& v)
    {
        return Become(Modulo(*this, v));
    }

    Valuable& Variable::operator^=(const Valuable& v)
    {
        auto ii = v.IsInt();
        if (ii && v.IsZero()) {
            Become(1_v);
        } else if (ii && v == constants::one) {
        } else
            Become(Exponentiation(*this, v));
        return *this;
    }
    
    Valuable& Variable::d(const Variable& x)
    {
        return Become(1_v);
    }
    
    bool Variable::operator <(const Valuable& v) const
    {
        if (v.IsVa())
        {
            auto& i = v.as<Variable>();
            if (varSetHost != i.varSetHost) {
                throw "Unable to compare variable sequence numbers from different var hosts. Do you need a lambda for delayed comparision during evaluation? implement then.";
            }
            return varSetHost->CompareIdsLess(varId, i.varId);
        }
        else if (v.IsInt() || v.IsFraction() || v.IsConstant())
        {
            // For numeric types, compare using evaluation
            auto thisVal = varSetHost->Host(varId);
            return thisVal < v;
        }
        
        // For other types, use base class comparison
        return base::operator <(v);
    }

    bool Variable::operator==(const Valuable& value) const
    {
        if (value.IsVa()) {
            return operator==(value.as<Variable>());
        }
        else if (value.IsInt() || value.IsFraction() || value.IsConstant()) {
            // For numeric types, compare using evaluation
            auto thisVal = varSetHost->Host(varId);
            return thisVal == value;
        }
        return Hash() == value.Hash() && value.operator==(*this);
    }

    bool Variable::operator==(const Variable& v) const
    {
        if (varSetHost != v.varSetHost) {
            throw "Unable to compare variable sequence numbers from different var hosts. Do you need a lambda for delayed comparision during evaluation? implement then.";
        }
        return hash == v.Hash()
            && varSetHost->CompareIdsEqual(varId, v.varId);
    }
    
    std::ostream& Variable::print(std::ostream& out) const
    {
        return varSetHost->print(out, varId);
    }

    Valuable::universal_lambda_t Variable::CompileIntoLambda(variables_for_lambda_t variables) const
    {
        auto beg = std::begin(variables);
        auto end = std::end(variables);
        auto it = std::find(beg, end, *this);
        if(it == end) {
            return [ref=std::cref(varSetHost->Host(varId))](universal_lambda_params_t) {
                return ref.get();
            };
        } else {
            auto idx = it - beg;
            return [idx](universal_lambda_params_t params) {
                return *(std::begin(params) + idx);
            };
        }
    }

    bool Variable::IsComesBefore(const Valuable& value) const
    {
        if (operator==(value)) {
            return {};
        } else if (value.IsVa())
            return value.as<Variable>() < *this;
        else {
            return !value.IsComesBefore(*this);
        }
    }
    
    void Variable::CollectVa(std::set<Variable>& s) const
    {
        s.emplace(*this);
    }

    void Variable::CollectVaNames(Valuable::va_names_t&  s) const{
        s.emplace(varSetHost->GetName(varId), *this);
    }

    bool Variable::eval(const std::map<Variable, Valuable>& with) {
        auto it = with.find(*this);
        auto evaluated = it != with.end();
        if(evaluated) {
            auto c = it->second;
            Become(std::move(c));
        }
        return evaluated;
    }
    
    void Variable::Eval(const Variable& va, const Valuable& v)
    {
        if(va==*this)
        {
            auto copy = v;
            Become(std::move(copy));
        }
    }
    
    void Variable::solve(const Variable& va, solutions_t& solutions) const
	{
		if (operator==(va))
			solutions.emplace(constants::zero);
    }
    
    const Valuable::vars_cont_t& Variable::getCommonVars() const
    {
        vars[*this] = 1_v;
        if(vars.size()>1)
        {
            vars.clear();
            getCommonVars();
        }
        return vars;
    }
    
    Valuable Variable::InCommonWith(const Valuable& v) const
    {
        auto c = 1_v;
        if(v.IsSimple() || v.IsConstant())
            ;
        else if (v.IsProduct()
                 || v.IsFraction()
                 ) {
            c = v.InCommonWith(*this);
        } else if (v.IsExponentiation()) {
            auto& e = v.as<Exponentiation>();
            if (e.getBase() == *this) {
                if (e.getExponentiation().IsInt()) {
                    if (e.getExponentiation() > 0) {
                        c = *this;
                    }
                } else if (e.IsMultival()==YesNoMaybe::Yes) {
                }
                else {
                    IMPLEMENT
                }
            }
        } else if (v.IsVa()) {
            if (*this == v)
                c = v;
        } else {
            c = v.InCommonWith(*this);
        }
        return c;
    }
    
    Valuable Variable::operator()(const Variable& va, const Valuable& augmentation) const
    {
        if (*this == va) {
            return {augmentation};
        }
        else
            IMPLEMENT
    }

    Valuable::solutions_t Variable::Distinct() const {
    	return {*this}; }

    Valuable Variable::Abet(const std::initializer_list<Valuable> l) const {
        Product a;
        for (auto& item : l)
            a.Add(Equals(item));
        return Valuable(a.Move());
    }

    Valuable Variable::Intersect(const Valuable& other, const Variable& var) const {
        if (operator==(var)) {
            auto solutions1 = IntSolutions(var);
            auto solutions2 = other.IntSolutions(var);
            solutions_t intersection;
            for (const auto& sol : solutions1) {
                if (solutions2.find(sol) != solutions2.end()) {
                    intersection.insert(sol);
                }
            }
            return Valuable(std::move(intersection));
        }
        return base::Intersect(other, var);
    }

    bool Variable::IsPolynomial(const Variable& v) const {
        return true;
    }

    Valuable::vars_cont_t Variable::GetVaExps() const {
        return {{*this, constants::one}};
    }
}}
