//
// Created by Сергей Кривонос on 01.09.17.
//
#include "Valuable.h"

#include "i.h"
#include "Fraction.h"
#include "Infinity.h"
#include "Integer.h"
#include "VarHost.h"
#include "Sum.h"
#include <iterator>
#include <string>
#include <stack>
#include <map>
#include <algorithm>
#include <boost/numeric/conversion/converter.hpp>

namespace omnn{
namespace math {
    const a_int Valuable::a_int_cz = 0;
    const max_exp_t Valuable::max_exp_cz(a_int_cz);

    omnn::math::Valuable::YesNoMaybe operator||(omnn::math::Valuable::YesNoMaybe _1, omnn::math::Valuable::YesNoMaybe _2){
        constexpr omnn::math::Valuable::YesNoMaybe OrMap[] = {
            // Yes = 1, Maybe = 10, No = 100
            {},              // 000 bug
            omnn::math::Valuable::YesNoMaybe::Yes, // 001 yes
            omnn::math::Valuable::YesNoMaybe::Maybe, // 010 maybe
            omnn::math::Valuable::YesNoMaybe::Yes,   // 011  yes, maybe
            omnn::math::Valuable::YesNoMaybe::No, // 100 no
            omnn::math::Valuable::YesNoMaybe::Yes, // 101 yes,no
            omnn::math::Valuable::YesNoMaybe::Maybe, // 110 maybe,no
            omnn::math::Valuable::YesNoMaybe::Yes, // 111 yes,maybe,no
        };
        return OrMap[static_cast<uint8_t>(_1) | static_cast<uint8_t>(_2)];
    }
    omnn::math::Valuable::YesNoMaybe operator&&(omnn::math::Valuable::YesNoMaybe _1, omnn::math::Valuable::YesNoMaybe _2){
        constexpr omnn::math::Valuable::YesNoMaybe AndMap[] = {
            // Yes = 1, Maybe = 10, No = 100
            {},              // 000 bug
            omnn::math::Valuable::YesNoMaybe::Yes, // 001 yes
            omnn::math::Valuable::YesNoMaybe::Maybe, // 010 maybe
            omnn::math::Valuable::YesNoMaybe::Maybe,   // 011  yes, maybe
            omnn::math::Valuable::YesNoMaybe::No, // 100 no
            omnn::math::Valuable::YesNoMaybe::No, // 101 yes,no
            omnn::math::Valuable::YesNoMaybe::No, // 110 maybe,no
            omnn::math::Valuable::YesNoMaybe::No, // 111 yes,maybe,no
        };
        return AndMap[static_cast<uint8_t>(_1) | static_cast<uint8_t>(_2)];
    }

    thread_local bool Valuable::optimizations = true;
    thread_local bool Valuable::bit_operation_optimizations = {};
    thread_local bool Valuable::enforce_solve_using_rational_root_test_only = {};
    
    
    void implement()
    {
        throw "Implement!";
        return;
    }
    
    Valuable* Valuable::Clone() const
    {
        if (exp)
            return exp->Clone();
        else
            IMPLEMENT
    }

    Valuable* Valuable::Move()
    {
        if (exp)
            return exp->Move();
        else
            IMPLEMENT
    }
    
    void Valuable::New(void*, Valuable&&)
    {
        IMPLEMENT
    }
   
    int Valuable::getTypeSize() const
    {
        return sizeof(Valuable);
    }

    size_t Valuable::getAllocSize() const
    {
        return sz;
    }
    
    void Valuable::setAllocSize(size_t sz)
    {
        this->sz = sz;
    }
    
    Valuable::Valuable(const Valuable& v, ValuableDescendantMarker)
    : hash(v.Hash()), maxVaExp(v.getMaxVaExp()), view(v.view), optimized(v.optimized)
    {
        assert(!exp);
    }

    Valuable::Valuable(Valuable&& v, ValuableDescendantMarker)
    : hash(v.Hash()), maxVaExp(v.getMaxVaExp()), view(v.view), optimized(v.optimized)
    {
        assert(!exp);
    }

    Valuable::operator std::type_index() const
    {
        return std::type_index(typeid(get()));
    }
    
    Valuable& Valuable::Become(Valuable&& i)
    {
        auto newWasView = GetView(); // TODO: fix it, supervise all View usages
        auto h = i.Hash();
        auto e = i.exp;
        if(e)
        {
            while (e->exp) {
                e = e->exp;
            }
           
            if(exp)
            {
                exp = e;
                if (Hash() != h) {
                    IMPLEMENT
                }
            }
            else
            {
                Become(std::move(*e));
            }
            
            e.reset();
        }
        else
        {
            auto sizeWas = getAllocSize();
            auto newSize = i.getTypeSize();
            
            if (newSize <= sizeWas) {
                constexpr decltype(newSize) BufSz = 768;
                assert(BufSz >= newSize && "Increase BufSz");
                char buf[BufSz];
                i.New(buf, std::move(i));
                Valuable& bufv = *reinterpret_cast<Valuable*>(buf);
                this->~Valuable();
				bufv.New(this, std::move(bufv));
                setAllocSize(sizeWas);
                if (Hash() != h) {
                    IMPLEMENT
                }
                optimize();
            }
            else if(exp && exp->getAllocSize() >= newSize)
            {
                exp->Become(std::move(i));
            }
            else
            {
                auto moved = i.Move();
                this->~Valuable();
                new(this) Valuable(moved);
                setAllocSize(sizeWas);
                if (Hash() != h) {
                    IMPLEMENT
                }
                optimize();
            }
        }
        SetView(newWasView);
        return *this;
    }

    Valuable& Valuable::operator =(Valuable&& v)
    {
        return Become(std::move(v));
    }

    Valuable& Valuable::operator =(const Valuable& v)
    {
        exp.reset(v.Clone());
        return *this;
    }

    Valuable::Valuable(const Valuable& v) :
        exp(v.Clone())
    {
    }

    Valuable::Valuable(Valuable* v) :
        exp(v)
    {
    }
    Valuable::Valuable(): exp(new Integer(Valuable::a_int_cz)) {}
    Valuable::Valuable(double d) : exp(new Fraction(d)) { exp->optimize(); }
    Valuable::Valuable(a_int&& i) : exp(new Integer(std::move(i))) {}
    Valuable::Valuable(const a_int& i) : exp(new Integer(std::move(i))) {}
    Valuable::Valuable(boost::rational<a_int>&& r) : exp(new Fraction(std::move(r))) { exp->optimize(); }

