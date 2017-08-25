#pragma once

#include <map>
#include <set>
#include <string>
#include <type_traits>
#include <boost/operators.hpp>
//#include "VarHost.h"

// todo: template<from variable_id type>
class Expression
        : boost::operators<Expression>
{
public:
    using variable_id = int;
    using default_num_type = int;
    using value = double;

    using power_of_var = default_num_type;
    using num = default_num_type;
    using polynom = std::map<power_of_var, num>;
    using value_set = std::map<variable_id, value>;
    using solution = std::map<variable_id, value>;
    using express_result = std::function<value(const value_set &)>; // f(x1,x2,...)

    Expression() = default;

    template<class T>
    Expression(const T& iterable) {
        int i=0;
        for(const auto& var : iterable) {
            polynom p;
            p[1] = var;
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

    Expression& operator += (const Expression& e)
    {
        // varhost  `
        for(auto& varPolynom : e.polynoms)
        {
            auto& target = polynoms[varPolynom.first];
            EmergePolynom(target, varPolynom.second);
        }
        return *this;
    }

    Expression& operator *= (const Expression& e)
    {
        for(auto& varPolynom : e.polynoms)
        {
            auto& target = polynoms[varPolynom.first];
            EmergePolynom(target, varPolynom.second);
        }
        return *this;
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

private:
    std::map<variable_id, polynom> polynoms;
};
