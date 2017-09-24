#pragma once

#include <functional>
#include <map>
#include <set>
#include <string>
#include <type_traits>
#include <boost/operators.hpp>
#include "Fraction.h"
#include "SymmetricDouble.h"
#include <iterator>
//#include "VarHost.h"

namespace omnn{
namespace extrapolator {

// todo: template<from variable_id type>
class Expression
        : public ValuableDescendantContract<Expression>
{
    using base = ValuableDescendantContract<Expression>;
public:
    using variable_id = int;
    using default_num_type = Fraction;
    using value = Expression;

    using power_of_var = default_num_type;
    using num = default_num_type;
    using polynom = std::map<power_of_var, num>;
    using value_set = std::map<variable_id, value>;
    using solution = std::map<variable_id, value>;
    using express_result = std::function<value(const value_set &)>; // f(x1,x2,...)

    Expression() = default;
    Expression(const Expression&) = default;
    Expression& operator=(const Expression&) = default;
	Expression(int i)
	{
		polynoms[0][0] = i;
	}
	Expression(const Valuable& v)
    : Expression(*cast(v))
    {
        auto e = cast(v);
        if(!e) throw;
    }
    template<class T>
    Expression(const T& iterable) {
        int i=0;
        for(const auto& var_coef : iterable) {
            polynom p;
            p[1] = var_coef;
            polynoms[i++] = p;
        }
    }

    Expression(std::initializer_list<default_num_type>&& l):Expression(l){}
//    solution Solve() {
//
//    }

//    Expression Express(const variable_id& var) const
//    {
//        auto v = polynoms[var];
//        if(v.size() != 1)
//            throw "not yet supported, redesign math to make possible to get roots";
//        express_result resulting_function = [&]{
//  sum of -members of other var polynoms
//}
//        auto p = polynoms[var];
//
//    }
//
//    express_result Eval(const variable_id& var) const
//    {
//        auto v = polynoms[var];
//        if(v.size() )
//            express_result resulting_function =
//            auto p = polynoms[var];
//
//    }

    static void EmergePolynom(polynom& p, const polynom& pp)
    {
        for(auto& member : pp)
        {
            auto& target = p[member.first];
            target += member.second;
        }
    }

//    Expression& operator ^ (power_of_var pow)
//    {
//        for(auto& varPolynom : polynoms)
//        {
//            for(auto& member : varPolynom.second)
//            {
//                member.first*=pow;
//            }
//        }
//        return *this;
//    }

    void Fit(const Expression& e)
    {
        (*this *= *this) += e*e;
    }



    // virtual operators
	Valuable operator -() const override;
    Valuable& operator +=(const Valuable& v) override;
    Valuable& operator +=(int v) override;
    Valuable& operator *=(const Valuable& v) override;
    Valuable& operator /=(const Valuable& v) override;
    Valuable& operator %=(const Valuable& v) override;
    Valuable& operator --() override;
    Valuable& operator ++() override;
    bool operator <(const Valuable& v) const override;
    bool operator ==(const Valuable& v) const override;
// todo :
	//Valuable abs() const override { return Valuable::abs(); }
    void optimize() override;
    std::ostream& print(std::ostream& out) const override;
private:
    std::map<variable_id, polynom> polynoms;
};

}}