    Variable x,a,b,c;
//    auto MergeOrF = x.Equals((Exponentiation((b ^ 2) - 4_v * a * c, 1_v/2)-b)/(a*2));
//    auto aMergeOrF = MergeOrF(a);
//    auto bMergeOrF = MergeOrF(b);
//    auto cMergeOrF = MergeOrF(c);
    Valuable Valuable::MergeOr(const Valuable& v1, const Valuable& v2)
    {
        if(v1.IsInt() && v2.IsInt()
           && v1.abs()==v2.abs())
        {
            return v1.abs() * (1_v ^ Fraction{1,2});
        }
        else {
            Variable x;
            auto s = x.Equals(v1).logic_or(x.Equals(v2));
            if(s.IsSum())
            {
                auto sum = Sum::cast(s);
                std::vector<Valuable> coefficients;
                sum->FillPolyCoeff(coefficients, x);
                auto& a = coefficients[2];
                auto& b = coefficients[1];
                auto& c = coefficients[0];
                auto d = (b ^ 2) - 4_v * a * c;
                return (Exponentiation(d, 1_v/2)-b)/(a*2);
            }else{
                IMPLEMENT
            }
            return x;
        }
    }

    Valuable Valuable::MergeAnd(const Valuable& v1, const Valuable& v2)
    {
        Variable x;
        return ((v1+v2)+((-1_v)^(1_v/2))*(v1-v2))/2;
    }

    Valuable::Valuable(const solutions_t& s)
    {
        switch (s.size()) {
        case 0: IMPLEMENT; break;
        case 1: operator=(*s.begin()); break;
        case 2: {
            auto it = s.begin();
            operator=(MergeOr(*it, *++it));
            break;
        }
        case 3: {
            auto it = s.begin();
            operator=(MergeOr(MergeOr(*it, *++it), *++it));
            break;
        }
        case 4: {
            auto it = s.begin();
            operator=(MergeOr(MergeOr(*it, *++it), MergeOr(*++it, *++it)));
            break;
        }
        default:
            IMPLEMENT // just do same as case 2 for each couple in the set in paralell and then to the resulting set 'recoursively'
        }
    }

    Valuable::Valuable(const std::string& s, std::shared_ptr<VarHost> h)
    {
        auto l = s.length();
        using index_t = decltype(l);
        std::stack <index_t> st;
        std::map<index_t, index_t> bracketsmap;
        int c = 0;
        std::string numbers = "0123456789";
        while (c < l)
        {
            if (s[c] == '(')
                st.push(c);
            else if (s[c] == ')')
            {
                bracketsmap.insert({st.top(), c});
                st.pop();
            }
            c++;
        }
        if (!st.empty())
            throw "parentneses relation missmatch";
        if (bracketsmap.empty())
        {
            std::size_t found = s.find("*");
            if (found != std::string::npos)
            {
                std::string lpart = s.substr(0, found);
                std::string rpart = s.substr(found + 1, s.length() - found);
                Become(Valuable(lpart, h)*Valuable(rpart, h));
            }
            else
            {
                found = s.find("^");
                if (found != std::string::npos)
                {
                    std::string lpart = s.substr(0, found);
                    std::string rpart = s.substr(found + 1, s.length() - found);
                    Become(Valuable(lpart, h) ^ Valuable(rpart, h));
                }
                else
                {
                    found = s.find_first_not_of("0123456789");
                    if (found == std::string::npos)
                    {
                        Become(Integer(s));
                    }
                    else
                    {
                        if (s[0] == 'v')
                            Become(Valuable(h->Host(std::stoi(s.c_str()+1)).Clone()));
                        else
                            IMPLEMENT
                    }
                }
            }
        }
        else {
            Sum sum;
            Valuable v;
            auto o_mov = [&](Valuable&& val) {
                v = std::move(val);
            };
            auto o_mul = [&](Valuable&& val) {
                v *= std::move(val);
            };
            auto o_e = [&](Valuable&& val) {
                v ^= std::move(val);
            };
            std::function<void(Valuable&&)> o = o_mov;
            //std::stack<char> op;
            for (index_t i = 0; i < l; ++i)
            {
                auto c = s[i];
                if (c == '(')
                {
                    auto cb = bracketsmap[i];
                    auto next = i + 1;
                    o(Valuable(s.substr(next, cb - next), h));
                    i = cb;
                }
                else if (c == 'v') {
                    auto next = s.find_first_not_of("0123456789", i+1);
                    auto id = s.substr(i + 1, next - i);
                    o(std::move(h->New(std::stoi(id))));
                    i = next - 1;
                }
                else if ( (c >= '0' && c <= '9') || c == '-') {
                    auto next = s.find_first_not_of("0123456789", i+1);
                    o(Integer(s.substr(i, next - i)));
                    i = next - 1;
                }
                else if (c == '+') {
                    sum.Add(v);
                    v = 0;
                    o = o_mov;
                }
                else if (c == '*') {
                    o = o_mul;
                }
                else if (c == '^') {
                    IMPLEMENT
                    auto _ = o;
                    o = o_e;
                }
                else if (c == ' ') {
                }
                else {
                    IMPLEMENT
                }
            }
            sum.Add(v);
            Become(std::move(sum));
        }

        auto _ = str();
        if ((_.front() == '(' && _.back() == ')') && !(s.front() == '(' && s.back() == ')') )
            _ = _.substr(1, _.length()-2);
        
        if (s != _)
            IMPLEMENT;
    }

