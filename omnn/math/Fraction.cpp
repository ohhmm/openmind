//
// Created by Сергей Кривонос on 01.09.17.
//
#include "Fraction.h"
#include "Integer.h"
#include "Sum.h"
#include "Product.h"
#include "PrincipalSurd.h"
#include "i.h"

#include <utility>

#include <boost/lexical_cast.hpp>


namespace omnn{
namespace math {
	Fraction::Fraction(const Integer& n)
		: base(n, 1)
	{
	}

    Fraction::Fraction(const boost::multiprecision::cpp_dec_float_100& f)
    {
        auto s = boost::lexical_cast<std::string>(f);
        if (s=="nan") {
            throw s;
        }
        auto doti = s.find_first_of('.');
        if (doti == -1)
        {
            numerator() = Integer(boost::multiprecision::cpp_int(f));
            denominator() = 1;
        }
        else
        {
            auto afterDotI = doti + 1;
            auto fsz = s.length() - afterDotI;
            denominator() = 10_v ^ fsz;
            numerator() = Integer(boost::multiprecision::cpp_int(s.c_str()+afterDotI));
            if (s[0]=='-') {
                numerator() = -numerator();
            }
            s.erase(doti);
            numerator() += Integer(boost::multiprecision::cpp_int(s)) * denominator();
        }
    }
    
	Valuable Fraction::operator -() const
    {
        return Fraction(-getNumerator(), getDenominator());
    }

    bool Fraction::operator==(const Fraction& f) const {
        return Hash() == f.Hash()
            && getNumerator().Hash() == f.getNumerator().Hash()
            && getDenominator().Hash() == f.getDenominator().Hash()
            && getNumerator() == f.getNumerator()
            && getDenominator() == f.getDenominator()
            ;
    }

    bool Fraction::operator ==(const Valuable& v) const
    {
        auto eq = v.IsFraction() && Hash()==v.Hash();
        if(eq){
            eq = operator==(v.as<Fraction>());
        } else if (v.IsExponentiation() || v.IsSum() || v.IsProduct()) {
            eq = v.operator==(*this);
        } else if (v.IsInt()) {
			eq = getNumerator() == v && getDenominator() == constants::one;
        }
        return eq;
    }

    void Fraction::solve(const Variable& va, solutions_t& s) const {
        numerator().solve(va, s);
        solutions_t exclude;
        denominator().solve(va, exclude);
        for(auto& so:exclude)
            s.erase(so);
    }
    
    Valuable::solutions_t Fraction::Distinct() const {
        Valuable::solutions_t branches;
        for (auto&& n : numerator().Distinct()) {
            for (auto&& d : denominator().Distinct()) {
                branches.emplace(n / d);
            }
        }
        return branches;
    }

