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
#include <type_traits>

#include <boost/compute.hpp>

namespace omnn{
namespace math {

    namespace
    {
        using namespace std;
        
        type_index order[] = {
            //typeid(Sum),
            typeid(Product),
            typeid(Exponentiation),
            typeid(Variable),
            typeid(Integer),
            typeid(Fraction),
        };
        
        auto ob = std::begin(order);
        auto oe = std::end(order);
        
        // inequality should cover all cases
        auto toc = [](const Valuable& x, const Valuable& y) // type order comparator
        {
            auto it1 = std::find(ob, oe, static_cast<type_index>(x));
            assert(it1!=oe); // IMPLEMENT
            auto it2 = std::find(ob, oe, static_cast<type_index>(y));
            assert(it2!=oe); // IMPLEMENT
            return it1 == it2 ? *it1 > *it2 : it1 < it2;
        };
        
        SumOrderComparator soc;
    }
    
    // store order operator
    bool SumOrderComparator::operator()(const Valuable& v1, const Valuable& v2) const
    {
        return v1.OfSameType(v2)
            || (Product::cast(v1) && Exponentiation::cast(v2))
            || (Product::cast(v2) && Exponentiation::cast(v1))
            || (Product::cast(v1) && Variable::cast(v2))
            || (Product::cast(v2) && Variable::cast(v1))
            ? v1.IsComesBefore(v2) : toc(v1,v2);
    }

    const Sum::iterator Sum::Add(const Valuable& item)
    {
        Sum::iterator it = end();
        if(item.IsSum()) {
            auto s = cast(item);
            for(auto& i : *s) {
                auto a = Add(i);
                if (it == end() || soc(*a, *it)) {
                    it = a;
                }
            }
            it = members.begin();
        }
        else
        {
            it = std::find(begin(), end(), item);
            if(it==end())
                it = base::Add(item);
            else
                Update(it, item*2);
            auto itemMaxVaExp = item.getMaxVaExp();
            if(itemMaxVaExp > maxVaExp)
                maxVaExp = itemMaxVaExp;
        }
        return it;
    }
    
    void Sum::Update(iterator& it, const Valuable& v)
    {
        Delete(it);
        it=Add(v);
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
                
                ++it;
            }
            