    Valuable::Valuable(const std::string& str, const Valuable::va_names_t& vaNames, bool itIsOptimized){
		auto optimizationsWas = Valuable::optimizations;
		Valuable::optimizations = !itIsOptimized && optimizationsWas;
		// remove spaces
		auto s = str;
		s.erase(remove_if(s.begin(), s.end(), isspace), s.end());
        auto l = s.length();
        using index_t = decltype(l);
        std::stack <index_t> st;
        std::map<index_t, index_t> bracketsmap;
        int c = 0;
        std::string numbers = "0123456789";
        while (c < l)
        {
            if (s[c] == '(')
                st.push(c);
            else if (s[c] == ')')
            {
                bracketsmap.insert({st.top(), c});
                st.pop();
            }
            c++;
        }
        if (!st.empty())
            throw "parentneses relation missmatch";
        if (bracketsmap.empty())
        {
            auto found = s.find("+");
            if (found != std::string::npos) {
                auto lpart = s.substr(0, found);
				auto rpart = s.substr(found + 1, s.length() - found);
                auto l = Valuable(lpart, vaNames, itIsOptimized);
                if (itIsOptimized)
                    l.optimized = itIsOptimized;
                auto r = Valuable(rpart, vaNames, itIsOptimized);
                if (itIsOptimized)
                    r.optimized = itIsOptimized;
                auto sum = l+r;
                if (itIsOptimized)
					sum.optimized = itIsOptimized;
                Become(std::move(sum));
            } else if ((found = s.find("*")) != std::string::npos) {
                auto lpart = s.substr(0, found);
				auto rpart = s.substr(found + 1, s.length() - found);
                auto l = Valuable(lpart, vaNames, itIsOptimized);
                if (itIsOptimized)
                    l.optimized = itIsOptimized;
                auto r = Valuable(rpart, vaNames, itIsOptimized);
                if (itIsOptimized)
                    r.optimized = itIsOptimized;
                auto product = l*r;
                if (itIsOptimized)
                    product.optimized = itIsOptimized;
                Become(std::move(product));
			} else if((found = s.find("^")) != std::string::npos) {
                std::string lpart = s.substr(0, found);
                std::string rpart = s.substr(found + 1, s.length() - found);
                auto l = Valuable(lpart, vaNames, itIsOptimized);
                if (itIsOptimized)
                    l.optimized = itIsOptimized;
                auto r = Valuable(rpart, vaNames, itIsOptimized);
                if (itIsOptimized)
                    r.optimized = itIsOptimized;
                auto exp = l ^ r;
                if (itIsOptimized)
                    exp.optimized = itIsOptimized;
                Become(std::move(exp));
            }
			else
			{
				size_t offs = 0;
				while (s[offs] == ' ')
					++offs;
				if (s[offs] == '-')
					++offs;
				found = s.find_first_not_of("0123456789", offs);
				if (found == std::string::npos)
					Become(Integer(s));
				else
				{
					auto it = vaNames.find(s);
					if (it != vaNames.end()) {
						Valuable v(it->second);
						if (itIsOptimized)
							v.optimized = itIsOptimized;
						Become(std::move(v));
					}
					else
						IMPLEMENT
				}
			}

        }
        else {
            Sum sum;
            Valuable v;
            auto o_mov = [&](Valuable&& val) {
                v = std::move(val);
            };
            auto o_mul = [&](Valuable&& val) {
                v *= std::move(val);
            };
            auto o_e = [&](Valuable&& val) {
                v ^= std::move(val);
            };
            std::function<void(Valuable&&)> o = o_mov;
            //std::stack<char> op;
            for (index_t i = 0; i < l; ++i)
            {
                auto c = s[i];
                if (c == '(')
                {
                    auto cb = bracketsmap[i];
                    auto next = i + 1;
                    Valuable v(s.substr(next, cb - next), vaNames, itIsOptimized);
                    if (itIsOptimized)
                        v.optimized = itIsOptimized;
                    o(std::move(v));
                    i = cb;
                }
                else if ( (c >= '0' && c <= '9') || c == '-') {
                    auto next = s.find_first_not_of("0123456789", i+1);
                    o(Integer(s.substr(i, next - i)));
                    i = next - 1;
                }
                else if (c == '+') {
                    sum.Add(v);
                    v = 0;
                    o = o_mov;
                }
                else if (c == '*') {
                    o = o_mul;
                }
                else if (c == '^') {
                    IMPLEMENT
                    auto _ = o;
                    o = o_e;
                }
                else if (c == ' ') {
                }
                else if (std::isalnum(c)) {
                    auto b = i;
                    while (i < l && std::isalnum(s[++i]));
                    auto id = s.substr(b, i - b);
                    auto it = vaNames.find(id);
                    if (it != vaNames.end()){
                        Valuable v(it->second);
                        if (itIsOptimized)
                            v.optimized = itIsOptimized;
                        o(std::move(v));
                    }else
                        IMPLEMENT
                }
                else {
                    IMPLEMENT
                }
            }
            sum.Add(v);
            if (itIsOptimized)
                sum.optimized = itIsOptimized;
            Become(std::move(sum));
			Valuable::optimizations = optimizationsWas;
        }

        auto _ = this->str();
        if ((_.front() == '(' && _.back() == ')') && !(s.front() == '(' && s.back() == ')') )
            _ = _.substr(1, _.length()-2);
        
		_.erase(remove_if(_.begin(), _.end(), isspace), _.end());
        if (s != _)
            IMPLEMENT;
    }

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
        if(exp) {
            Valuable& o = exp->operator+=(v);
            if (o.exp) {
                exp = o.exp;
                return *this;
            }
            return o;
        }
        else
            IMPLEMENT
    }
    
    Valuable& Valuable::operator +=(int v)
    {
        if(exp) {
            Valuable& o = exp->operator+=(v);
            if (o.exp) {
                exp = o.exp;
            }
            return *this;
        }
        else
            IMPLEMENT
    }

    Valuable& Valuable::operator *=(const Valuable& v)
    {
        if(exp) {
            Valuable& o = exp->operator*=(v);
            if (o.exp) {
                exp = o.exp;
            }
            return *this;
        }
        else
            IMPLEMENT
    }

    a_int Valuable::Complexity() const
    {
        if(exp)
            return exp->Complexity();
        IMPLEMENT
    }
    
    
    bool Valuable::MultiplyIfSimplifiable(const Valuable& v)
    {
        if(exp)
            return exp->MultiplyIfSimplifiable(v);
        IMPLEMENT
    }

    std::pair<bool,Valuable> Valuable::IsMultiplicationSimplifiable(const Valuable& v) const
    {
        if(exp)
            return exp->IsMultiplicationSimplifiable(v);
        else {
            IMPLEMENT
            auto m = *this * v;
            return { m.Complexity() < Complexity() + v.Complexity(), m };
        }
    }
    
    Valuable& Valuable::operator /=(const Valuable& v)
    {
        if(exp) {
            Valuable& o = exp->operator/=(v);
            if (o.exp) {
                exp = o.exp;
            }
            return *this;
        }
            IMPLEMENT
    }

    Valuable& Valuable::operator %=(const Valuable& v)
    {
        if(exp) {
            Valuable& o = exp->operator%=(v);
            if (o.exp) {
                exp = o.exp;
            }
            return *this;
        }
        else // a - (n * int(a/n))
            IMPLEMENT // https://math.stackexchange.com/a/2027475/118612
    }

    Valuable& Valuable::operator--()
    {
        if(exp) {
            Valuable& o = exp->operator--();
            if (o.exp) {
                exp = o.exp;
            }
            return *this;
        }
        else
            IMPLEMENT
    }
    
    Valuable& Valuable::operator++()
    {
        if(exp) {
            Valuable& o = exp->operator++();
            if (o.exp) {
                exp = o.exp;
            }
            return *this;
        }
        else
            IMPLEMENT
    }
    
    Valuable& Valuable::operator^=(const Valuable& v)
    {
        if(exp) {
            Valuable& o = exp->operator^=(v);
            if (o.exp) {
                exp = o.exp;
            }
            return *this;
        }
        else
            IMPLEMENT
    }
    
    Valuable& Valuable::d(const Variable& x)
    {
        if(exp) {
            Valuable& o = exp->d(x);
            if (o.exp) {
                exp = o.exp;
            }
            return *this;
        }
        else
            IMPLEMENT
    }
    
