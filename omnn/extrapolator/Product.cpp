//
// Created by Сергей Кривонос on 25.09.17.
//
#include "Product.h"
#include "Sum.h"
#include "Fraction.h"
#include <type_traits>

namespace omnn{
namespace extrapolator {

    // store order operator
    bool ValuableLessCompare::operator()(const Valuable& v1, const Valuable& v2)
    {
        auto v1va = Variable::cast(v1);
        auto v2va = Variable::cast(v2);
        auto v1fa = v1.FindVa();
        auto v2fa = v2.FindVa();
        

        if(v1va)
        {
            if(v2va)
            {
                return v1 < v2;
            }
            else
            {
                if (v2fa)
                {
                    return true;
                }
                else
                {
                    auto v2s = Sum::cast(v2);
                    if (v2s) {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }
        else
        {
            if(v2va)
            {
                if (v1fa)
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
            else
            {
                if(v1fa)
                {
                    return false;
                }
                else
                {
                    auto v1s = Sum::cast(v1);
                    if (v1s)
                    {
                        return false;
                    }
                    else
                    {
                        if (Sum::cast(v2) || Product::cast(v2)) {
                            return true;
                        }
                        else
                            return v1 < v2;
                    }
                }
            }
        }
    }
    
	Valuable Product::operator -() const
	{
		Product p;
		for (auto& a : vars)
			if (a == *vars.begin())
				p.vars.insert(-a);
			else
				p.vars.insert(a);
		return p;
	}

	void Product::optimize()
	{
        for(auto& it : vars)
            const_cast<Valuable&>(it).optimize();
		if (vars.size() == 1)
		{
			Become(std::move(*const_cast<Valuable*>(&*vars.begin())));
		}
		else
		{
			for (auto it = vars.begin(); it != vars.end(); ++it)
			{
                if (*it == 0)
                {
                    Become(0);
                    return;
                }
				if (*it == 1)
					vars.erase(it++);
				auto t = it;
				for (auto it2 = ++t; it2 != vars.end();)
				{
                    if (it2->OfSameType(*it) && !Variable::cast(*it))
					{
						const_cast<Valuable&>(*it) *= *it2;
						vars.erase(it2++);	
					}
					else
						++it2;
				}
			}
		}
	}

	Valuable& Product::operator +=(const Valuable& v)
	{
		return Become(Sum(*this, v));
	}

	Valuable& Product::operator +=(int v)
	{
		return *this += Valuable(v);
	}

	Valuable& Product::operator *=(const Valuable& v)
	{
		for (const Valuable& a : vars) {
				if (a.OfSameType(v))
				{
					const_cast<Valuable&>(a) *= v;
					goto yes;
				}
			}
			// add new member
			vars.insert(v);

	yes:
		optimize();
		return *this;
	}

	Valuable& Product::operator /=(const Valuable& v)
	{
		Fraction f(1, v);
		for (const Valuable& a : vars) {
			if (a.OfSameType(f))
			{
				const_cast<Valuable&>(a) *= f;
				goto yes;
			}
		}
		// add new member
		vars.insert(f);

	yes:
		optimize();
		return *this;
	}

	Valuable& Product::operator %=(const Valuable& v)
	{
		return base::operator %=(v);
	}

	Valuable& Product::operator --()
	{
		return Become(Sum(this, -1));
	}

	Valuable& Product::operator ++()
	{
		return Become(Sum(this, 1));
	}

	bool Product::operator <(const Valuable& v) const
	{
		// not implemented comparison to this Valuable descent
		return base::operator <(v);
	}

	bool Product::operator ==(const Valuable& v) const
	{
        auto p = cast(v);
        return p && vars==p->vars;
	}

	std::ostream& Product::print(std::ostream& out) const
	{
        std::stringstream s;
        constexpr char sep[] = "*";
        for (auto& b : vars)
            s << b << sep;
        auto str = s.str();
        auto cstr = const_cast<char*>(str.c_str());
        cstr[str.size() - sizeof(sep) + 1] = 0;
        out << cstr;
        return out;
	}

	



}}
