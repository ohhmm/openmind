//
// Created by Сергей Кривонос on 01.09.17.
//

#include "Valuable.h"
#include "Fraction.h"
#include "Infinity.h"
#include "Integer.h"
#include "Variable.h"
#include <boost/numeric/conversion/converter.hpp>


namespace omnn{
namespace math {

    thread_local bool Valuable::optimizations = true;
    
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

    int Valuable::getAllocSize() const
    {
        return sz;
    }
    
    void Valuable::setAllocSize(int sz)
    {
        this->sz = sz;
    }
    
    Valuable::operator std::type_index() const
    {
        return exp ? static_cast<std::type_index>(*exp) : std::type_index(typeid(*this));
    }
    
    Valuable& Valuable::Become(Valuable&& i)
    {
        auto h = i.Hash();
        auto v = i.view;
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
                constexpr decltype(newSize) BufSz = 512;
                assert(BufSz >= newSize);
                char buf[BufSz];
                i.New(buf, std::move(i));
                Valuable& bufv = *reinterpret_cast<Valuable*>(buf);
                this->~Valuable();
                i.New(this, std::move(bufv));
                setAllocSize(sizeWas);
                if (Hash() != h) {
                    IMPLEMENT
                }
                if (v != view) {
                    IMPLEMENT
                }
                optimize();
            }
            else if(exp && exp->getAllocSize() >= newSize)
            {
                exp->Become(std::move(i));
                if (v != exp->view) {
                    IMPLEMENT
                }
            }
            else
            {
                auto moved = i.Move();
                this->~Valuable();
                new(this) Valuable(moved);
                if (Hash() != h) {
                    IMPLEMENT
                }
                if (v != view) {
                    IMPLEMENT
                }
                optimize();
            }
        }
        
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

    Valuable::Valuable(double d) : exp(new Fraction(d)) {}
    Valuable::Valuable(int i) : exp(new Integer(i)) {}
    Valuable::Valuable(const a_int& i) : exp(new Integer(i)) {}
    Valuable::Valuable(const long i) : exp(new Integer(i)) {}
    Valuable::Valuable(unsigned long i) : exp(new Integer(i)) {}
    Valuable::Valuable(unsigned long long i) : exp(new Integer(i)) {}

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
        else
            IMPLEMENT
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
    
    void Valuable::solve(const Variable& va, solutions_t& solutions) const
    {
        if(exp) {
            exp->solve(va, solutions);
        }
        else
            IMPLEMENT
    }

    Valuable::solutions_t Valuable::operator()(const Variable& va) const
    {
        if(exp) {
            return exp->operator()(va);
        }
        else
            IMPLEMENT
    }
    
    Valuable::solutions_t Valuable::operator()(const Variable& v, const Valuable& augmentation) const
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

    Valuable::solutions_t Valuable::solutions() const
    {
        std::set<Variable> vars;
        CollectVa(vars);
        if (vars.size() == 1) {
            return solutions(*vars.begin());
        }
        else
            IMPLEMENT
    }
    
