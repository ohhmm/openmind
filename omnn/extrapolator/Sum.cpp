//
// Created by Сергей Кривонос on 25.09.17.
//
#include "Sum.h"

#include "Formula.h"
#include "Fraction.h"
#include "Integer.h"
#include "Product.h"
#include "Variable.h"

#include <cmath>
#include <map>

namespace omnn{
namespace extrapolator {

    namespace
    {
        ProductOrderComparator toc; // todo : rename to type order comparator and move to file
    }
    // store order operator
    bool SumOrderComparator::operator()(const Valuable& v1, const Valuable& v2) const
    {
        return v1.OfSameType(v2) ? v1.IsComesBefore(v2) : toc(v1,v2);
    }
    
    void Sum::Add(const Valuable& item)
    {
        auto it = std::find(begin(), end(), item);
        if(it==end())
           base::Add(item);
        else
           Update(it, item*2);
        auto itemMaxVaExp = item.getMaxVaExp();
        if(itemMaxVaExp > maxVaExp)
            maxVaExp = itemMaxVaExp;
    }
    
    void Sum::Update(typename cont::iterator& it, const Valuable& v)
    {
        Delete(it);
        Add(v);
    }
    
	Valuable Sum::operator -() const
	{
		Sum s;
		for (auto& a : members) 
			s.Add(-a);
		return s;
	}

    void Sum::optimize()
    {
        //if (!optimizations) return;

        if (isOptimizing)
            return;
        isOptimizing = true;

        Valuable w = 0_v;
        do
        {
            w = *this;
            if (members.size() == 1) {
                cont::iterator b = members.begin();
                Become(std::move(const_cast<Valuable&>(*b)));
                isOptimizing = false;
                return;
            }

            for (auto it = members.begin(); it != members.end();)
            {
                // optimize member
                auto copy = *it;
                copy.optimize();
                if (!it->Same(copy)) {
                    Update(it, copy);
                    continue;
                }
                
                if (*it == 0)
                {
                    Delete(it);
                    continue;
                }
                
                auto s = cast(*it);
                if (s) {
                    for (auto& m : s->members)
                    {
                        Add(std::move(m));
                    }
                    Delete(it);
                    continue;
                }
                
                auto it2 = it;
                ++it2;
                auto c = *it;
                auto mc = -*it;
                const Variable* va;
                const Exponentiation* e;
                const Product* p;
                const Integer* i;
                const Fraction* f;
                
                auto up = [&](){
                    mc = -c;
                    va = Variable::cast(c);
                    e = Exponentiation::cast(c);
                    p = Product::cast(c);
                    i = Integer::cast(c);
                    f = Fraction::cast(c);
                };
                
                up();
                
                for (; it2 != members.end();)
                {
                    const Fraction* f2;
                    if ((i && (f2=Fraction::cast(*it2)) && f2->IsSimple())
                        || (Integer::cast(*it2) && (i || (f && f->IsSimple())))
                        || (p && mc == *it2)
                        )
                    {
                        c += *it2;
                        Delete(it2);
                        up();
                    }
                    else if(it2->Same(c))
                    {
                        c *= 2;
                        Delete(it2);
                        up();
                    }
                    else if(it2->Same(mc))
                    {
                        c = 0_v;
                        mc = 0_v;
                        Delete(it2);
                        up();
                        mc = 0_v;
                    }
                    else if (it2->OfSameType(c)
                             && p // commonize by vars
                             && p->getCommonVars().size()
                             && p->getCommonVars()==Product::cast(*it2)->getCommonVars())
                    {
                        auto s = c + *it2;
                        s.optimize();
                        if (!Sum::cast(s)) {
                            c = s;
                            Delete(it2);
                            up();
                        }
                    }
                    else
                        ++it2;
                }

                if (it->Same(c))
                    ++it;
                else
                {
                    std::cout << it->str() << " -> " << c.str() << std::endl;
                    Update(it, c);
                }
            }

            // optimize members
            for (auto it = members.begin(); it != members.end();)
            {
                auto copy = *it;
                copy.optimize();
                if (!it->Same(copy)) {
                    Update(it, copy);
                }
                else
                    ++it;
            }
            
#ifndef NDEBUG
            if (w!=*this) {
                std::cout << "Sum optimized from \n\t" << w << "\n \t to " << *this << std::endl;
            }
#endif
        } while (w != *this);

        if (members.size() == 0) {
            Become(0_v);
        }
        
        isOptimizing = false;
    }

	Valuable& Sum::operator +=(const Valuable& v)
	{
		auto i = cast(v);
		if (i) {
			for (auto& a : i->members) {
				Add(a);
			}
		}
		else
		{
            if(!Product::cast(v))
            {
                for (auto it = members.begin(); it != members.end();)
                {
                    if(it->OfSameType(v))
                    {
                        Update(it, *it + v);
                        optimize();
                        return *this;
                    }
                    else
                        ++it;
                }
            }
            
            // add new member
			Add(v);
		}

        optimize();
		return *this;
	}

	Valuable& Sum::operator *=(const Valuable& v)
	{
        Valuable sum = 0_v;
        auto add = [&](const Valuable& m)
        {
            auto s = Sum::cast(sum);
            if (s) {
                const_cast<Sum*>(s)->Add(m);
            }
            else
                sum += m;
        };
        auto f = cast(v);
		if (f)
		{
            for (auto it = members.rbegin(); it != members.rend(); ++it) {
				for (auto& b : f->members) {
                    add(*it*b);
				}
			}
		}
		else
        {
            for (auto& a : members) {
                add(a * v);
            }
		}
        return Become(std::move(sum));
	}