    Valuable Valuable::I(const Variable& x, const Variable& C) const
    {
        if(exp) {
            return exp->I(x,C);
        }
        else
            IMPLEMENT
    }

    Valuable& Valuable::i(const Variable& x, const Variable& C)
    {
        if(exp) {
            Valuable& o = exp->i(x,C);
            if (o.exp) {
                exp = o.exp;
            }
            return *this;
        }
        else
            IMPLEMENT
    }

    void Valuable::solve(const Variable& va, solutions_t& solutions) const
    {
        if(exp) {
            exp->solve(va, solutions);
        }
        else
            IMPLEMENT
    }

    Valuable Valuable::operator()(const Variable& va) const
    {
        if(exp) {
            return exp->operator()(va);
        }
        else
            IMPLEMENT
    }
    
    Valuable Valuable::operator()(const Variable& v, const Valuable& augmentation) const
    {
        if(exp) {
            return exp->operator()(v, augmentation);
        }
        else
            IMPLEMENT
    }
    
    bool Valuable::IsUnivariate() const
    {
        std::set<Variable> vars;
        CollectVa(vars);
        return (vars.size() == 1);
    }

    Valuable::solutions_t Valuable::Solutions() const
    {
        std::set<Variable> vars;
        CollectVa(vars);
        if (vars.size() == 1) {
            return Solutions(*vars.begin());
        }
        else
            IMPLEMENT
    }
    
    Valuable::solutions_t Valuable::Solutions(const Variable& v) const
    {
        solutions_t solutions;
        if(this->view == View::Solving)
            solve(v,solutions);
        else {
            auto c = *this;
            c.SetView(View::Solving);
            c.optimize();
            c.solve(v, solutions);
        }
        return solutions;
    }

    Valuable::solutions_t Valuable::IntSolutions() const
    {
        std::set<Variable> vars;
        CollectVa(vars);
        if (vars.size() == 1) {
            return IntSolutions(*vars.begin());
        }
        else
            IMPLEMENT
    }

    Valuable::solutions_t Valuable::IntSolutions(const Variable& v) const
    {
        solutions_t solutions;
        if(this->view == View::Solving)
            solve(v,solutions);
        else {
            auto c = *this;
            c.SetView(View::Solving);
            c.optimize();
            c.solve(v, solutions);
        }
        for(auto it = solutions.begin(); it != solutions.end(); ++it){
            while (it!=solutions.end() && !it->IsInt()) {
                it = solutions.erase(it);
            }
            if(it==solutions.end())
                break;
        }
        return solutions;
    }

    Valuable::solutions_t Valuable::GetIntegerSolution() const
    {
        std::set<Variable> vars;
        CollectVa(vars);
        if (vars.size() == 1) {
            return GetIntegerSolution(*vars.begin());
        }
        else
            IMPLEMENT
    }
    
    Valuable::solutions_t Valuable::GetIntegerSolution(const Variable& va) const
    {
        if(exp) {
            return exp->GetIntegerSolution(va);
        }
        else
            IMPLEMENT
    }
    
    bool Valuable::Test(const Variable& va, const Valuable& v) const
    {
        auto t = *this;
        t.Eval(va, v);
        t.optimize();
//#ifndef NDEBUG // for debug
//        if (t!=0) {
//            std::cout << t << '\n';
//        }
//#endif
        return t==0;
    }
    
    using zone_t = std::pair<Valuable/*from*/,Valuable/*to*/>;
    using zero_zone_t = std::pair<zone_t/*whole*/,std::deque<zone_t>/*subranges*/>;

    zero_zone_t Valuable::get_zeros_zones(const Variable& v, solutions_t& some) const
    {
        auto fm = calcFreeMember().abs();
        zero_zone_t z {{-fm, fm},{}};
        
        auto f = *this;
        f.d(v); f.optimize();
        auto fs = f.Solutions(v);
        
        std::list<decltype(fs)::value_type> ls;
        ls.assign(fs.begin(), fs.end());
        ls.sort();

        auto y = [&](auto x){
            auto _y = *this;
            _y.Eval(v, x);
            _y.optimize();
            return _y;
        };
        auto prev = -fm;
        auto valPrevious = y(prev);
        if (valPrevious == 0) {
            some.insert(prev);
        }
        auto addz = [&](auto& e){
            Valuable& from = prev;
            Valuable atFromVal = valPrevious;
            Valuable& to = e;
            Valuable val = y(e);
            auto isAtFromNeg = atFromVal < 0;
            auto isAtFromPos = atFromVal > 0;
            auto isAtToNeg = val < 0;
            auto isAtToPos = val > 0;
            if( (isAtFromNeg && isAtToPos) || (isAtFromPos && isAtToNeg) ) {
                if (z.second.size() && z.second.back().second == from)
                    z.second.back().second = to;
                else
                    z.second.push_back({from, to});
            }
            else if (val == 0) {
                some.insert(e);
            } else {
                IMPLEMENT
            }
            prev = to;
            valPrevious = val;
        };
        
        for(auto& e : ls)
            addz(e);
        addz(fm);
        
        if(z.second.size()) {
            z.first.first = z.second.begin()->first;
            z.first.second = z.second.rbegin()->second;
        }
        return z;
    }
    
    bool Valuable::operator<(const Valuable& v) const
    {
        if(exp)
            return exp->operator<(v);
        else if (!FindVa()) {
            double _1 = operator double();
            double _2 = static_cast<double>(v);
            if(_1 == _2) {
                IMPLEMENT
            }
            return _1 < _2;
        }
        else
            IMPLEMENT
    }

    bool Valuable::operator==(const Valuable& v) const
    {
        if(exp)
            return Hash()==v.Hash() && exp->operator==(v);
        else
            IMPLEMENT
    }

    bool Valuable::IsInt() const { return exp && exp->IsInt(); }
    bool Valuable::IsFraction() const { return exp && exp->IsFraction(); }
    bool Valuable::IsSimpleFraction() const { return exp && exp->IsSimpleFraction(); }
    bool Valuable::IsSimple() const { return IsInt() || IsSimpleFraction(); }
    bool Valuable::IsFormula() const { return exp && exp->IsFormula(); }
    bool Valuable::IsExponentiation() const { return exp && exp->IsExponentiation(); }
    bool Valuable::IsVa() const { return exp && exp->IsVa(); }
    bool Valuable::IsVaExp() const { return exp && exp->IsVaExp(); }
    bool Valuable::IsProduct() const { return exp && exp->IsProduct(); }
    bool Valuable::IsSum() const { return exp && exp->IsSum(); }
    bool Valuable::IsInfinity() const { return exp && exp->IsInfinity(); }
    bool Valuable::IsMInfinity() const { return exp && exp->IsMInfinity(); }
    bool Valuable::Is_e() const { return exp && exp->Is_e(); }
    bool Valuable::Is_i() const { return exp && exp->Is_i(); }
    bool Valuable::Is_pi() const { return exp && exp->Is_pi(); }