    void Fraction::optimize()
    {
        if (!optimizations && !IsSimple()) {
            hash = numerator().Hash() ^ denominator().Hash();
            return;
        }
        if (optimized) {
#if !defined(NDEBUG) && !defined(NOOMDEBUG)
            auto h = numerator().Hash() ^ denominator().Hash();
            if (h != hash){
                LOG_AND_IMPLEMENT("Fix hash updating for " << *this);
            }
#endif
            return;
        }

        OptimizationLoopDetect<Fraction> antilooper(*this);
        if (antilooper.isLoopDetected()) {
#if !defined(NDEBUG) && !defined(NOOMDEBUG)
            LOG_AND_IMPLEMENT("Loop of optimizating detected in " << *this);
#endif
            return;
        }

        bool reoptimize_the_fraction;
        do {
            reoptimize_the_fraction = false;
            numerator().optimize();
            denominator().optimize();

            if (!FindVa() // TODO: variables reduction from denominator should log non-zero expression for post-check
                && numerator() == denominator()
                && !denominator().IsZero()
                && IsMultival() == YesNoMaybe::No
                )
            {
				Become(1);
				break;
			}

            while (denominator().IsFraction()) {
                auto& fdn = denominator().as<Fraction>();
                numerator() *= fdn.denominator();
                denominator() = std::move(fdn.numerator());
            }

            //        if (numerator().IsSum()) {
            //            Become(numerator()/denominator());
            //            return;
            //        }

            if (denominator().IsInt()) {
                if (denominator().ca() == 1) {
                    Become(std::move(numerator()));
                    break;
                } else if (denominator() < 0) {
                    numerator() = -numerator();
                    denominator() = -denominator();
                    if (denominator().ca() == 1) {
                        Become(std::move(numerator()));
                        break;
                    }
                }
            }

            if (denominator() == numerator() && IsMultival() == YesNoMaybe::No) {
                // TODO : mark var constraints deduced from denominator!=0
                Become(1);
                break;
            }

            if (denominator().Is_i()) {
                numerator() *= constants::minus_1;
                numerator() *= denominator();
                Become(std::move(numerator()));
                break;
            }

            if (numerator().IsExponentiation()) {
                auto& e = numerator().as<Exponentiation>();
                auto& exp = e.getExponentiation();
                if (exp.IsInt() && exp < 0) {
                    denominator() *= e.getBase() ^ (-exp);
                    numerator() = constants::one;
                } else if (exp.IsFraction()) {
                    auto& f = exp.as<Fraction>();
                    auto in = e.getBase() / (denominator() ^ f.Reciprocal());
                    if (in.IsInt()) {
                        e.setBase(std::move(in));
                        Become(std::move(e));
                        break;
                    }
                }
            }

            // Integers
            if (numerator().IsInt()) {
                auto& n = numerator().ca();
                auto& denom = denominator();
                auto dni = denom.IsInt();
                if (n == 0) {
                    if (dni && denom.IsZero())
                        throw "NaN";
                    Become(std::move(numerator()));
                    break;
                }
                if (dni) {
                    auto& dn = denominator().ca();
                    if (n < 0 && dn < 0) {
                        numerator() = -numerator();
                        denominator() = -denominator();
                        reoptimize_the_fraction = true;
                        continue;
                    }
                    if (dn == constants::one) {
                        Become(std::move(numerator()));
                        break;
                    }
                    if (dn == -1) {
                        Become(-numerator());
                        break;
                    }
                    auto d = boost::gcd(n, dn);
                    if (d != 1) {
                        numerator() /= Integer(d);
                        denominator() /= Integer(d);
                        reoptimize_the_fraction = true;
                        continue;
                    }
                } else if (denom.IsSimple()) {
                    if (denom.IsPrincipalSurd()) {
                        auto& ps = denom.as<PrincipalSurd>();
                        auto& e = ps.Index();
                        numerator() *= ps ^ (e + constants::minus_1);
                        setDenominator(std::move(ps.Radicand()));
                        reoptimize_the_fraction = true;
                        continue;
                    }
                }
            } else if (numerator().IsProduct()) {
                Become(numerator() / denominator());
                break;
            }

            if (denominator().IsProduct()) {
                auto& dn = denominator().as<Product>();
                if (dn.Has(numerator())) {
                    denominator() /= numerator();
                    numerator() = constants::one;
                    reoptimize_the_fraction = true;
                    continue;
                } else if (dn.Has(-numerator())) {
                    denominator() /= -numerator();
                    numerator() = constants::minus_1;
                    reoptimize_the_fraction = true;
                    continue;
                } else {
                    if (dn.HasValueType<MinusOneSurd>()) {
                        numerator() *= constants::minus_1;
                        numerator() *= constants::i;
                        denominator() /= constants::i;
                    }
                    if (numerator().IsInt() || numerator().IsSimpleFraction()) {
                        for (auto& m : dn) {
                            if (m.IsVa()) {
                                numerator() *= m ^ -1;
                            } else if (m.IsExponentiation()) {
                                auto& e = m.as<Exponentiation>();
                                numerator() *= e.getBase() ^ -e.getExponentiation();
                            } else
                                numerator() /= m;
                        }
                        Become(std::move(numerator()));
                        break;
                    }
                }
            } else if (denominator().IsSum()) {
                auto& s = denominator().as<Sum>();
                auto lcm = s.LCMofMemberFractionDenominators();
                if (lcm != constants::one) {
                    numerator() *= lcm;
                    denominator() *= lcm;
                    reoptimize_the_fraction = true;
                    continue;
                } else if (denominator().IsSum()) {
                    auto& s = denominator().as<Sum>();
                    auto lcm = s.LCMofMemberFractionDenominators();
                    if (lcm != constants::one) {
                        numerator() *= lcm;
                        denominator() *= lcm;
                        reoptimize_the_fraction = true;
                        continue;
                    }
                }
            } else if (denominator().FindVa() && !denominator().IsSum()) {
                Become(Product{std::move(numerator()), Exponentiation(std::move(denominator()), constants::minus_1)});
                break;
            } else // no products
            {
                // TODO :
                // IMPLEMENT // uncomment to cover scenarios

                // sum
                //                auto s = Sum::cast(numerator());
                //                if (s) {
                //                    auto sum(std::move(*s));
                //                    sum /= denominator();
                //                    Become(std::move(sum));
                //                    return;
                //                }
            }
        } while (reoptimize_the_fraction);

        if (IsFraction()) {
            if (IsSimple())
                hash = numerator().Hash() ^ denominator().Hash();
            else if (!denominator().IsSum())
                Become(numerator() * (denominator() ^ constants::minus_1));
            else {
                hash = numerator().Hash() ^ denominator().Hash();
            }
        }
    }

