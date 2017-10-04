//
// Created by Сергей Кривонос on 25.09.17.
//

#include "Product.h"
#include "Sum.h"
#include "Fraction.h"

namespace omnn{
namespace extrapolator {

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
		if (vars.size() == 1)
		{
			Become(std::move(*const_cast<Valuable*>(&*vars.begin())));
		}
		else
		{
			for (auto it = vars.begin(); it != vars.end(); ++it)
			{
				if (*it == 1)
					vars.erase(it++);
				auto t = it;
				for (auto it2 = ++t; it2 != vars.end();)
				{
					if (it2->OfSameType(*it))
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
		return Become(Sum(this, v));
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
		// TODO : compare sets
		return base::operator ==(v);
	}

	std::ostream& Product::print(std::ostream& out) const
	{
		/*for (auto& b : vars)
			out << b << "*";*/
		for (std::set<Valuable>::iterator it = vars.begin(); it != vars.end(); ++it)
			out << '*' << *it;
		return out;
	}

	



}}