    Valuable::YesNoMaybe Valuable::IsMultival() const {
        if(exp)
            return exp->IsMultival();
        else
            IMPLEMENT
    }
    
    void Valuable::Values(const std::function<bool(const Valuable&)>& f) const {
        if(exp)
            exp->Values(f);
        else
            IMPLEMENT
    }

    Valuable::YesNoMaybe Valuable::IsEven() const {
        if(exp)
            return exp->IsEven();
        else
            IMPLEMENT
    }

    bool Valuable::is(const std::type_index&) const
    {
        IMPLEMENT//ed in contract
    }

    std::ostream& Valuable::print(std::ostream& out) const
    {
        if(exp)
            return exp->print(out);
        else
            IMPLEMENT
    }
 
    std::ostream& Valuable::code(std::ostream& out) const
    {
        return exp ? exp->code(out) : print(out);
    }

    std::ostream& operator<<(std::ostream& out, const Valuable& obj)
    {
        return obj.print(out);
    }
    
    std::ostream& operator<<(std::ostream& out, const Valuable::solutions_t& obj)
    {
        for(auto& s : obj){
            s.print(out);
            out << ' ';
        }
        return out;
    }

    Valuable Valuable::calcFreeMember() const
    {
        if(exp)
            return exp->calcFreeMember();
        else
            IMPLEMENT
    }

    Valuable Valuable::abs() const
    {
        if(exp)
            return exp->abs();
        else
            IMPLEMENT
    }

    Valuable::View Valuable::GetView() const
    {
        return exp ? exp->GetView() : view;
    }
    
    void Valuable::SetView(View v)
    {
        if(exp)
            exp->SetView(v);
        else {
            optimized = optimized && view == v;
            view = v;
        }
    }
    
    void Valuable::optimize()
    {
        if(optimizations && exp) {
            while (exp->exp) {
                exp = exp->exp;
            }
            exp->optimize();
            while (exp->exp) {
                exp = exp->exp;
            }
            return;
        }
        else if(!exp)
            IMPLEMENT
    }

	Valuable Valuable::Sqrt() const {
        if(exp)
            return exp->Sqrt();
        else
            IMPLEMENT
    }

    Valuable& Valuable::sq() {
        if (exp)
            return exp->sq();
        else
            IMPLEMENT
    }

    Valuable Valuable::Sq() const
    {
        auto t = *this;
        return t.sq();
    }

    const Variable* Valuable::FindVa() const
    {
        if (exp) {
            return exp->FindVa();
        }
        IMPLEMENT
    }
    
    bool Valuable::HasVa(const Variable& x) const
    {
        if (exp) {
            return exp->HasVa(x);
        }
        IMPLEMENT
    }
    
    void Valuable::CollectVa(std::set<Variable>& s) const
    {
        if (exp)
            exp->CollectVa(s);
        else
            IMPLEMENT
    }

    void Valuable::CollectVaNames(std::map<std::string, Variable>& s) const
    {
        if (exp)
            exp->CollectVaNames(s);
        else
            IMPLEMENT
    }

    Valuable::va_names_t Valuable::VaNames() const {
        va_names_t vaNames;
        CollectVaNames(vaNames);
        return vaNames;
    }

    std::shared_ptr<VarHost> Valuable::getVaHost() const {
        auto aVa = FindVa();
        std::shared_ptr<VarHost> host;
#ifndef NDEBUG
        // check that all of Vars() has common va host.
#endif
        if (aVa)
            host = aVa->getVaHost();
        else {
            static Variable AVa;
            host = AVa.getVaHost();
        }
        return host;
    }

    Valuable::var_set_t Valuable::Vars() const
    {
        var_set_t vars;
        CollectVa(vars);
        return vars;
    }
    
    bool Valuable::eval(const std::map<Variable, Valuable>& with){
        if (exp)
            return exp->eval(with);
        else
            IMPLEMENT
    }

    void Valuable::Eval(const Variable& va, const Valuable& v)
    {
        if (exp) {
            if (v.HasVa(va)) {
                Variable t;
                Eval(va, t);
                Eval(t, v);
            } else
                exp->Eval(va, v);
            while (exp->exp) {
                exp = exp->exp;
            }
            return;
        }
        IMPLEMENT
    }
    
    bool Valuable::OfSameType(const Valuable& v) const
    {
        const Valuable& v1 = get();
        const Valuable& v2 = v.get();
        return typeid(v1) == typeid(v2);
    }
    
    bool Valuable::Same(const Valuable& v) const
    {
        return Hash()==v.Hash() && OfSameType(v) && operator==(v);
    }
 
    bool Valuable::HasSameVars(const Valuable& v) const
    {
        std::set<Variable> thisVa, vVa;
        this->CollectVa(thisVa);
        v.CollectVa(vVa);
        return thisVa == vVa;
    }
    
    bool Valuable::IsMonic() const
    {
        std::set<Variable> vars;
        CollectVa(vars);
        return vars.size() == 1;
    }
    
    max_exp_t Valuable::getMaxVaExp() const
    {
        return exp ? exp->getMaxVaExp() : maxVaExp;
    }
    
    bool Valuable::IsComesBefore(const Valuable& v) const
    {
        if (exp)
            return exp->IsComesBefore(v);
        else
            IMPLEMENT
    }
    
    const Valuable::vars_cont_t& Valuable::getCommonVars() const
    {
        if (exp)
            return exp->getCommonVars();
        else
            IMPLEMENT
    }
    
    Valuable Valuable::InCommonWith(const Valuable& v) const
    {
        if (exp)
            return exp->InCommonWith(v);
        else
            IMPLEMENT
    }

    Valuable Valuable::varless() const
    {
        auto _ = *this / getVaVal();
        if (_.FindVa()) {
            if (_.IsProduct()) {
                Product p;
                for(auto&& m: Product::cast(_)->GetConstCont()){
                    if (!m.FindVa()) {
                        p.Add(std::move(const_cast<Valuable&&>(m)));
                    }
                }
                _ = p;
            } else
                IMPLEMENT
        }
        return _;
    }
    
    Valuable Valuable::VaVal(const vars_cont_t& v)
    {
        Valuable p(1);
        for(auto& kv : v)
        {
            p *= kv.first ^ kv.second;
        }
        p.optimize();
        return p;
    }
    
    Valuable Valuable::getVaVal() const
    {
        return VaVal(getCommonVars());
    }
    
    const Valuable::vars_cont_t& Valuable::emptyCommonVars()
    {
        static const Valuable::vars_cont_t _;
        return _;
    }