    bool Fraction::MultiplyIfSimplifiable(const Valuable& v)
    {
        auto is = IsSimpleFraction() && v.IsSimple() && !v.IsRadical();
        if(is){
            *this *= v;
        } else if (!v.IsFraction()) {
            auto s = v.IsMultiplicationSimplifiable(*this);
            is = s.first;
            if (is) {
                Become(std::move(s.second));
            }
        } else {
            IMPLEMENT
        }
        return is;
    }

    std::pair<bool,Valuable> Fraction::IsMultiplicationSimplifiable(const Valuable& v) const
    {
        std::pair<bool,Valuable> is;
        is.first = IsSimpleFraction() && v.IsSimple();
        if(is.first){
            is.second = *this * v;
        } else if (v.IsConstant()) {
        } else if (!v.IsFraction()) {
            is = v.IsMultiplicationSimplifiable(*this);
        } else {
            IMPLEMENT
        }
        return is;
    }

bool Fraction::SumIfSimplifiable(const Valuable& v)
{
    auto is = IsSimpleFraction() && v.IsSimple();
    if(is){
        *this += v;
    } else if (!v.IsFraction()) {
        auto s = v.IsSummationSimplifiable(*this);
        is = s.first;
        if (is) {
            Become(std::move(s.second));
        }
    } else {
        IMPLEMENT
    }
    return is;
}

std::pair<bool,Valuable> Fraction::IsSummationSimplifiable(const Valuable& v) const
{
    std::pair<bool,Valuable> is;
    auto simple = IsSimpleFraction();
    is.first = simple && v.IsSimple();
    if(is.first){
        is.second = *this + v;
    } else if (v.IsVa()) {
        is.first = !simple && HasVa(v.as<Variable>());
        if (is.first) {
            LOG_AND_IMPLEMENT("Optimize summation of " << *this << " with " << v);
        }
    } else if (v.IsModulo()) {
    } else if (v.IsInt()) {
        is = (getDenominator() * v).IsSummationSimplifiable(getNumerator());
        if (is.first)
            is.second /= getDenominator();
    } else if (!v.IsFraction()) {
        is = v.IsSummationSimplifiable(*this);
    } else {
        IMPLEMENT
    }
    return is;
}

    Valuable& Fraction::operator +=(const Valuable& v)
    {
        if (v.IsFraction()){
            auto& f = v.as<Fraction>();
            if(denominator() == f.denominator()) {
                numerator() += f.numerator();
            } else {
                numerator() = numerator() * f.denominator() + f.numerator() * denominator();
                denominator() *= f.denominator();
            }
            optimized = {};
        }
        else if(v.IsInt() && IsSimple()) {
            setNumerator(numerator() + denominator() * v.as<Integer>());
        } else {
            return Become(Sum {*this, v});
        }

        optimize();
        return *this;
    }

