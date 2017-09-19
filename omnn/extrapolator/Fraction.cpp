//
// Created by Сергей Кривонос on 01.09.17.
//

#include "Fraction.h"
#include <boost/rational.hpp>

namespace std{
    template<>
    struct is_signed<boost::multiprecision::cpp_int> {
        bool value = true;
    };
}
namespace omnn{
namespace extrapolator {

    Valuable Fraction::operator -() const
    {
        return Fraction(-numerator, denominator);
    }

    void Fraction::optimize()
    {
        auto d = boost::gcd(numerator, denominator);
        numerator /= d;
        denominator /= d;
    }
    
    Valuable& Fraction::operator +=(const Valuable& v)
    {
        auto i = cast(v);
        if (i){
			auto f = *i;
            if(denominator != f.denominator) {
                f.numerator *= denominator;
                f.denominator *= denominator;
                numerator *= i->denominator;
                denominator *= i->denominator;
            }

            numerator += f.numerator;
            optimize();
        }
        else
        {
            // try other type
            // no type matched
            base::operator +=(v);
        }
        return *this;
    }

    Valuable& Fraction::operator +=(int v)
    {
		numerator += v*denominator;
		optimize();
		return *this;
    }

    Valuable& Fraction::operator *=(const Valuable& v)
    {
        auto i = cast(v);
        if (i) 
		{
			numerator *= i->numerator;
			denominator *= i->denominator;
		}
            
        else
        {
            // try other type
            // no type matched
            base::operator *=(v);
        }
		
		optimize();
        return *this;
    }

    Valuable& Fraction::operator /=(const Valuable& v)
    {
        auto i = cast(v);
        if (i)
		{
			numerator *= i->denominator;
			denominator *= i->numerator;
		}
            //arbitrary += i->arbitrary;
        else
        {
            // try other type
            // no type matched
            base::operator /=(v);
        }
		optimize();
		return *this;
    }

    Valuable& Fraction::operator %=(const Valuable& v)
    {
		return base::operator %=(v);
    }

    Valuable& Fraction::operator --()
    {
        //arbitrary--;
        return *this+=-1;
    }

    Valuable& Fraction::operator ++()
    {
        //arbitrary++;
        return *this+=1;
    }

    bool Fraction::operator <(const Valuable& v) const
    {
        auto i = cast(v);
		if (i)
		{
			auto f = *i;
			auto l = *this;
			if (l.denominator != f.denominator) {
				f.numerator *= l.denominator;
				f.denominator *= l.denominator;
				l.numerator *= i->denominator;
				l.denominator *= i->denominator;
			}
			return l.numerator < f.numerator;//arbitrary < i->arbitrary;
		}
            
        else
        {
            // try other type
        }

        // not implemented comparison to this Valuable descent
        return base::operator <(v);
    }

    bool Fraction::operator ==(const Valuable& v) const
    {
        auto i = cast(v);
		if (i)
		{
			auto f = *i;
			auto l = *this;
			if (l.denominator != f.denominator) {
				f.numerator *= l.denominator;
				f.denominator *= l.denominator;
				l.numerator *= i->denominator;
				l.denominator *= i->denominator;
			}
			return l.numerator == f.numerator;//arbitrary < i->arbitrary;
		}
        else
        {
            // try other type
        }
        // no type matched
        return base::operator ==(v);
    }

    std::ostream& Fraction::print(std::ostream& out) const
    {
        return out << numerator << '/' << denominator;
    }

}}