    Valuable::operator bool() const
    {
        return *this != 0_v;
    }
    
    Valuable::operator int() const
    {
        if (exp)
            return exp->operator int();
        else
            IMPLEMENT
    }
  
    Valuable::operator a_int() const
    {
        if (exp)
            return exp->operator a_int();
        else
            IMPLEMENT
    }
    
    a_int& Valuable::a()
    {
        if (exp)
            return exp->a();
        else
            IMPLEMENT
    }

    const a_int& Valuable::ca() const
    {
        if (exp)
            return exp->ca();
        else
            IMPLEMENT
    }

    Valuable::operator uint64_t() const
    {
        if (exp)
            return exp->operator uint64_t();
        else
            IMPLEMENT
    }
    
    Valuable::operator double() const
    {
        if (exp)
            return exp->operator double();
        else
            IMPLEMENT
    }

    Valuable::operator long double() const
    {
        if (exp)
            return exp->operator double();
        else
            IMPLEMENT
    }

    Valuable::operator uint32_t() const
    {
        if (exp)
            return exp->operator uint32_t();
        else
            IMPLEMENT
    }
    
    Valuable::operator unsigned char() const
    {
        if (exp)
            return exp->operator unsigned char();
        else
            IMPLEMENT
    }
    
    Valuable Valuable::Equals(const Valuable& v) const {
        return *this - v;
    }
    
    Valuable Valuable::NotEquals(const Valuable& v) const {
//        return IfEq(v,1,0);
        return Equals(v) ^ -1;
    }

    Valuable Valuable::Abet(const Variable& x, std::initializer_list<Valuable> l) const
    {
        Product a;
        for(auto& item : l)
            a.Add(item.Equals(x));
        return Valuable(a.Move());
    }

//    Valuable Valuable::NE(const Valuable& to, const Valuable& abet) const
//    {
//        return abet / (*this-to);
//    }
//
//    Valuable Valuable::NE(const Variable& x, const Valuable& to, std::initializer_list<Valuable> abet) const
//    {
//        return Abet(x, abet)/(*this-to);
//    }

    Valuable Valuable::LogicAnd(const Valuable& v) const
    {
        return Sq()+v.Sq();
    }

    Valuable& Valuable::logic_and(const Valuable& v)
    {
        return sq()+=v.Sq();
    }

    Valuable& Valuable::logic_or(const Valuable& v)
    {
        return operator*=(v);
    }

    Valuable Valuable::LogicOr(const Valuable& v) const
    {
        return *this * v;
    }

    Valuable Valuable::Ifz(const Valuable& Then, const Valuable& Else) const
    {
        auto thisAndThen = LogicAnd(Then);
        return thisAndThen.LogicOr(thisAndThen.LogicOr(Else).LogicAnd(Else));
    }

    Valuable Valuable::IfEq(const Valuable& e, const Valuable& Then, const Valuable& Else) const
    {
        auto equalsAndThen = Equals(e).LogicAnd(Then);
        return equalsAndThen.LogicOr(equalsAndThen.LogicOr(Else).LogicAnd(Else));
    }

    Valuable Valuable::For(const Valuable& initialValue, const Valuable& lambda) const
    {
        IMPLEMENT
    }

    Valuable Valuable::bit(const Valuable& n) const
    {
        if (exp)
            return exp->bit(n);
        else if (n > 0) {
            return Shr().bit(n-1);
        } else if (n == 0)
            // (this & 1) == (this % 2) == (1+((-1)^(this+1)))/2
            // this=1: (1+(-1^2))/2 = 1
            // this=2: (1+(-1^3))/2 = 0
            // this=3: (1+(-1^4))/2 = 1
            // ...
            // this=-1: (1+(-1^0))/2=1
            // this=-2: (1+(-1^-1))/2=0
            // this=-3: (1+(-1^-2))/2=1
            // ...
            return (1_v+((-1_v)^(*this+1)))/2;
//            return *this % 2;
        else
            IMPLEMENT;
    }
    
    Valuable Valuable::bits(int n, int l) const
    {
        if(n<0)
            IMPLEMENT
        return Shr(n).And(l, -1);
    }
    
    Valuable Valuable::And(const Valuable& n, const Valuable& v) const
    {
        auto s = 0_v;
        auto ow = optimizations;
        optimizations = {};
        for(auto i = n; i--;)
        {
            s *= 2;
            s += bit(i)*v.bit(i);
        }
        optimizations = ow;
        if (ow && bit_operation_optimizations) {
            s.optimize();
        }
        return s;
    }
    
    Valuable Valuable::Or(const Valuable& n, const Valuable& v) const
    {
        auto s = 0_v;
        auto ow = optimizations;
        optimizations = {};
        for(auto i = n; i--;)
        {
            s *= 2;
            auto _1 = bit(i);
            auto _2 = v.bit(i);
            s += _1+_2-_1*_2;
        }
        optimizations = ow;
        if (ow && bit_operation_optimizations) {
            s.optimize();
        }
        return s;
    }
    
    Valuable Valuable::Xor(const Valuable& n, const Valuable& v) const
    {
        auto s = 0_v;
        auto ow = optimizations;
        optimizations = {};
        for(auto i = n; i--;)
        {
            s *= 2;
            auto _1 = bit(i);
            auto _2 = v.bit(i);
            s += (_1+_2).bit(0);
        }
        optimizations = ow;
        if (ow && bit_operation_optimizations) {
            s.optimize();
        }
        return s;
    }

    Valuable Valuable::Not(const Valuable& n) const
    {
        auto s = 0_v;
        auto ow = optimizations;
        optimizations = {};
        for(auto i = n; i--;)
        {
            s *= 2;
            auto _1 = 1-bit(i);
            s += (_1).bit(0);
        }
        optimizations = ow;
        if (ow && bit_operation_optimizations) {
            s.optimize();
        }
        return s;
    }
    
    Valuable& Valuable::shl(const Valuable& n)
    {
        return *this *= 2_v^n;
    }
    
    Valuable& Valuable::shr(const Valuable& n)
    {
        if(!n.IsInt()){
            IMPLEMENT
        }
        
        if (n>1)
            return shr(n-1).shr();
        else if (n!=0)
            return shr();
        else
            return *this;
    }
    
    Valuable& Valuable::shr()
    {
        return operator+=(-bit(0)).operator/=(2);
    }

    Valuable Valuable::Shl(const Valuable& n) const
    {
        return *this * (2_v^n);
    }

    Valuable Valuable::Shr(const Valuable& n) const
    {
        auto v = Valuable(get());
        return v.shr(n);
    }
    
    Valuable Valuable::Shr() const
    {
        return (*this-bit(0))/2;
    }
    