    Valuable& Fraction::operator*=(const Fraction& f) {
        numerator() *= f.numerator();
        denominator() *= f.denominator();
        optimized = {};
        optimize();
        return *this;
    }

    Valuable& Fraction::operator *=(const Valuable& v)
    {
        if (v.IsFraction())
        {
            return operator*=(v.as<Fraction>());
        }
        else if (v.IsInt())
        {
            numerator() *= v;
            optimized = {};
        }
        else if (denominator() == v && v.IsMultival() == YesNoMaybe::No)
            return Become(std::move(numerator()));
        // TODO: Valuable::Has
        //else if (denominator().Has(v))
        //    setDenominator([](auto& d) { d /= v; });
        else
        {
            return Become(v * *this);
        }
		
        optimize();
        return *this;
    }

    Valuable& Fraction::operator /=(const Valuable& v)
    {
        if (v.IsFraction())
        {
            auto& f = v.as<Fraction>();
            numerator() *= f.denominator();
            denominator() *= f.numerator();
            optimized = {};
        }
        else if (v.IsProduct())
        {
            for(auto& _ : v.as<Product>())
                *this /= _;
        }
        else
        {
            denominator() *= v;
            optimized = {};
        }
        optimize();
        return *this;
    }

    Valuable& Fraction::operator %=(const Valuable& v)
    {
        Integer d(*this / v);
		return *this -= d * v;
    }

    Valuable& Fraction::operator^=(const Valuable& v)
    {
        if(v.IsFraction()){
            auto& vf = v.as<Fraction>();
            auto& vfdn = vf.denominator();
            if (vfdn == constants::two)
                ;
            else if (vfdn.bit() != constants::one) {
                return Become(Exponentiation(*this, v));
            }
        }
        numerator() ^= v;
        denominator() ^= v;
        optimized = {};
        optimize();
        return *this;
    }
    
    Valuable& Fraction::d(const Variable& x)
    {
        if (IsSimpleFraction()) {
            Become(0_v);
        } else {
            IMPLEMENT
            optimized = {};
        }
        return *this;
    }
    
    bool Fraction::operator <(const Valuable& v) const
    {
        if (v.IsFraction())
        {
            auto& f = v.as<Fraction>();
            return numerator() * f.denominator() < f.numerator() * denominator();
        }
        else if (v.IsInt())
        {
            if(denominator()<0)
                return -numerator() < v * -denominator();
            else
                return numerator() < v * denominator();
        }
        else
            return base::operator <(v);
    }
    
    Fraction::operator double() const
    {
        return static_cast<double>(numerator()) / static_cast<double>(denominator());
    }
    
    Fraction::operator a_rational() const
    {
        return static_cast<a_rational>(numerator()) / static_cast<a_rational>(denominator());
    }
    
    Valuable& Fraction::sq(){
        numerator().sq();
        denominator().sq();
        optimized = {};
        optimize();
        return *this;
    }

    Valuable Fraction::Sqrt() const
    {
        return numerator().Sqrt() / denominator().Sqrt();
    }

    std::ostream& Fraction::print_sign(std::ostream& out) const
    {
        return out << '/';
    }

    std::ostream& Fraction::code(std::ostream& out) const
    {
        if(IsSimpleFraction())
            out << operator double();
        else{
            out << '(';
            auto& n = numerator();
            if (n.IsInt())
                out << n.ca() << '.'; 
            else if (n.IsSimple())
                out << n.operator double();
            else
                n.code(out);
            out << '/';
            auto& d = denominator();
            if (d.IsSimple())
                out << d.operator double();
            else
                d.code(out);
            out << ')';
        }
        return out;
    }
    const Valuable::vars_cont_t& Fraction::getCommonVars() const
    {
        vars = numerator().getCommonVars();
        for(auto& r : denominator().getCommonVars())
        {
            vars[r.first] -= r.second;
        }
        return vars;
    }
    
    Valuable::vars_cont_t Fraction::GetVaExps() const
    {
        auto vars = numerator().GetVaExps();
        for (auto& r : denominator().GetVaExps()) {
            vars[r.first] -= r.second;
        }
        return vars;
    }

