//
// Created by Сергей Кривонос on 25.09.17.
//

#include "Sum.h"

namespace omnn{
namespace extrapolator {
	Valuable Sum::operator -() const
	{
		Sum s;
		for (auto& a : members) 
			s.members.push_back( - a);
		return s;
	}

	/*void Sum::optimize()
	{
		
	}*/

	Valuable& Sum::operator +=(const Valuable& v)
	{
		auto i = cast(v);
		if (i) {
			for (auto& a : i->members) {
				this->members.push_back(a);
			}

		}
		else
		{
			// try other type
			// no type matched
			base::operator +=(v);
		}
		return *this;
	}

	Valuable& Sum::operator +=(int v)
	{
		auto f = cast(v);
		if (f)
		{
			members.push_back(v);
		}
		else
		{
			auto i = Integer::cast(v);
			if (i)
			{
				members.push_back(*i);
			}
			else
			{
				// try other type
				// no type matched
				base::operator +=(v);
			}
		}
		optimize();
		return *this;
	}

	Valuable& Sum::operator *=(const Valuable& v)
	{
		auto f = cast(v);
		if (f)
		{
			Sum s;
			for (auto& a : members) {
				for (auto& b : f->members) {
					s.members.push_back(a*b);
				}
			}
			*this = s;
		}
		else {
			auto i = Integer::cast(v);
			if (i)
			{
				Sum s;
				for (auto& a : members) {
						s.members.push_back(a*(*i));
					
				}
				*this = s;
			}
			else
			{
				// try other type
				// no type matched
				base::operator *=(v);
			}
		}

		optimize();
		return *this;
	}

	Valuable& Sum::operator /=(const Valuable& v)
	{
		auto i = cast(v);
		if (i)
		{
			Sum s;
			for (auto& a : members) {
				for (auto& b : i->members) {
					s.members.push_back(a/b);
				}
			}
			*this = s;
		}
		else
		{
			auto i = Integer::cast(v);
			if (i)
			{
				Sum s;
				for (auto& a : members) {
					s.members.push_back(a/(*i));

				}
				*this = s;
			}
			else
			{
				// try other type
				// no type matched
				base::operator /=(v);
			}

		}
		optimize();
		return *this;
	}

	Valuable& Sum::operator %=(const Valuable& v)
	{
		return base::operator %=(v);
	}

	Valuable& Sum::operator --()
	{
		return *this += -1;
	}

	Valuable& Sum::operator ++()
	{
		return *this += 1;
	}

	bool Sum::operator <(const Valuable& v) const
	{

		// not implemented comparison to this Valuable descent
		return base::operator <(v);
	}

	bool Sum::operator ==(const Valuable& v) const
	{
		
		return base::operator ==(v);
	}

	std::ostream& Sum::print(std::ostream& out) const
	{
		for (auto& b : members) 
			out << b << "+";
			return out;// << "" << '/' << "";
	}



}}