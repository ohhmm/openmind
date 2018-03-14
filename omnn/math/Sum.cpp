//
// Created by Сергей Кривонос on 25.09.17.
//
#include "Sum.h"

#include "Formula.h"
#include "Fraction.h"
#include "Integer.h"
#include "Product.h"
#include "System.h"

#include <cmath>
#include <map>
#include <type_traits>

#ifndef NDEBUG
# define BOOST_COMPUTE_DEBUG_KERNEL_COMPILATION
#endif
#include <boost/compute.hpp>

namespace omnn{
namespace math {

    namespace
    {
        using namespace std;
        
        type_index order[] = {
            typeid(Sum),
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
            || (v1.IsProduct() && v2.IsExponentiation())
            || (v2.IsProduct() && v1.IsExponentiation())
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
            it = std::find(members.begin(), members.end(), item);
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

        Valuable w;
        do
        {
            w = *this;
            if (members.size() == 1) {
                Valuable m;
                {m = *members.begin();}
                Become(std::move(m));
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
            
            if (view == Equation) {
                auto& coVa = getCommonVars();
                if (coVa.size()) {
                    *this /= VaVal(coVa);
                    if (!IsSum()) {
                        isOptimizing = {};
                        return;
                    }
                }
            }
            
            for (auto it = members.begin(); it != members.end();)
            {
                if (it->IsSum()) {
                    for (auto& m : cast(*it)->members)
                    {
                        Add(std::move(m));
                    }
                    Delete(it);
                    continue;
                }
                
                auto it2 = it;
                ++it2;
                Valuable c = *it;
                Valuable mc;
                bool p;
                bool i;
                const Fraction* f;
                
                auto up = [&](){
                    mc = -c;
                    p = c.IsProduct();
                    i = c.IsInt();
                    f = c.IsFraction() ? Fraction::cast(c) : nullptr;
                };
                
                up();
                
                for (; it2 != members.end();)
                {
                    if ((i && it2->IsFraction() && Fraction::cast(*it2)->IsSimple())
                        || (it2->IsInt() && (i || (f && f->IsSimple())))
                        || (c.IsProduct() && mc == *it2)
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
                            && c.getCommonVars()==it2->getCommonVars())
                    {
                        auto sum = c.IsProduct() ? c : Product{c};
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
                
                // todo : make all va exponentiations > 0
            }
            else if (view == Equation) {
                auto& coVa = getCommonVars();
                if (coVa.size()) {
                    *this /= VaVal(coVa);
                    if (!IsSum()) {
                        isOptimizing = {};
                        return;
                    }
                }
            }
        }
        
        isOptimizing = false;
    }

    const Valuable::vars_cont_t& Sum::getCommonVars() const
    {
        vars.clear();
        
        auto it = members.begin();
        if (it != members.end())
        {
            vars = it->getCommonVars();
            ++it;
            while (it != members.end()) {
                auto& _ = it->getCommonVars();
                for(auto i = vars.begin(); i != vars.end();)
                {
                    auto found = _.find(i->first);
                    if(found != _.end())
                    {
                        auto mm = std::minmax(i->second, found->second,
                                              [](auto& _1, auto& _2){return _1.abs() < _2.abs();});
                        if ((i->second < 0) == (found->second < 0)) {
                            i->second = mm.first;
                        }
                        else
                        {
                            auto sign = mm.second / mm.second.abs();
                            i->second = sign * (mm.second.abs() - mm.first.abs());
                        }
                        ++i;
                    }
                    else
                    {
                        vars.erase(i++);
                    }
                }
                ++it;
            }
        }

        return vars;
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
        Valuable sum = Sum{};
        auto add = [&](const Valuable& m)
        {
            if (sum.IsSum()) {
                Sum::cast(sum)->Add(m);
            }
            else
                sum += m;
        };
        if (v.IsSum())
		{
            for(auto& _1 : *cast(v))
				for (auto& _2 : members)
                    add(_1*_2);
		}
		else
        {
            for (auto& a : members) {
                add(a * v);
            }
		}
        Become(std::move(sum));
        return *this;
	}

	Valuable& Sum::operator /=(const Valuable& v)
	{
        Valuable s = 0_v;
		if (v.IsSum())
		{
            auto i = cast(v);
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
                if (size() < i->size())
                {
                    s = Fraction(*this, v);
                }
                else if (HasSameVars(v))
                {
                    auto b=i->begin();
                    auto e = i->end();
                    size_t offs = 0;
                    std::deque<Valuable> hist {*this};
                    
                    auto icnt = size() * 2;
                    while(*this != 0_v && icnt--)
                    {
                        if(IsSum())
                        {
                            auto it = begin();
                            for (auto i=offs; it != end() && i--; ) {
                                ++it;
                            }
                            if (it == end()) {
                                IMPLEMENT;
                            }
                            auto vars = it->Vars();
                            auto coVa = it->getCommonVars();
                            auto maxVa = std::max_element(coVa.begin(), coVa.end(),
                                                          [](auto&_1,auto&_2){return _1.second < _2.second;});
                            auto it2 = b;
                            while(it2 != e && it2->Vars() != vars)
                                ++it2;
                            if (it2 == e) {
                                
                                for (it2 = b; it2 != e; ++it2)
                                {
                                    bool found = {};
                                    for(auto& v : it2->Vars())
                                    {
                                        found = v == maxVa->first;
                                        if (found) {
                                            auto coVa2 = it2->getCommonVars();
                                            auto coVa2vIt = coVa2.find(v);
                                            if (coVa2vIt == coVa2.end()) {
                                                IMPLEMENT
                                            }
                                            auto coVa1vIt = coVa.find(v);
                                            if (coVa1vIt == coVa.end()) {
                                                IMPLEMENT
                                            }
                                            found = coVa1vIt->second >= coVa2vIt->second;
                                            
                                        }
                                        
                                        if (!found) {
                                            break;
                                        }
                                    }
                                    
                                    if(found)
                                        break;
                                }
                                
                                if (it2 == e) {
                                    IMPLEMENT;
                                }
                            }

                            auto t = *begin() / *it2;
                            s += t;
                            t *= v;
                            *this -= t;
                            if (std::find(hist.begin(), hist.end(), *this) == hist.end()) {
                                hist.push_back(*this);
                                constexpr size_t MaxHistSize = 8;
                                if (hist.size() > MaxHistSize) {
                                    hist.pop_front();
                                    offs = 0;
                                }
                            }
                            else
                            {
                                ++offs;
                                hist.clear();
                            }
                        }
                        else
                        {
                            s += *this / v;
                            break;
                        }
                    }
                    if (*this != 0_v) {
                        s = Fraction(*this, v);
                    }
                }
                else
                {
                    s = Fraction(*this, v);
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
    
    Sum::Sum(const std::initializer_list<Valuable>& l)
    {
        for (const auto& arg : l)
        {
            auto a = cast(arg);
            if(a)
                for(auto& m: *a)
                    this->Add(m);
            else
                this->Add(arg);
        }
    }
    
    bool Sum::IsComesBefore(const Valuable& v) const
    {
        if (!v.IsSum()) {
            return base::IsComesBefore(v);
        }
        
        auto sz1 = size();
        auto s = Sum::cast(v);
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
                    IMPLEMENT
                }
                int ie = static_cast<int>(*i);
                if (ie < 0)
                {
                    coefficients.clear();
                    return cast(*this * (v^(-ie)))->FillPolyCoeff(coefficients, v);
                }
                else if (ie > grade) {
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
                            IMPLEMENT
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
    
    Valuable::solutions_t Sum::operator()(const Variable& va, const Valuable& augmentation) const
    {
        solutions_t solutions;
        {
        auto e = *this + augmentation;
        if (!e.IsSum()) {
            IMPLEMENT;
        }
        auto es = cast(e);
        std::vector<Valuable> coefs;
        auto grade = es->FillPolyCoeff(coefs, va);
        if (coefs.size() && grade && grade <= 2)
        {
            es->solve(va, solutions, coefs, grade);
            for (auto i=solutions.begin(); i != solutions.end();) {
                if (i->HasVa(va)) {
                    IMPLEMENT
                    solutions.erase(i++);
                }
                else
                    ++i;
            }
            
            if (solutions.size()) {
                return solutions;
            }
        }
        }
        Valuable _ = augmentation;
        Valuable todo = 0_v;
        for(auto& m : *this)
        {
            if (m.HasVa(va)) {
                todo += m;
            } else {
                _ -= m;
            }
        }
        
        if (todo.IsSum()) {
            auto coVa = todo.getCommonVars();
            auto it = coVa.find(va);
            if (it != coVa.end()) {
                todo /= it->first ^ it->second;
                if (todo.HasVa(va))
                    IMPLEMENT;
                if (it->second < 0) {
                    _ *= it->first ^ -it->second;
                    return _(va, todo);
                }
                else if (todo != 0_v)
                {
                    Valuable solution = Fraction(_, todo);
                    if (it->second != 1_v) {
                        solution ^= 1_v/it->second;
                    }
                    else
                        solution.optimize();
                    solutions.insert(solution);
                }
            }
            else
            {
                auto stodo = cast(todo);
                if (stodo->size() == 2)
                {
                    if(stodo->begin()->IsFraction())
                    {
                        solutions = (*stodo->begin())(va, _ - *stodo->rbegin());
                    }
                    else if(stodo->rbegin()->IsFraction())
                    {
                        solutions = (*stodo->rbegin())(va, _ - *stodo->begin());
                    }
                    else
                    {
                        IMPLEMENT
                    }
                }
                else
                {
                    solutions.insert(0_v);
                    for(auto& m : *stodo)
                    {IMPLEMENT
                        solutions_t incoming(std::move(solutions));
                        for(auto& e : m(va, _))
                        {
                            IMPLEMENT // test it
                            for(auto& s : incoming)
                            {
                                solutions.insert(s + e);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            solutions = todo(va, _);
        }

        return solutions;
    }
    
    Valuable::solutions_t Sum::operator()(const Variable& va) const
    {
        Valuable::solutions_t s;
        
        std::vector<Valuable> coefs;
        auto grade = FillPolyCoeff(coefs, va);
        if (coefs.size() && grade && grade <= 2)
        {
            solve(va, s, coefs, grade);
            for (auto i=s.begin(); i != s.end();) {
                if (i->HasVa(va)) {
                    IMPLEMENT
                    s.erase(i++);
                }
                else
                    ++i;
            }
        }

        if (!s.size())
        {
            Valuable augmentation = 0_v;
            Valuable _ = 0_v;
            for(auto& m : *this)
            {
                if (m.HasVa(va)) {
                    _ += m;
                } else {
                    augmentation -= m;
                }
            }
            s = _(va, augmentation);
        }
        
        return s;
    }
    
    void Sum::solve(const Variable& va, solutions_t& solutions) const
    {
        std::vector<Valuable> coefficients;
        auto grade = FillPolyCoeff(coefficients, va);
        solve(va, solutions, coefficients, grade);
    }
    
    void Sum::solve(const Variable& va, solutions_t& solutions, const std::vector<Valuable>& coefficients, size_t grade) const
    {        
        switch (grade) {
            case 1: {
                //x=-(b/a)
                auto& b = coefficients[0];
                auto& a = coefficients[1];
                solutions.insert(-b / a);
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
                for(auto i = wgsz; i-->0;)
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
                }
                
                if(solutions.size() == grade)
                    return;
                
                // decomposition
//                auto yx = -*this;
//                Variable y;
//                yx.Valuable::Eval(va, y);
//                yx += *this;
//                yx /= va - y;
//                auto _ = yx.str();
                
                sz = grade + 1;
                auto sza = (grade >> 1) + (grade % 2) + 1;
                auto szb = (grade >> 1) + 1;
                std::vector<Variable> vva(sza);
                std::vector<Variable> vvb(szb);
                Valuable eq1, eq2;
                for (auto i = sza; i--; ) {
                    eq1 += vva[i] * (va ^ i);
                }
                for (auto i = szb; i--; ) {
                    eq2 += vvb[i] * (va ^ i);
                }
                
                auto teq = eq1*eq2;
                std::vector<Valuable> teq_coefficients;
                auto teqs = Sum::cast(teq);
                if (teqs) {
                    if (teqs->FillPolyCoeff(teq_coefficients, va) != grade) {
                        IMPLEMENT
                    }
                } else {
                    IMPLEMENT
                }
                System sequs;
                for (auto i = sz; i--; ) {
                    auto c = coefficients[i];
                    Valuable eq = teq_coefficients[i]-c;
                    sequs << eq;
                }
                //sequs << copy;
                std::vector<solutions_t> s(szb);
                for (auto i = szb; i--; ) {
                    s[i] = sequs.Solve(vvb[i]);
                }
                
                auto ss = sequs.Solve(vvb[0]);
                if (ss.size()) {
                    for(auto& s : ss)
                    {
                        copy /= va - s;
                    }
                    copy.solve(va, solutions);
                }
                else
                    IMPLEMENT;
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
