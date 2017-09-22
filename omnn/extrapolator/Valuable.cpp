//
// Created by Сергей Кривонос on 01.09.17.
//

#include "Valuable.h"
#include "Integer.h"

namespace omnn{
namespace extrapolator {

    static void implement()
    {
        throw "Implement!";
    }
    
    #define IMPLEMENT { implement(); throw; }

    Valuable::Valuable(int i) : exp(new Integer(i)) {}

    Valuable::~Valuable()
    {
    }

    Valuable Valuable::operator -() const
    {
        if(exp)
            return exp->operator-();
        else
            IMPLEMENT
    }

    Valuable& Valuable::operator +=(const Valuable& v)
    {
        if(exp)
            return exp->operator+=(v);
        else
            IMPLEMENT
    }
    
    Valuable& Valuable::operator +=(int v)
    {
        if(exp)
            return exp->operator+=(v);
        else
            IMPLEMENT
        }

    Valuable& Valuable::operator *=(const Valuable& v)
    {
        if(exp)
            return exp->operator*=(v);
        else
            IMPLEMENT

    }

    Valuable& Valuable::operator /=(const Valuable& v)
    {
        if(exp)
            return exp->operator/=(v);
        else
            IMPLEMENT

    }

    Valuable& Valuable::operator %=(const Valuable& v)
    {
        if(exp)
            return exp->operator%=(v);
        else
            IMPLEMENT
    }

    Valuable& Valuable::operator--()
    {
        if(exp)
            return exp->operator--();
        else
            IMPLEMENT
    }
    
    Valuable& Valuable::operator++()
    {
        if(exp)
            return exp->operator++();
        else
            IMPLEMENT
    }
    
    bool Valuable::operator<(const Valuable& v) const
    {
        if(exp)
            return exp->operator<(v);
        else
            IMPLEMENT

    }

    bool Valuable::operator==(const Valuable& v) const
    {
        if(exp)
            return exp->operator==(v);
        else
            IMPLEMENT

    }
    
    std::ostream& Valuable::print(std::ostream& out) const
    {
        if(exp)
            return exp->print(out);
        else
            IMPLEMENT
    }
    
    std::ostream& operator<<(std::ostream& out, const Valuable& obj)
    {
        return obj.print(out);
    }

    void Valuable::abs() {
        if(exp)
            return exp->abs();
        else
            IMPLEMENT
    }

    void Valuable::optimize() {
        if(exp)
            return exp->optimize();
        else
            IMPLEMENT
    }

    void Valuable::sqrt() {
        if(exp)
            return exp->sqrt();
        else
            IMPLEMENT
    }

    Valuable ValuableDescendantContract::sqrt() const { IMPLEMENT }
}}
