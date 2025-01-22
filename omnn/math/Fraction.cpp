//
// Created by Сергей Кривонос on 01.09.17.
//
#include "Fraction.h"
#include "Infinity.h"
#include "Integer.h"
#include "Modulo.h"
#include "NaN.h"
#include "Sum.h"
#include "Product.h"
#include "PrincipalSurd.h"
#include "i.h"
#include "VarHost.h"

#include <utility>

#include <boost/lexical_cast.hpp>
#if !defined(__APPLE__) && !defined(NDEBUG)
#include <boost/stacktrace.hpp>
#endif


using namespace omnn::math;


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
            numerator() = Integer(boost::multiprecision::cpp_int(s.c_str() + s.find_first_not_of('0', afterDotI)));
            if (s[0]=='-') {
                numerator() = -numerator();
            }
            s.erase(doti);
            if (!s.empty()) {
                boost::multiprecision::cpp_int aliquot(s);
                if (aliquot != 0)
                    operator+=(aliquot);
            }
        }
    }
    
	Valuable Fraction::operator -() const
    {
        auto frac = ptrs::make_shared<Fraction>(-getNumerator(), getDenominator());
        frac->optimize();
        return Valuable(std::move(frac));
    }

    bool Fraction::operator==(const Fraction& fraction) const {
        auto equal = base::operator==(fraction);
        if (!equal) {
            auto optimized1 = is_optimized();
            auto optimized2 = fraction.is_optimized();
            auto bothOptimized = optimized1 && optimized2;
            if (!bothOptimized) {
                auto rational1 = IsRational();
                auto rational2 = fraction.IsRational();
                if (rational1 == rational2) {
                    auto bothAreRational = (IsRational() && fraction.IsRational()) == YesNoMaybe::Yes;
                    if (bothAreRational) {
                        equal = operator a_rational() == static_cast<a_rational>(fraction);
                    } else { // TODO: FIXME: both irrationals hangs on macos
                        //OptimizeOn on;
                        //equal = numerator() * fraction.denominator() == fraction.numerator() * denominator();
                    }
                }
            }
        }
        return equal;
    }

    bool Fraction::operator==(const Valuable& v) const {
        auto eq = v.IsFraction();
        if (eq) {
            eq = operator==(v.as<Fraction>());
        } else if (v.IsExponentiation() || v.IsSum() || v.IsProduct()) {
            eq = v.operator==(*this);
        } else if (v.IsInt()) {
            eq = getNumerator() == v && getDenominator() == constants::one;
        } else if (!is_optimized()) {
            eq = denominator() == constants::one && numerator() == v;
        } else if (v.IsVa()) {
        } else if (v.IsPrincipalSurd()) {
        } else if (v.IsConstant()) {
        } else if (v.IsLogarithm()) {
        } else if (v.IsInfinity()) {
        } else if (v.IsNaN()) {
        } else if (v.IsModulo()) {
        } else {
            LOG_AND_IMPLEMENT(*this << " =?= " << v);
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
        DUO_OPT_PFX

        bool reoptimize_the_fraction;
        do {
            numerator().optimize();
            denominator().optimize();
            reoptimize_the_fraction = {};

            if (denominator() == numerator() && IsMultival() != YesNoMaybe::Yes) {
                if (denominator().IsZero()) {
                    Become(NaN());
                    return;
                } else {
                    auto variable = denominator().FindVa();
                    if (variable)
                        variable->getVaHost()->LogNotZero(denominator());
                }
                Become(1);
                break;
            }

            if (numerator().IsZero()) {
                auto& dn = denominator();
                if (dn.IsZero()) {
                    Become(NaN());
                } else {
                    auto variable = dn.FindVa();
                    if (variable)
                        variable->getVaHost()->LogNotZero(denominator());
                }
				Become(std::move(numerator()));
                break;
			}

            if (numerator().IsFraction()) {
                auto& num = numerator().as<Fraction>();
                num.numerator() *= denominator();
                Become(std::move(numerator()));
                break;
            }

            auto gcd = numerator().GCD(denominator());
            auto gcdIsMultival = gcd.IsMultival() == YesNoMaybe::Yes;
            if (gcdIsMultival) {
                if (gcd == constants::plus_minus_1) {
                    if (gcd == denominator()) {
                        Become(std::move(numerator()));
                        break;
                    } else if (denominator().IsProduct()) {
                        denominator().as<Product>().Delete(gcd);
                        reoptimize_the_fraction = true;
                        continue;
                    }
                }
            } else {
                reoptimize_the_fraction = !gcd.IsZero() && gcd != 1;
                if (reoptimize_the_fraction) {
                    numerator() /= gcd;
                    denominator() /= gcd;
                    continue;
                }
            }

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
                auto& fdn = denominator().template as<Fraction>();
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
                auto& e = numerator().template as<Exponentiation>();
                auto& exp = e.eexp();
                if (exp.IsInt() && exp < 0) {
                    denominator() *= e.ebase() ^ (-exp);
                    numerator() = constants::one;
                } else if (exp.IsFraction()) {
                    auto& f = exp.template as<Fraction>();
                    auto in = e.ebase() / (denominator() ^ f.Reciprocal());
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
                if (n.is_zero()) {
                    if (dni && denom.IsZero()) {
                        Become(NaN(SharedFromThis()));
                    } else {
                        Become(std::move(numerator()));
                    }
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
                    auto d = boost::integer::gcd(n, dn);
                    if (d != 1) {
                        numerator() /= Integer(d);
                        denominator() /= Integer(d);
                        reoptimize_the_fraction = true;
                        continue;
                    }
                } else if (denom.IsSimple()) {
                    if (denom.IsPrincipalSurd()) {
                        auto& ps = denom.template as<PrincipalSurd>();
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
                auto& dn = denominator().template as<Product>();
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
                                auto& e = m.template as<Exponentiation>();
                                numerator() *= e.ebase() ^ -e.eexp();
                            } else
                                numerator() /= m;
                        }
                        Become(std::move(numerator()));
                        break;
                    }
                }
            } else if (denominator().IsSum()) {
                auto& s = denominator().template as<Sum>();
                auto lcm = s.LCMofMemberFractionDenominators();
                if (lcm != constants::one) {
                    numerator() *= lcm;
                    denominator() *= lcm;
                    reoptimize_the_fraction = true;
                    continue;
                } else if (denominator().IsSum()) {
                    auto& s = denominator().template as<Sum>();
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

    bool Fraction::MultiplyIfSimplifiable(const Fraction& fraction) {
        auto isNumeratorsMultiplicationSimplifiable = numerator().IsMultiplicationSimplifiable(fraction.numerator());
        auto isDenominatorsMultiplicationSimplifiable = denominator().IsMultiplicationSimplifiable(fraction.denominator());
        auto simplifiable = isNumeratorsMultiplicationSimplifiable.first || isDenominatorsMultiplicationSimplifiable.first;
        if (simplifiable) {
            if (isNumeratorsMultiplicationSimplifiable.first) {
                setNumerator(std::move(isNumeratorsMultiplicationSimplifiable.second));
            } else {
                numerator() *= fraction.numerator();
                optimized = {};
            }

            if (isDenominatorsMultiplicationSimplifiable.first) {
                setDenominator(std::move(isDenominatorsMultiplicationSimplifiable.second));
            } else {
                denominator() *= fraction.denominator();
                optimized = {};
            }

            optimize();
        }
        return simplifiable;
    }

    bool Fraction::MultiplyIfSimplifiable(const Valuable& v)
    {
        auto simplifiable = IsSimpleFraction() && v.IsSimple() && !v.IsRadical();
        if(simplifiable){
            *this *= v;
        } else if (v.IsFraction()) {
            simplifiable = MultiplyIfSimplifiable(v.as<Fraction>());
        } else {
            auto s = v.IsMultiplicationSimplifiable(*this);
            simplifiable = s.first;
            if (simplifiable) {
                Become(std::move(s.second));
            }
        }
        return simplifiable;
    }

    std::pair<bool,Valuable> Fraction::IsMultiplicationSimplifiable(const Valuable& v) const
    {
        std::pair<bool,Valuable> is;
        is.first = IsSimpleFraction() && v.IsSimple();
        if (!is.first) {
            auto gcd = denominator().GCD(v);
            is.first = gcd != constants::one;
        }
        if (is.first) {
            is.second = *this * v;
        } else {
            is = numerator().IsMultiplicationSimplifiable(v);
            if (is.first) {
                is.second /= denominator();
            }
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

std::pair<bool,Valuable> Fraction::IsSummationSimplifiable(const Valuable& value) const
{
    std::pair<bool,Valuable> is;
    is.first = IsSimple() && value.IsSimpleFraction();
    if(is.first){
        is.second = *this + value;
        is.first = is.second.Complexity() <= Complexity() + value.Complexity();
    } else if (value.IsVa()) {
        is.first = HasVa(value.as<Variable>());
        if (is.first) {
            LOG_AND_IMPLEMENT("Optimize summation of " << *this << " with " << value);
        }
    } else if (value.IsModulo()) {
    } else if (value.IsInt()) {
        is = (getDenominator() * value).IsSummationSimplifiable(getNumerator());
        if (is.first)
            is.second /= getDenominator();
    } else if (!value.IsFraction()) {
        is = value.IsSummationSimplifiable(*this);
    } else {
        auto& frac = value.as<Fraction>();
        if (getDenominator() == frac.getDenominator()) {
            is = getNumerator().IsSummationSimplifiable(frac.getNumerator());
            if (is.first) {
                is.second /= getDenominator();
            }
        } else {
            is = (getNumerator() * frac.getDenominator())
                     .IsSummationSimplifiable(frac.getNumerator() * getDenominator());
            if (is.first) {
                auto commonDenominator = getDenominator() * frac.getDenominator(); // probably faster than getDenominator().LCM(frac.getDenominator());
                is.second /= commonDenominator;
            }
        }
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
        return Become(Modulo(*this, v));
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
            Become(0);
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

    Valuable& Fraction::sqrt() {
        std::function<void(Valuable&)> updater = [this](Valuable& num) {
            num *= denominator();
            num.sqrt();
        };
        updateNumerator(updater);
        optimize();
        return *this;
    }

    Valuable Fraction::Sqrt() const
    {
        return (numerator() * denominator()).sqrt() / denominator();
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

    Valuable Fraction::InCommonWith(const Fraction& frac) const {
        auto common = getNumerator().InCommonWith(frac.getNumerator());
        common /= getDenominator().InCommonWith(frac.getDenominator());
        return common;
    }

    Valuable Fraction::InCommonWith(const Valuable& v) const {
        auto common = constants::one;
        if (v.IsFraction()) {
            common = InCommonWith(v.as<Fraction>());
        } else if (v.IsProduct() || v.IsSum()) {
            common = v.InCommonWith(*this);
        } else {
            common = getNumerator().InCommonWith(v);
        }
        return common;
    }

    bool Fraction::IsComesBefore(const Fraction& fraction) const {
        bool is;
        auto numBefore = numerator().IsComesBefore(fraction.numerator());
        auto denomBefore = denominator().IsComesBefore(fraction.denominator());
        if (numBefore == denomBefore) {
            is = numBefore;
        } else if (numerator() == fraction.numerator()) {
            is = denomBefore;
        } else if (denominator() == fraction.denominator()) {
            is = numBefore;
        } else if (operator==(fraction)) {
            is = numerator().IsComesBefore(fraction.numerator());
        } else if (fraction.IsSimpleFraction() && IsSimpleFraction()) {
            is = operator<(fraction);
        } else {
            VarHost::NonZeroLogOffScope zeroLogOff;
            auto denominatorsLCM = denominator().LCM(fraction.denominator());
            auto lhs = numerator() * denominatorsLCM / denominator();
            auto rhs = fraction.numerator() * denominatorsLCM / fraction.denominator();
            is = lhs != rhs && lhs.IsComesBefore(rhs);
        }
        return is;
    }

    bool Fraction::IsComesBefore(const Valuable& v) const
    {
        bool is;
        if (v.IsFraction())
            is = IsComesBefore(v.as<Fraction>());
        else if (v.IsProduct()) {
            auto& prod = v.as<Product>();
            if (prod.size() == 1)
                is = IsComesBefore(prod.begin()->get());
            if (prod.size() == 0)
                is = IsComesBefore(constants::one);
            else
                is = Product{*this}.IsComesBefore(prod);
        } else if (v.IsSum())
            is = Sum{*this}.IsComesBefore(v);
        else if (v.IsVa())
            is = FindVa();
        else if (v.IsInt())
            is = true;
        else if (IsSimple() && v.IsExponentiation())
            is = {};
        else {
            auto degreeDiff = base::getMaxVaExp() - v.getMaxVaExp();
            if (!degreeDiff.is_zero()) {
                is = degreeDiff.sign() >= 0;
            }
        }

        return is;
    }
    
    Fraction::operator unsigned char() const
    {
        return static_cast<unsigned char>(static_cast<Integer>(*this));
    }
  
    Fraction::operator a_int() const
    {
        if (IsRational() == YesNoMaybe::Yes) {
            auto rational = operator a_rational();
            return static_cast<a_int>(rational);
        } else {
#if defined(__APPLE__) || defined(NDEBUG)
            LOG_AND_IMPLEMENT("Converting " << *this << " to integer value");
#else
            LOG_AND_IMPLEMENT("Converting " << *this << " to integer value:\n" << boost::stacktrace::stacktrace());
#endif
        }
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


const PrincipalSurd* Fraction::PrincipalSurdFactor() const {
    auto hasPrincipalSurd = numerator().PrincipalSurdFactor();
    if (!hasPrincipalSurd)
        hasPrincipalSurd = denominator().PrincipalSurdFactor();
    return hasPrincipalSurd;
}

size_t Fraction::FillPolynomialCoefficients(std::vector<Valuable>& coefficients, const Variable& v) const {
    if (!IsPolynomial(v)) {
        IMPLEMENT
    }

    if (_2.HasVa(v)) {
        IMPLEMENT
    }

    // Get the grade first to ensure proper sizing
    std::vector<Valuable> coef;
    auto grade = _1.FillPolynomialCoefficients(coef, v);
    // Ensure coefficients vector has enough space
    if (coefficients.size() <= grade) {
        coefficients.resize(grade + 1);
    }
    // Divide all coefficients by denominator
    for (size_t i = 0; i <= grade; ++i) {
        coef[i] /= _2;
        coefficients[i] += std::move(coef[i]);
    }
    return grade;
}