            for (auto it = members.begin(); it != members.end();)
            {
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
                        Delete(it2);
                        up();
                    }
                    else if (c.getCommonVars().size()
                             //&& (!s || !Sum::cast(*it2))
                             //&& (!p || p->GetFirstOccurence<Sum>() != p->end())
                            && c.getCommonVars()==it2->getCommonVars())
                    {
                        auto sum = p ? c : Product(c);
                        sum += *it2;
                        if (!sum.IsSum()) {
                            c = sum;
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

        if(IsSum())
        {
            if (members.size() == 0) {
                Become(0_v);
            }
            else if (members.size() == 1) {
                cont::iterator b = members.begin();
                Become(std::move(const_cast<Valuable&>(*b)));
            }
            else if (view==Solving){
                // make coefficients int to use https://simple.wikipedia.org/wiki/Rational_root_theorem
                bool checked = false;
                while (!checked) {
                    for(auto& m : members)
                    {
                        if (m.IsProduct()) {
                            auto p = Product::cast(m);
                            auto f = p->GetFirstOccurence<Fraction>();
                            if (f != p->end()) {
                                operator*=(Fraction::cast(*f)->getDenominator());
                                break;
                            }
                        } else if (m.IsFraction()) {
                            operator*=(Fraction::cast(m)->getDenominator());
                            break;
                        }
                    }
                    checked = true;
                }
            }
        }
        
        isOptimizing = false;
    }

	Valuable& Sum::operator +=(const Valuable& v)
	{
		if (v.IsSum()) {
			Add(v);
		}
		else
		{
            for (auto it = members.begin(); it != members.end(); ++it)
            {
                if(it->OfSameType(v) && it->getCommonVars()==v.getCommonVars())
                {
                    auto s = *it + v;
                    if (!Sum::cast(s)) {
                        Update(it, s);
                        optimize();
                        return *this;
                    }
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
        //auto was = Valuable::optimizations;
        //Valuable::optimizations = {};
        Become(std::move(sum));
        //Valuable::optimizations = was;
        return *this;
	}

	Valuable& Sum::operator /=(const Valuable& v)
	{
        Valuable s = 0_v;
		auto i = cast(v);
		if (i)
		{
            if(!v.FindVa())
            {
                for (auto& a : members) {
                    for (auto& b : i->members) {
                        s += a / b;
                    }
                }
            }
            else
            {
                while(*this != 0_v)
                {
                    if(cast(*this))
                    {
                        auto t = *begin() / *i->begin();
                        s += t;
                        t *= v;
                        *this -= t;
                    }
                    else
                    {
                        s += *this / v;
                        break;
                    }
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
    
    Valuable& Sum::d(const Variable& x)
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
        for(auto m : *this)
            add(m.d(x));
        
        return Become(std::move(sum));
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

    Valuable Sum::calcFreeMember() const
    {
        Valuable _ = 0_v;
        for(auto& m : *this) {
            _ += m.calcFreeMember();
        }
        return _;
    }

    size_t Sum::FillPolyCoeff(std::vector<Valuable>& coefficients, const Variable& v) const
    {
        size_t grade = 0;
        coefficients.resize(members.size());
        
        #pragma omp for shared grade,coefficients
        for (auto& m : members)
        {
            if(m.IsProduct())
            {
                auto& coVa = m.getCommonVars();
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
    
    void Sum::solve(const Variable& va, std::set<Valuable>& solutions) const
    {
        std::vector<Valuable> coefficients;
        auto grade = FillPolyCoeff(coefficients, va);
        switch (grade) {
            case 1: {
                IMPLEMENT
                break;
            }
            case 2: {
                // square equation x=(-b+√(b*b-4*a*c))/(2*a)
                auto& a = coefficients[2];
                auto& b = coefficients[1];
                auto& c = coefficients[0];
                auto d = (b ^ 2) - 4_v * a * c;
                auto dsq = d.sqrt();
                auto a2 = a * 2;
                solutions.insert((-dsq-b)/a2);
                solutions.insert((dsq-b)/a2);
                break;
            }
//            case 3: {
//                auto& a = coefficients[3];
//                auto& b = coefficients[2];
//                auto& c = coefficients[1];
//                auto& d = coefficients[0];
//                auto di = (b^2)*(c^2) - 4_v*a*(c^3) - 4_v*(b^3)*d - 27_v*(a^2)*(d^2) + 18_v*a*b*c*d;
//                solutions.insert(
//                break;
//            }
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
                auto xp1 = b/(-4*a);
                auto xp2 = p4/2;
                auto xp3_1 = (p5-p6).sqrt()/2;
                auto xp3_2 = (p5+p6).sqrt()/2;
                solutions.insert(xp1-xp2-xp3_1);
                solutions.insert(xp1-xp2+xp3_1);
                solutions.insert(xp1+xp2-xp3_2);
                solutions.insert(xp1+xp2+xp3_2);
                break;
            }
            default: {
                // build OpenCL kernel
                using namespace boost::compute;
                auto copy = *this;
                copy.optimize();
                std::stringstream source;
                source << "__kernel void f(__global long *c) {"
                        << "    const uint i = get_global_id(0);"
                        << "    long " << va << "=i;"
                        << "    c[i] = "; copy.code(source);
                source << ";}";
                
                device cuwinner = system::default_device();
                for(auto& p: system::platforms())
                    for(auto& d: p.devices())
                        if (d.compute_units() > cuwinner.compute_units())
                            cuwinner = d;
                auto wgsz = cuwinner.max_work_group_size();
                context context(cuwinner);
                
                kernel k(program::build_with_source(source.str(), context), "f");
                auto sz = wgsz * sizeof(cl_long);
                buffer c(context, sz);
                k.set_arg(0, c);
                
                command_queue queue(context, cuwinner);
                // run the add kernel
                queue.enqueue_1d_range_kernel(k, 0, wgsz, 0);
                
                // transfer results to the host array 'c'
                std::vector<cl_long> z(wgsz);
                queue.enqueue_read_buffer(c, 0, sz, &z[0]);
                queue.finish();
                
                std::vector<Valuable> newSolutions;
                for(int i = wgsz; i-->0;)
                    if (z[i] == 0) {
                        // lets recheck on host
                        auto copy = *this;
                        copy.Eval(va, i);
                        copy.optimize();
                        if (copy == 0_v) {
                            auto it = solutions.insert(i);
                            if (it.second) { // new value
                                newSolutions.push_back(i);
                            }
                        }
                    }
                
                if(solutions.size() == grade)
                    return;
                
                if (newSolutions.size()) {
                    Valuable newSolutionsPoly = 1_v;
                    for(auto solution : newSolutions)
                    {
                        newSolutionsPoly *= va - solution;
                    }
                    newSolutionsPoly.optimize();
                    auto copy = *this / newSolutionsPoly;
                    copy.solve(va, solutions);
                    
                    if(solutions.size() == grade)
                        return;
                } else {
                    
                }
                
                // TODO : IMPLEMENT
                
                break;
            }
        }
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
