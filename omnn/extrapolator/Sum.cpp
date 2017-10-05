//
// Created by Сергей Кривонос on 25.09.17.
//

#include "Sum.h"
#include "Product.h"
#include "Variable.h"

namespace omnn{
namespace extrapolator {
	Valuable Sum::operator -() const
	{
		Sum s;
		for (auto& a : members) 
			s.members.push_back(-a);
		return s;
	}

	void Sum::optimize()
	{
        if (members.size()==1) {
            Become(std::move(members.front()));
        }
        else
        {
            // todo : emerge same typed members
        }
	}

	Valuable& Sum::operator +=(const Valuable& v)
	{
		auto i = cast(v);
		if (i) {
			for (auto& a : i->members) {
				*this += a;
			}
		}
		else
		{
            for (const Valuable& a : members) {
                if(a.OfSameType(v))
                {
                    const_cast<Valuable&>(a) += v;
                    goto yes;
                }
            }
            // add new member
			members.push_back(v);
		}
    yes:
        optimize();
		return *this;
	}

	Valuable& Sum::operator +=(int v)
	{
		return *this+=(Integer(v));
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
            s.optimize();
			members = s.members;
		}
		else
        {
            for (auto& a : members) {
                a*=v;
            }
            optimize();
		}

		return *this;
	}

	Valuable& Sum::operator /=(const Valuable& v)
	{
        // todo: store valuables in Fraction
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
        auto s = cast(v);
        return s && members==s->members;
	}

	std::ostream& Sum::print(std::ostream& out) const
	{
		for (auto& b : members) 
			out << b << "+";
		return out;// << "" << '/' << "";
	}

    /** fast linear equation formula deduction */
	Formula Sum::FormulaOfVa(const Variable& v) const
	{
		// TODO : make it workfor non-linear as well once Formula ready
        Valuable e(0);
        const Variable* cv = nullptr; // the var found
        const Product* svp = nullptr; // product with var
        
		for (auto& m : members)
		{
           
			auto p = Product::cast(m);
			if(p)
			{
				for(auto pm : *p)
				{
					auto vp = Variable::cast(pm);
					if(*vp==v)
					{
                        if(cv || svp)
                            throw "More then one variable occurence need Implement!";
						cv=vp;
                        svp = p;
                        break;;
					}
				}
			}
			else
			{
				auto vp = Variable::cast(m);
				if(vp && *vp==v)
				{
					cv=vp;
                    // TODO: e = *this - v;
				}
				else
				{
                    // TODO : other types?
                    
					e -= m;
				}
			}
		}
        
        if(cv)
        {
            if(svp)
            {
                Product o;
                for(auto a : *svp){
                    if(a!=v)
                        o*=a;
                }
                e /= o;
            }
            return Formula::DeclareFormula(v, e);
        }
        
        throw "No such variable";
	}

}}