    Valuable::solutions_t Valuable::solutions(const Variable& v) const
    {
        solutions_t solutions;
        solve(v, solutions);
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
    
    using extrenums_t = std::vector<std::pair<Valuable/*value*/,Valuable/*direction*/> >;
    extrenums_t Valuable::extrenums() const
    {
        extrenums_t e;
        std::set<Variable> vars;
        CollectVa(vars);
        if (vars.size() == 1) {
            extrenums(*vars.begin(), e);
        }
        else
            IMPLEMENT;
        return e;
    }

    extrenums_t Valuable::extrenums(const Variable& v) const
    {
        extrenums_t e;
        return extrenums(v, e);
    }

    extrenums_t& Valuable::extrenums(const Variable& v, extrenums_t& e) const
    {
        auto f = *this;
        f.d(v);
        f.optimize();
        auto dd = f; dd.d(v);
        dd.optimize();
        for(auto& s : f.solutions(v))
        {
            auto ddc = dd;
            ddc.Eval(v, s);
            ddc.optimize();
            e.push_back({s, ddc});
        }
        return e;
    }
    
    using zone_t = std::pair<Valuable/*from*/,Valuable/*to*/>;
    using zero_zone_t = std::pair<zone_t/*whole*/,std::deque<zone_t>/*subranges*/>;

    zero_zone_t Valuable::get_zeros_zones(const Variable& v) const
    {
        return get_zeros_zones(v, extrenums(v));
    }

    zero_zone_t Valuable::get_zeros_zones(const Variable& v, const extrenums_t& extrs) const
    {
        zero_zone_t z {{Infinity(), MInfinity()},{}};
        if (!extrs.size()) {
            return z;
        }
        
        auto y = [&](auto x){
            auto _y = *this;
            _y.Eval(v, x);
            return _y;
        };
        
        auto dv = *this; dv.d(v);
        dv.optimize();
        auto dxval = [&](auto x){
            auto dy = dv;
            dy.Eval(v, x);
            return dy;
        };
        
        auto inf = Infinity();
        auto valPrevious = y(MInfinity());
        auto& b = extrs.begin()->first;
//        if ((y(b-inf) < b && b > 0) || (y(b-inf) > b && b < 0)){
//            z.second.push_back({MInfinity(), b});
//        }
        for(auto& e : extrs)
        {
            if(valPrevious > 0 != e.first > 0) {
                z.second.push_back({valPrevious, e.first});
                if (z.first.first > valPrevious) {
                    z.first.first = valPrevious;
                }
                if (z.first.second < e.first) {
                    z.first.second = e.first;
                }
            }
            else if(valPrevious == 0 || e.first == 0) {
                IMPLEMENT
            }
            valPrevious = e.first;
        }
        
        return z;
    }
    
    bool Valuable::operator<(const Valuable& v) const
    {
        if(exp)
            return exp->operator<(v);
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
    bool Valuable::IsFormula() const { return exp && exp->IsFormula(); }
    bool Valuable::IsExponentiation() const { return exp && exp->IsExponentiation(); }
    bool Valuable::IsVa() const { return exp && exp->IsVa(); }
    bool Valuable::IsProduct() const { return exp && exp->IsProduct(); }
    bool Valuable::IsSum() const { return exp && exp->IsSum(); }
    bool Valuable::IsInfinity() const { return exp && exp->IsInfinity(); }
    bool Valuable::IsMInfinity() const { return exp && exp->IsMInfinity(); }

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

    void Valuable::SetView(View v)
    {
        if(exp)
            exp->view = v;
        else
            IMPLEMENT
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

	Valuable Valuable::sqrt() const {
        if(exp)
            return exp->sqrt();
        else
            IMPLEMENT
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

    Valuable::var_set_t Valuable::Vars() const
    {
        var_set_t vars;
        CollectVa(vars);
        return vars;
    }
    
    void Valuable::Eval(const Variable& va, const Valuable& v)
    {
        if (exp) {
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
        const Valuable& v1 = *cast<Valuable>(*this);
        const Valuable& v2 = *cast<Valuable>(v);
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
    
    a_int Valuable::getMaxVaExp() const
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
    
    Valuable Valuable::varless() const
    {
        return *this / getVaVal();
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
    
    Valuable::operator size_t() const
    {
        if (exp)
            return exp->operator size_t();
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

    Valuable::operator unsigned() const
    {
        if (exp)
            return exp->operator unsigned();
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
    
//    Valuable Valuable::ifz(const Valuable& z, const Valuable& Then, const Valuable& Else) const
//    {
//        return ((z)^2+(Then)^2)(((((z)^2)+((Then)^2))(Else))^2+(Else)^2);
//    }
    
    Valuable Valuable::bit(const Valuable& n) const
    {
        if (exp)
            return exp->bit(n);
        else if (n > 0) {
            return shr().bit(n-1);
        } else if (n == 0)
            // (this & 1) == (this % 2) == (1+((-1)^(this+1)))/2
            return (1_v+((-1_v)^(*this+1)))/2;
        else
            IMPLEMENT;
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
        if (ow) {
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
        if (ow) {
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
        if (ow) {
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
        if (ow) {
            s.optimize();
        }
        return s;
    }
    
    Valuable& Valuable::shl(const Valuable& n)
    {
        return *this *= 2_v^n;
    }

    Valuable Valuable::Shl(const Valuable& n) const
    {
        return *this * 2_v^n;
    }

    Valuable Valuable::shr() const
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
        if (ow) {
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
    
    Valuable Valuable::sh(const Valuable& n) const
    {
        IMPLEMENT
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
        return v.sqrt();
    }
}

::omnn::math::Valuable operator"" _v(const char* v, std::size_t)
{
    return ::omnn::math::Integer(boost::multiprecision::cpp_int(v));
}

::omnn::math::Valuable operator"" _v(unsigned long long v)
{
    return ::omnn::math::Integer(boost::multiprecision::cpp_int(v));
}

::omnn::math::Valuable operator"" _v(long double v)
{
    return ::omnn::math::Fraction(v);
}