    Valuable Valuable::Cyclic(const Valuable& total, const Valuable& shiftLeft) const
    {
        auto s = 0_v;
        auto ow = optimizations;
        optimizations = {};
        for(auto i = total; i--;)
        {
            auto shi = i + shiftLeft;
            if(shi >= total)
                shi -= total;
            else if (shi < 0)
                shi += total;
            auto _1 = bit(i);
            s += bit(i) * (2^(shi));
        }
        optimizations = ow;
        if (ow && bit_operation_optimizations) {
            s.optimize();
        }
        return s;
    }
    
    auto sh_const =   "0x428a2f9871374491b5c0fbcfe9b5dba5" /*  0 */
                        "3956c25b59f111f1923f82a4ab1c5ed5"
                        "d807aa9812835b01243185be550c7dc3" /*  8 */
                        "72be5d7480deb1fe9bdc06a7c19bf174"
                        "e49b69c1efbe47860fc19dc6240ca1cc" /* 16 */
                        "2de92c6f4a7484aa5cb0a9dc76f988da"
                        "983e5152a831c66db00327c8bf597fc7" /* 24 */
                        "c6e00bf3d5a7914706ca635114292967"
                        "27b70a852e1b21384d2c6dfc53380d13" /* 32 */
                        "650a7354766a0abb81c2c92e92722c85"
                        "a2bfe8a1a81a664bc24b8b70c76c51a3" /* 40 */
                        "d192e819d6990624f40e3585106aa070"
                        "19a4c1161e376c082748774c34b0bcb5" /* 48 */
                        "391c0cb34ed8aa4a5b9cca4f682e6ff3"
                        "748f82ee78a5636f84c878148cc70208" /* 56 */
                        "90befffaa4506cebbef9a3f7c67178f2"_v;
    
