//
// Created by Сергей Кривонос on 20.09.17.
//
#include "Expression.h"

namespace omnn{
namespace extrapolator {


Valuable Expression::operator -() const
{
    Expression e;
    //all poly vars sign changes
    return e;
}

Valuable& Expression::operator +=(const Valuable& v)
{
    auto e = cast(v);
    if (e){
        // TODO: varhost  `
        for(auto& varPolynom : e->polynoms)
        {
            auto& target = polynoms[varPolynom.first];
            EmergePolynom(target, varPolynom.second);
        }
    }
    else
    {
        // try other type
        auto f = Fraction::cast(v);
        Fraction integer;
        if(!f)
        {
            
            auto i = Integer::cast(v);
            if(i){
                integer = Fraction(*i);
                f = &integer;
            }
        }
        if(f)
        {
            // add to 0 exponentation var



        }

        // no type matched
        base::operator +=(v);
    }
    return *this;
}

Valuable& Expression::operator +=(int v)
{
    // zero exponentation coeficient


    optimize();
    return *this;
}

Valuable& Expression::operator *=(const Valuable& val)
{
    auto e = cast(val);
    if (e)
    {
        Expression ex;
        for(auto& a : polynoms) {
            for(auto& b : e->polynoms) {
                //TODO : out+=a*b;
            }
        }
        *this = ex;
    }
    else
    {
        // try other type
        // no type matched
        base::operator *=(val);
    }

    optimize();
    return *this;
}

Valuable& Expression::operator /=(const Valuable& v)
{
    auto i = cast(v);
    if (i)
    {

    }
    else
    {
        // try other type
        // no type matched
        base::operator /=(v);
    }
    optimize();
    return *this;
}

Valuable& Expression::operator %=(const Valuable& v)
{
    return base::operator %=(v);
}

Valuable& Expression::operator --()
{
    // todo step by differential hosted at varhost
    return *this;
}

Valuable& Expression::operator ++()
{
    return *this;
}

bool Expression::operator <(const Valuable& v) const
{

    auto i = cast(v);
    if (i)
    {
        // TBD
        // compare by estimated calculation time
        // if only zero exponentation coeficient here, then we could to compare as scalars
        // but comparision of expressions may be needed for its performance optimizations
        // or we could compare with all vars set to one, we need to know how it will be used first
    }

    else
    {
        // try other type
    }

    // not implemented comparison to this Valuable descent
    return base::operator <(v);
}

bool Expression::operator ==(const Valuable& v) const
{
    auto i = cast(v);
    if (i)
    {
        // ToBeDiscussed
    }
    else
    {
        // try other type
    }
    // no type matched
    return base::operator ==(v);
}

void Expression::optimize()
{
    
}

std::ostream& Expression::print(std::ostream& out) const
{
    // string representation
    return out << " ";
}



}}