    Valuable Fraction::InCommonWith(const Valuable& v) const
    {
        auto c = 1_v;
        if (v.IsFraction()) {
            auto& f = v.as<Fraction>();
            c *= getNumerator().InCommonWith(f.getNumerator());
            c /= getDenominator().InCommonWith(f.getDenominator());
        }
        else
        {
            c = getNumerator().InCommonWith(v);
        }
        return c;
    }

    bool Fraction::IsComesBefore(const Valuable& v) const
    {
//        auto va1 = FindVa();
//        auto va2 = v.FindVa();
//        return !va1 && !va2
//            ? (IsSimple() && (v.IsInt() || (v.IsFraction() && Fraction::cast(v)->IsSimple())) ? operator<(v) : str().length() < v.str().length())
//                : (va1 && va2
//                   ? str().length() < v.str().length()
//                   : va1!=nullptr );
        auto mve = base::getMaxVaExp();
        auto vmve = v.getMaxVaExp();
        auto is = mve > vmve;
        if (mve != vmve)
        {}
        else if (v.IsFraction())
        {
            if (IsSimple() && v.IsSimpleFraction())
                is = *this < v;
            else
            {
                auto& f = v.as<Fraction>();
                is = numerator().IsComesBefore(f.numerator()) || denominator().IsComesBefore(f.denominator());
            }
//            auto e = cast(v);
//            bool numerator()IsVa = numerator().IsVa();
//            bool vbaseIsVa = e->numerator().IsVa();
//            if (numerator()IsVa && vbaseIsVa)
//                is = denominator() == e->denominator() ? numerator().IsComesBefore(e->numerator()) : denominator() > e->denominator();
//            else if(numerator()IsVa)
//                is = false;
//            else if(vbaseIsVa)
//                is = true;
//            else if(numerator() == e->numerator())
//                is = denominator().IsComesBefore(e->denominator());
//            else if(denominator() == e->denominator())
//                is = numerator().IsComesBefore(e->numerator());
//            else
//            {
//                auto expComesBefore = denominator().IsComesBefore(e->denominator());
//                auto ebaseComesBefore = numerator().IsComesBefore(e->numerator());
//                is = ebaseComesBefore || expComesBefore;//expComesBefore==ebaseComesBefore || str().length() > e->str().length();
//            }
        }
        else if(v.IsProduct())
            is = Product{*this}.IsComesBefore(v);
        else if(v.IsSum())
            is = Sum{*this}.IsComesBefore(v);
        else if(v.IsVa())
            is = FindVa();
        else if(v.IsInt())
            is = true;
        else
            IMPLEMENT

        return is;
    }
    
    Fraction::operator unsigned char() const
    {
        return static_cast<unsigned char>(static_cast<Integer>(*this));
    }
  
    Fraction::operator a_int() const
    {
        if (!IsSimple()) {
            IMPLEMENT
        }
        return static_cast<a_int>(numerator())/static_cast<a_int>(denominator());
    }
    
    Fraction::operator boost::multiprecision::cpp_dec_float_100() const
    {
        if (IsSimple())
        {
            boost::multiprecision::cpp_dec_float_100 f(numerator().ca());
            f /= boost::multiprecision::cpp_dec_float_100(denominator().ca());
            // TODO : check validity
            return f;
        }
        else
            IMPLEMENT;
    }
    
    Valuable Fraction::operator()(const Variable& v, const Valuable& augmentation) const
    {
        return (augmentation * denominator() - numerator())(v);
    }
    
    Valuable Fraction::Reciprocal() const
    {
        return Fraction(denominator(), numerator());
    }

    Valuable& Fraction::reciprocal() {
        std::swap(_1, _2);
        return *this;
    }

    Valuable Fraction::Sign() const {
        OptimizeOn oo;
        return numerator().Sign() * denominator().Sign();
    }

    Valuable Fraction::abs() const {
        return Sign() == constants::one
            ? Valuable(Clone())
            : std::abs(numerator()) / std::abs(denominator());
    }

}}
