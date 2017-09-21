#pragma once

#include <functional>
#include <map>
#include <set>
#include <string>
#include <type_traits>
#include <boost/operators.hpp>
#include "Fraction.h"
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
    Expression(const Valuable& v)
    : Expression(*cast(v))
    {
        auto e = cast(v);
        if(!e) throw;
    }
    template<class T>
    Expression(const std::initializer_list<T>& iterable) {
        int i=0;
        for(const auto& var : iterable) {
            polynom p;
            p[1] = num(var);
            polynoms[i++] = p;
        }
    }
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

    Expression Sqr() const
    {
        Expression e = *this;
        for(auto& varPolynom : e.polynoms)
        {
            for(auto& member : varPolynom.second)
            {
                power_of_var* power = const_cast<power_of_var*>(&member.first);
                *power*=2;
            }
        }
        return e;
    }

    void Fit(const Expression& e)
    {
        *this = Sqr() + e.Sqr();
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
    void optimize() override;
    std::ostream& print(std::ostream& out) const override;
private:
    std::map<variable_id, polynom> polynoms;
};

}}
