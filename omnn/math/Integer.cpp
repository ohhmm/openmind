//
// Created by Сергей Кривонос on 01.09.17.
//

#include "Integer.h"
#include "Exponentiation.h"
#include "Fraction.h"
#include "Sum.h"
#include "Product.h"
#include <algorithm>
#include <cmath>
#include <boost/functional/hash.hpp>
#include <boost/numeric/conversion/converter.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>


namespace omnn{
namespace math {

    Integer::Integer(const Fraction& f)
    : arbitrary(Integer::cast(f.getNumerator())->arbitrary / Integer::cast(f.getDenominator())->arbitrary)
    {
        
    }
    
    Integer::operator int64_t() const {
        return boost::numeric_cast<int64_t>(arbitrary);
    }
    
    Integer::operator size_t() const {
        return boost::numeric_cast<size_t>(arbitrary);
    }
    
    Valuable Integer::operator -() const
    {
        return Integer(-arbitrary);
    }
    
    Valuable& Integer::operator +=(const Valuable& v)
    {
        auto i = cast(v);
        if (i)
        {
            arbitrary += i->arbitrary;
            hash = std::hash<base_int>()(arbitrary);
        }
        else
        {
            Become(v + *this);
        }
        return *this;
    }

    Valuable& Integer::operator +=(int v)
    {
        arbitrary += v;
        hash = std::hash<base_int>()(arbitrary);
        return *this;
    }

    Valuable& Integer::operator *=(const Valuable& v)
    {
        auto i = cast(v);
        if (i)
        {
            arbitrary *= i->arbitrary;
            hash = std::hash<base_int>()(arbitrary);
        }
        else
        {
            // all other types should handle multiplication by Integer
            Become(v**this);
        }
        return *this;
    }

    Valuable& Integer::operator /=(const Valuable& v)
    {
        auto i = cast(v);
        if (i)
        {
            auto div = arbitrary/i->arbitrary;
            if (div*i->arbitrary==arbitrary)
            {
                arbitrary = div;
                hash = std::hash<base_int>()(arbitrary);
            }
            else
                Become(Fraction(*this,*i));
        }
        else
        {
			Become(Fraction(*this, v));
        }
        return *this;
    }

    Valuable& Integer::operator %=(const Valuable& v)
    {
        auto i = cast(v);
        if (i)
        {
            arbitrary %= i->arbitrary;
            hash = std::hash<base_int>()(arbitrary);
        }
        else
        {
            // try other type
            // no type matched
            base::operator %=(v);
        }
        return *this;
    }

    Valuable& Integer::operator --()
    {
        arbitrary--;
        hash = std::hash<base_int>()(arbitrary);
        return *this;
    }

    Valuable& Integer::operator ++()
    {
        arbitrary++;
        hash = std::hash<base_int>()(arbitrary);
        return *this;
    }

    Integer::operator int() const
    {
        return boost::numeric_cast<int>(arbitrary);
    }

    Integer::operator double() const
    {
        return boost::numeric_cast<double>(arbitrary);
    }

    Integer::operator long double() const
    {
        return boost::numeric_cast<long double>(arbitrary);
    }
    
    Integer::operator unsigned char() const
    {
        return boost::numeric_cast<unsigned char>(arbitrary);
    }

    Valuable& Integer::operator^=(const Valuable& v)
    {
        if(arbitrary == 0 || arbitrary == 1)
        {
            if (v == 0) {
                throw "Implement!"; //NaN
            }
            else {
                return *this;
            }
        }
        auto i = cast(v);
        if(i)
        {
            if (*i != 0_v) {
                if (*i > 1) {
                    Valuable x = *this;
                    Valuable n = v;
                    if (n < 0_v)
                    {
                        x = 1_v / x;
                        n = -n;
                    }
                    if (n == 0_v)
                    {
                        arbitrary = 1;
                        return *this;
                    }
                    auto y = 1_v;
                    while(n > 1)
                    {
                        auto in = Integer::cast(n);
                        if (!in) throw "Implement!";
                        if (in && (in->arbitrary % 2) == 0)
                        {
                            x *= x;
                            n /= 2;
                        }
                        else
                        {
                            y *= x;
                            x *= x;
                            --n;
                            n /= 2;
                        }
                    }
                    x *= y;
                    i = Integer::cast(x);
                    if (i) {
                        arbitrary = std::move(i->arbitrary);
                        hash = std::hash<base_int>()(arbitrary);
                    }
                    else
                        return Become(std::move(x));
                } else {
                    // negative
                    Become(Exponentiation(*this, v));
                }
            }
            else { // zero
                if (arbitrary == 0)
                    throw "NaN"; // feel free to handle this properly
                else
                {
                    arbitrary = 1;
                    hash = std::hash<base_int>()(arbitrary);
                }
            }
        }
        else
        {
            auto f = Fraction::cast(v);
            if (f->IsSimple()) {
                auto n = f->getNumerator();
                auto dn = f->getDenominator();

                if (n != 1)
                    *this ^= n;
                Valuable nroot;
                bool rootFound = false;
                Valuable left =0, right = *this;

                while (!rootFound)
                {
                    auto d = right - left;
                    d -= d % 2;
                    if (d!=0) {
                        nroot = left + d / 2;
                        auto result = nroot ^ dn;
                        if (result == *this)
                        {
                            rootFound = true;
                            return Become(std::move(nroot));
                        }
                        else
                        {
                            if (result > *this)
                            {
                                right = nroot;
                            }
                            else
                            {
                                left = nroot;
                            }
                        }
                    }
                    else
                        throw "Implement!";
                }
            }
            else
                Become(Exponentiation(*this, v));
        }
        
        optimize();
        return *this;
    }
    
    bool Integer::operator <(const Valuable& v) const
    {
        auto i = cast(v);
        if (i)
            return arbitrary < i->arbitrary;
        else
        {
            // try other type
            auto i = Fraction::cast(v);
            if (i)
            {
                return !(v < *this) && *this!=v;
            }
            else
            {
                // try other type
                // no type matched
            }
        }
        // not implemented comparison to this Valuable descent
        return base::operator <(v);
    }

    bool Integer::operator ==(const Valuable& v) const
    {
        auto i = cast(v);
        if (i)
            return Hash() == i->Hash() && arbitrary == i->arbitrary;
        else if(v.FindVa())
            return false;
        else
            return v == *this;
    }

    std::ostream& Integer::print(std::ostream& out) const
    {
        return out << arbitrary;
    }
    
    Valuable Integer::calcFreeMember() const
    {
        return *this;
    }

    bool Integer::Factorization(const std::function<bool(const Integer&)>& f) const
    {
        auto h = arbitrary;
        if(h < 0)
            h = -h;
        for (base_int i = 1; i <= h; ++i)
            if (cast(*this/Integer(i)) && f(i))
                return true;
        return false;
    }
}}