    const Valuable SHA256_K[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
        0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
        0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
        0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
        0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
        0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
        0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
        0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };
    Valuable Valuable::sh(const Valuable& n) const
    {
//        Valuable state = "0x6a09e667bb67ae853c6ef372a54ff53a510e527f9b05688c1f83d9ab5be0cd19"_v;
        Valuable W[16];
        Valuable T[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
        auto state = T;
        // lambdas
//#define S0(x) (rotrFixed(x,2)^rotrFixed(x,13)^rotrFixed(x,22))
        auto S0 = [](const Valuable& v){
            return v.Cyclic(32, -2).Xor(32, v.Cyclic(32, -13)).Xor(32, v.Cyclic(32, -22));
        };
//#define S1(x) (rotrFixed(x,6)^rotrFixed(x,11)^rotrFixed(x,25))
        auto S1 = [](const Valuable& v){
            return v.Cyclic(32, -6).Xor(32, v.Cyclic(32, -11)).Xor(32, v.Cyclic(32, -25));
        };
//#define s0(x) (rotrFixed(x,7)^rotrFixed(x,18)^(x>>3))
        auto s0 = [](const Valuable& v){
            return v.Cyclic(32, -7).Xor(32, v.Cyclic(32, -18)).Xor(32, v.Shr(3));
        };
//#define s1(x) (rotrFixed(x,17)^rotrFixed(x,19)^(x>>10))
        auto s1 = [](const Valuable& v){
            return v.Cyclic(32, -17).Xor(32, v.Cyclic(32, -19)).Xor(32, v.Shr(10));
        };
//#define blk0(i) (W[i] = data[i])
        auto blk0 = [&](int i) {
            return (W[i] = bits(32*i,32));
        };
//#define blk2(i) (W[i&15]+=s1(W[(i-2)&15])+W[(i-7)&15]+s0(W[(i-15)&15]))
        auto blk2 = [&](int i) {
            return (W[i&15]+=s1(W[(i-2)&15])+W[(i-7)&15]+s0(W[(i-15)&15])).And(32, -1);
        };
//#define Ch(x,y,z) (z^(x&(y^z)))
        auto Ch = [](const Valuable& _1, const Valuable& _2, const Valuable& _3) {
            return _3.Xor(32, _2).And(32,_1).Xor(32, _3);
        };
//#define Maj(x,y,z) (y^((x^y)&(y^z)))
        auto Maj = [](const Valuable& _1, const Valuable& _2, const Valuable& _3) {
            return _3.Xor(32, _2).And(32,_1.Xor(32, _2)).Xor(32, _2);
        };
        
#define a(i) T[(0-i)&7]
#define b(i) T[(1-i)&7]
#define c(i) T[(2-i)&7]
#define d(i) T[(3-i)&7]
#define e(i) T[(4-i)&7]
#define f(i) T[(5-i)&7]
#define g(i) T[(6-i)&7]
#define h(i) T[(7-i)&7]
        
#define R(i) h(i)+=S1(e(i))+Ch(e(i),f(i),g(i))+SHA256_K[i+j]+(j?blk2(i):blk0(i));\
d(i)+=h(i);h(i)+=S0(a(i))+Maj(a(i),b(i),c(i))
//        auto _ = [&](auto i) {
//            h(i) +=
//        };
        /* Copy context->state[] to working vars */
//        memcpy(T, state, sizeof(T));
        /* 64 operations, partially loop unrolled */
        for (unsigned int j=0; j<64; j+=16)
        {
            R( 0); R( 1); R( 2); R( 3);
            R( 4); R( 5); R( 6); R( 7);
            R( 8); R( 9); R(10); R(11);
            R(12); R(13); R(14); R(15);
        }
        /* Add the working vars back into context.state[] */
        state[0] += a(0);
        state[1] += b(0);
        state[2] += c(0);
        state[3] += d(0);
        state[4] += e(0);
        state[5] += f(0);
        state[6] += g(0);
        state[7] += h(0);
        
        return ((((((state[0].And(32,-1).Shl(32) + state[1].And(32,-1)).Shl(32) + state[2].And(32,-1)).Shl(32) + state[3].And(32,-1)).Shl(32) + state[4].And(32,-1)).Shl(32) + state[5].And(32,-1)).Shl(32) + state[6].And(32,-1)).Shl(32) + state[7].And(32,-1);
        
        
//        Пояснения:
//        Все переменные беззнаковые, имеют размер 32 бита и при вычислениях суммируются по модулю 232
//        message — исходное двоичное сообщение
//        m — преобразованное сообщение
//
//        Инициализация переменных
//        (первые 32 бита дробных частей квадратных корней первых восьми простых чисел [от 2 до 19]):
//        auto h0 = "0x6A09E667"_v;
//        auto h1 = "0xBB67AE85"_v;
//        auto h2 = "0x3C6EF372"_v;
//        auto h3 = "0xA54FF53A"_v;
//        auto h4 = "0x510E527F"_v;
//        auto h5 = "0x9B05688C"_v;
//        auto h6 = "0x1F83D9AB"_v;
//        auto h7 = "0x5BE0CD19"_v;
//
//        Таблица констант
//        (первые 32 бита дробных частей кубических корней первых 64 простых чисел [от 2 до 311]):
//        auto k[64] = {
//        "0x428A2F98"_v, "0x71374491"_v, "0xB5C0FBCF"_v, "0xE9B5DBA5"_v, "0x3956C25B"_v, "0x59F111F1"_v, "0x923F82A4"_v, "0xAB1C5ED5"_v,
//        "0xD807AA98"_v, "0x12835B01"_v, "0x243185BE"_v, "0x550C7DC3"_v, "0x72BE5D74"_v, "0x80DEB1FE"_v, "0x9BDC06A7"_v, "0xC19BF174"_v,
//        "0xE49B69C1"_v, "0xEFBE4786"_v, "0x0FC19DC6"_v, "0x240CA1CC"_v, "0x2DE92C6F"_v, "0x4A7484AA"_v, "0x5CB0A9DC"_v, "0x76F988DA"_v,
//        "0x983E5152"_v, "0xA831C66D"_v, "0xB00327C8"_v, "0xBF597FC7"_v, "0xC6E00BF3"_v, "0xD5A79147"_v, "0x06CA6351"_v, "0x14292967"_v,
//        "0x27B70A85"_v, "0x2E1B2138"_v, "0x4D2C6DFC"_v, "0x53380D13"_v, "0x650A7354"_v, "0x766A0ABB"_v, "0x81C2C92E"_v, "0x92722C85"_v,
//        "0xA2BFE8A1"_v, "0xA81A664B"_v, "0xC24B8B70"_v, "0xC76C51A3"_v, "0xD192E819"_v, "0xD6990624"_v, "0xF40E3585"_v, "0x106AA070"_v,
//        "0x19A4C116"_v, "0x1E376C08"_v, "0x2748774C"_v, "0x34B0BCB5"_v, "0x391C0CB3"_v, "0x4ED8AA4A"_v, "0x5B9CCA4F"_v, "0x682E6FF3"_v,
//        "0x748F82EE"_v, "0x78A5636F"_v, "0x84C87814"_v, "0x8CC70208"_v, "0x90BEFFFA"_v, "0xA4506CEB"_v, "0xBEF9A3F7"_v, "0xC67178F2"_v};
//
        // ǁ - concat
//        Предварительная обработка:
//        auto m = Shl(1) + 1;
//        m = m ǁ [k нулевых бит], где k — наименьшее неотрицательное число, такое что
//        (L + 1 + K) mod 512 = 448, где L — число бит в сообщении (сравнима по модулю 512 c 448)
//        m = m ǁ Длина(message) — длина исходного сообщения в битах в виде 64-битного числа
//        с порядком байтов от старшего к младшему
//
//        Далее сообщение обрабатывается последовательными порциями по 512 бит:
//        разбить сообщение на куски по 512 бит
//        для каждого куска
//        разбить кусок на 16 слов длиной 32 бита (с порядком байтов от старшего к младшему внутри слова): w[0..15]
//
//        Сгенерировать дополнительные 48 слов:
//        для i от 16 до 63
//        s0 = (w[i-15] rotr 7) xor (w[i-15] rotr 18) xor (w[i-15] shr 3)
//        s1 = (w[i-2] rotr 17) xor (w[i-2] rotr 19) xor (w[i-2] shr 10)
//        w[i] = w[i-16] + s0 + w[i-7] + s1
//
//        Инициализация вспомогательных переменных:
//        a = h0
//        b = h1
//        c = h2
//        d = h3
//        e = h4
//        f = h5
//        g = h6
//        h = h7
//
//        Основной цикл:
//        для i от 0 до 63
//        Σ0 = (a rotr 2) xor (a rotr 13) xor (a rotr 22)
//        Ma = (a and b) xor (a and c) xor (b and c)
//        t2 = Σ0 + Ma
//        Σ1 = (e rotr 6) xor (e rotr 11) xor (e rotr 25)
//        Ch = (e and f) xor ((not e) and g)
//        t1 = h + Σ1 + Ch + k[i] + w[i]
//
//        h = g
//        g = f
//        f = e
//        e = d + t1
//        d = c
//        c = b
//        b = a
//        a = t1 + t2
//
//        Добавить полученные значения к ранее вычисленному результату:
//        h0 = h0 + a
//        h1 = h1 + b
//        h2 = h2 + c
//        h3 = h3 + d
//        h4 = h4 + e
//        h5 = h5 + f
//        h6 = h6 + g
//        h7 = h7 + h
//
//        Получить итоговое значение хеша:
//        digest = hash = h0 ǁ h1 ǁ h2 ǁ h3 ǁ h4 ǁ h5 ǁ h6 ǁ h7
    }
    
    size_t Valuable::Hash() const
    {
        return exp
            ? exp->Hash()
            : hash;
    }
    
    std::string Valuable::str() const
    {
        std::stringstream s;
        print(s);
        return s.str();
    }
    
    std::wstring Valuable::save(const std::wstring& f) const
    {
        if (exp) {
            return exp->save(f);
        }
        else
            IMPLEMENT
    }

    size_t hash_value(const Valuable& v)
    {
        return v.Hash();
    }
}}

namespace std
{
    ::omnn::math::Valuable abs(const ::omnn::math::Valuable& v)
    {
        return v.abs();
    }
    ::omnn::math::Valuable sqrt(const ::omnn::math::Valuable& v)
    {
        return v.Sqrt();
    }
}

::omnn::math::Valuable operator"" _v(const char* v, std::size_t)
{
    using namespace ::omnn::math;
    if ((*v >= '0' && *v <= '9') || (*v == '-' && (v[1] >= '0' && v[1] <= '9')))
        return ::omnn::math::Integer(boost::multiprecision::cpp_int(v));
    else {
        static auto h = VarHost::make<std::string>();
        return Valuable(v,h);
    }
}

const ::omnn::math::Variable& operator"" _va(const char* v, std::size_t)
{
    using namespace ::omnn::math;
    static auto h = VarHost::make<std::string>();
    std::string id = v;
    return h->Host(id);
}

//constexpr
boost::multiprecision::cpp_int ull2cppint(unsigned long long v) {
    return v;
}

::omnn::math::Valuable operator"" _v(unsigned long long v)
{
    const auto va = ull2cppint(v);
    return ::omnn::math::Integer(va);
}

//constexpr const ::omnn::math::Valuable& operator"" _const(unsigned long long v)
//{
//    return ::omnn::math::vo<v>();
//}

::omnn::math::Valuable operator"" _v(long double v)
{
    return ::omnn::math::Fraction(boost::multiprecision::cpp_dec_float_100(v));
}