	Valuable& Sum::operator /=(const Valuable& v)
	{
        Valuable s = 0_v;
		auto i = cast(v);
		if (i)
		{
			for (auto& a : members) {
				for (auto& b : i->members) {
					s += a / b;
				}
			}
		}
		else
		{
            for (auto& a : members) {
                s += a / v;
            }
		}
        return Become(std::move(s));
	}

	Valuable& Sum::operator --()
	{
		return *this += -1;
	}

	Valuable& Sum::operator ++()
	{
		return *this += 1;
	}
    
    bool Sum::IsComesBefore(const Valuable& v) const
    {
        auto s = Sum::cast(v);
        if (!s) {
            return base::IsComesBefore(v);
        }
        
        auto sz1 = size();
        auto sz2 = s->size();
        if (sz1 != sz2) {
            return sz1 > sz2;
        }
            
        for (auto i1=begin(), i2=s->begin(); i1!=end(); ++i1, ++i2) {
            if (*i1 != *i2) {
                return i1->IsComesBefore(*i2);
            }
        }
        
        return {};
    }

	std::ostream& Sum::print(std::ostream& out) const
	{
        std::stringstream s;
        s << '(';
        constexpr char sep[] = " + ";
		for (auto& b : members) 
            s << b << sep;
        auto str = s.str();
        auto cstr = const_cast<char*>(str.c_str());
        cstr[str.size() - sizeof(sep) + 1] = 0;
        out << cstr << ')';
		return out;
	}
    
    Valuable Sum::sqrt() const
    {
        return Exponentiation(*this, 1_v/2);
    }

    size_t Sum::FillPolyCoeff(std::vector<Valuable>& coefficients, const Variable& v) const
    {
        size_t grade = 0;
        coefficients.resize(members.size());
        
        #pragma omp for shared grade,coefficients
        for (auto& m : members)
        {
            auto p = Product::cast(m);
            if(p)
            {
                auto& coVa = p->getCommonVars();
                auto it = coVa.find(v);
                auto vcnt = it == coVa.end() ? 0 : it->second; // exponentation of va
                auto i = Integer::cast(vcnt);
                if (!i) {
                    throw "Implement!";
                }
                int ie = static_cast<int>(*i);
                if (ie > grade) {
                    grade = ie;
                    if (ie >= coefficients.size()) {
                        coefficients.resize(ie+1);
                    }
                }
                
                coefficients[ie] += m / (v^vcnt);
            }
            else
            {
                auto va = Variable::cast(m);
                if (va && *va == v) {
                    coefficients[1] += 1_v;
                }
                else
                {
                    auto e = Exponentiation::cast(m);
                    if (e && e->getBase()==v)
                    {
                        auto ie = Integer::cast(e->getExponentiation());
                        if (ie) {
                            auto i = static_cast<decltype(grade)>(*ie);
                            if (i > grade) {
                                grade = i;
                                if (i >= coefficients.size()) {
                                    coefficients.resize(i+1);
                                }
                            }
                            coefficients[i] += 1_v;
                        }
                        else
                            throw "Implement!";
                    }
                    else
                    {
                        coefficients[0] += m;
                    }
                }
            }
        }
        return grade;
    }
    
    /** fast linear equation formula deduction */
	Formula Sum::FormulaOfVa(const Variable& v) const
	{
        Valuable fx(0);
        std::vector<Valuable> coefficients(4);
        auto grade = FillPolyCoeff(coefficients,v);
        
        switch (grade) {
            case 2: {
                // square equation axx+bx+c=0
                // root formula: x=((b*b-4*a*c)^(1/2)-b)/(2*a)
                auto& a = coefficients[2];
                auto& b = coefficients[1];
                auto& c = coefficients[0];
                fx = ((b*b-4*a*c).sqrt()-b)/(2*a);
                break;
            }
            case 4: {
                // four grade equation ax^4+bx^3+cx^2+dx+e=0
                // see https://math.stackexchange.com/questions/785/is-there-a-general-formula-for-solving-4th-degree-equations-quartic
                auto& a = coefficients[4];
                auto& b = coefficients[3];
                auto& c = coefficients[2];
                auto& d = coefficients[1];
                auto& e = coefficients[0];
                auto sa = a*a;
                auto sb = b*b;
                auto p1 = 2*c*c*c-9*b*c*d+27*a*d*d+27*b*b*e-72*a*c*e;
                auto p2 = p1+(4*((c*c-3*b*d+12*a*e)^3)+(p1^2)).sqrt();
                auto qp2 = (p2/2)^(1_v/3);
                p1.optimize();
                p2.optimize();
                qp2.optimize();
                auto p3 = (c*c-3*b*d+12*a*e)/(3*a*qp2)+qp2/(3*a);
                auto p4 = (sb/(4*sa)-(2*c)/(3*a)+p3).sqrt();
                auto p5 = sb/(2*sa)-(4*c)/(4*a)-p3;
                auto p6 = (-sb*b/(sa*a)+4*b*c/sa-8*d/a)/(4*p4);
                fx = -b/(4*a)+p4/2+(p5+p6).sqrt()/2;
//                fx.optimize();
                break;
            }
            default: {
                throw "Implement!";
                break;
            }
        }
        
        return Formula::DeclareFormula(v, fx);
	}
    
}}
