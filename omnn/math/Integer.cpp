//
// Created by Сергей Кривонос on 01.09.17.
//
#include "Integer.h"

#include "i.h"
#include "Infinity.h"
#include "Exponentiation.h"
#include "Fraction.h"
#include "Modulo.h"
#include "NaN.h"
#include "PrincipalSurd.h"
#include "Product.h"
#include "Sum.h"

#include <omnn/rt/Divisors.hpp>
#include <omnn/rt/tasq.h>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>

#include <boost/archive/binary_oarchive.hpp>
#if __cplusplus >= 201700
#include <random>
#ifndef __GNUC__
namespace std {
    template< class It >
    void random_shuffle( It f, It l )
    {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(f, l, g);
    }
}
#endif
#endif
#ifdef OPENMIND_USE_OPENCL
#include <boost/compute.hpp>
#endif
#include <boost/filesystem.hpp>
#include <boost/functional/hash.hpp>
#include <boost/math/special_functions/prime.hpp>
#include <boost/multiprecision/integer.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/detail/default_ops.hpp>
#include <boost/multiprecision/detail/integer_ops.hpp>
#include <boost/multiprecision/integer.hpp>
#include <boost/multiprecision/miller_rabin.hpp>
#include <boost/numeric/conversion/cast.hpp>


using namespace omnn::rt;

using boost::multiprecision::cpp_int;

namespace omnn{
namespace math {

    const Integer::ranges_t Integer::empty_zero_zone;

    Integer::Integer(const Fraction& f)
    : arbitrary(f.getNumerator().ca() / f.getDenominator().ca())
    {

    }

    Integer::operator a_int() const {
        return arbitrary;
    }

    a_int& Integer::a() {
        return arbitrary;
    }

    const a_int& Integer::ca() const {
        return arbitrary;
    }

    Integer::operator int64_t() const {
        return boost::numeric_cast<int64_t>(arbitrary);
    }

    Integer::operator uint64_t() const {
        return boost::numeric_cast<uint64_t>(arbitrary);
    }

    YesNoMaybe Integer::IsEven() const {
        return (arbitrary >= 0 ? arbitrary : -arbitrary) & 1 ? YesNoMaybe::No : YesNoMaybe::Yes;
    }


    Valuable Integer::operator -() const
    {
        return Integer(-arbitrary);
    }

    Valuable& Integer::operator +=(const Valuable& v)
    {
        if (v.IsInt())
        {
            arbitrary += v.ca();
            hash = std::hash<base_int>()(arbitrary);
        }
        else
        {
            Become(v + *this);
        }
        return *this;
    }

    Valuable& Integer::operator +=(int v)
    {
        arbitrary += v;
        hash = std::hash<base_int>()(arbitrary);
        return *this;
    }

    Valuable& Integer::operator *=(const Valuable& v)
    {
        if (v.IsInt())
        {
            arbitrary *= v.ca();
            hash = std::hash<base_int>()(arbitrary);
        }
        else
        {
            // all other types should handle multiplication by Integer
            Become(v**this);
        }
        return *this;
    }

    bool Integer::MultiplyIfSimplifiable(const Valuable& v)
    {
        auto is = v.IsInt();
        if(is) {
            arbitrary *= v.ca();
            hash = std::hash<base_int>()(arbitrary);
        } else if (v.IsVa()) {
        } else {
            auto s = v.IsMultiplicationSimplifiable(*this);
            is = s.first;
            if(is)
                Become(std::move(s.second));
        }
        return is;
    }

    std::pair<bool,Valuable> Integer::IsMultiplicationSimplifiable(const Valuable& v) const
    {
        std::pair<bool,Valuable> is;
        is.first = v.IsInt() || v.IsFraction();
        if (is.first) {
            is.second = v * *this;
            if (is.second.Complexity() > v.Complexity()) {
                LOG_AND_IMPLEMENT("Simplification failure: " << *this << "  *  " << v << " resulted in " << is.second);
            }
        } else if (v.IsVa() || v.IsExponentiation()) {
        } else {
            is = v.IsMultiplicationSimplifiable(*this);
        }
        return is;
    }

    bool Integer::SumIfSimplifiable(const Valuable& v)
    {
        auto is = v.IsInt();
        if(is) {
            arbitrary += v.ca();
            hash = std::hash<base_int>()(arbitrary);
        } else {
            auto s = v.IsSummationSimplifiable(*this);
            is = s.first;
            if(is)
                Become(std::move(s.second));
        }
        return is;
    }

    std::pair<bool,Valuable> Integer::IsSummationSimplifiable(const Valuable& v) const
    {
        std::pair<bool,Valuable> is;
        is.first = v.IsInt();
        if (is.first) {
            is.second = v + *this;
        } else if (v.IsVa() || v.IsExponentiation()) {
        } else if (v.IsProduct()) {
        } else {
            is = v.IsSummationSimplifiable(*this);
            if (is.first && is.second.Complexity() > v.Complexity()){
#if !defined(NDEBUG) && !defined(NOOMDEBUG)
                LOG_AND_IMPLEMENT("Simplification complexity exceeds source complexity: " << v << "   +   " << str() << "   =   " << is.second);
#endif
                is.first = {};
            }
        }
        return is;
    }

    Valuable& Integer::operator /=(const Valuable& v)
    {
        auto variable = v.FindVa();
        if (variable) {
            variable->getVaHost()->LogNotZero(v);
        }
        if (arbitrary.is_zero() ) {
            if (v.IsZero()) {
                Become(NaN());
            }
        }
        else if (v.IsInt())
        {
            auto& a = v.ca();
            if (a == 0) {
                if (arbitrary < 0) {
                    Become(MInfinity());
                } else if (arbitrary > 0) {
                    Become(Infinity());
                } else {
                    Become(NaN());
                }
            } else if (arbitrary % a == 0) {
                arbitrary /= a;
                hash = std::hash<base_int>()(arbitrary);
            } else {
                Become(Fraction(*this, v));
            }
        } else if (v.IsSimple()) {
            Become(Fraction(*this, v));
        }
        else
            *this *= v^-1;
        return *this;
    }

    Valuable& Integer::operator %=(const Valuable& v)
    {
        if (v.IsInt())
        {
            arbitrary %= v.ca();
            hash = std::hash<base_int>()(arbitrary);
        }
        else
        {
            Become(Modulo(*this, v));
        }
        return *this;
    }

    std::pair<bool,Valuable> Integer::IsModSimplifiable(const Valuable& v) const
    {
        if (v.IsZero()) {
            return {true, *this};
        }
        if (v.IsInt()) {
            return {true, *this % v};
        }
        if (v.IsFraction() || v.IsVa() || v.IsExponentiation()) {
            return {};
        }
        auto is = v.IsModSimplifiable(*this);
        if (is.first && is.second.Complexity() > v.Complexity()) {
            is.first = false;
        }
        return is;
    }

    Valuable& Integer::operator --()
    {
        --arbitrary;
        hash = std::hash<base_int>()(arbitrary);
        return *this;
    }

    Valuable& Integer::operator ++()
    {
        ++arbitrary;
        hash = std::hash<base_int>()(arbitrary);
        return *this;
    }

    Integer::operator int() const
    {
        return boost::numeric_cast<int>(arbitrary);
    }

    Integer::operator uint32_t() const
    {
        return boost::numeric_cast<uint32_t>(arbitrary);
    }

    Integer::operator double() const
    {
        return boost::numeric_cast<double>(arbitrary);
    }

    Integer::operator long double() const
    {
        return boost::numeric_cast<long double>(arbitrary);
    }

    Integer::operator a_rational() const
    {
        return a_rational(arbitrary);
    }

    Integer::operator unsigned char() const
    {
        return boost::numeric_cast<unsigned char>(arbitrary);
    }

    Valuable Integer::bit(const Valuable& n) const
    {
        if (n.IsInt()) {
            if (arbitrary < 0) {
                if (arbitrary == -1) {
                    return 1;
                } else {
                    return 1-bit(-n);
                }
            }
            unsigned N = static_cast<unsigned>(n);
            return static_cast<int>(bit_test(arbitrary, N));
        }
        else
            LOG_AND_IMPLEMENT(n << "th bit of " << *this);
    }

    Valuable& Integer::shl()
    {
        arbitrary = arbitrary << 1;
        hash = std::hash<base_int>()(arbitrary);
        return *this;
    }

    Valuable& Integer::shl(const Valuable& n)
    {
        if (n.IsInt()) {
            arbitrary = arbitrary << static_cast<int>(n);
            hash = std::hash<base_int>()(arbitrary);
        } else
            base::shl(n);
        return *this;
    }

    Valuable& Integer::shr(const Valuable& n)
    {
        if (n.IsInt()) {
            arbitrary = arbitrary >> static_cast<int>(n);
            hash = std::hash<base_int>()(arbitrary);
        } else
            base::shl(n);
        return *this;
    }

    Valuable Integer::Shr() const
    {
        return Integer(decltype(arbitrary)(arbitrary>>1));
    }

    Valuable Integer::Shr(const Valuable& n) const
    {
        if (!n.IsInt()) {
            IMPLEMENT
        }
        return Integer(decltype(arbitrary)(arbitrary>>static_cast<unsigned>(n)));
    }

    Valuable Integer::Or(const Valuable& n, const Valuable& v) const
    {
        IMPLEMENT
    }
    Valuable Integer::And(const Valuable& n, const Valuable& v) const
    {
        if (v.IsInt()) {
            auto mask = std::move(--((vo<2>() ^ n).a()));
            if (arbitrary < v.ca()) {
                mask &= arbitrary;
                mask &= v.ca();
            } else {
                mask &= v.ca();
                mask &= arbitrary;
            }
            return Valuable(std::move(mask));
        }
        else
            return base::And(n, v);
    }
    Valuable Integer::Xor(const Valuable& n, const Valuable& v) const
    {
        IMPLEMENT
    }
    Valuable Integer::Not(const Valuable& n) const
    {
        return Integer(~arbitrary);
    }

    constexpr bool Integer::IsPositivePowerOf2() const {
        return arbitrary > 0
            && (arbitrary & (arbitrary - 1)) == 0;
    }

    std::pair<Valuable,Valuable> Integer::GreatestCommonExp(const Valuable &e) const {
        // test : 50_v.GCE(2) == 5_v
        //        32_v.GCE(2) == 4_v
        //  which is needed to be powered into 2 to get gretest devisor that may be powered into 2 to get devisor of the initial value
        if (e == constants::one)
            return {*this,*this};

        auto& divisors = omnn::rt::DivisorsLookupTable::Divisors(ca());
        auto eIsPowerOf2 = divisors.size() > 0
                        && e.IsInt()
                        && e.as<Integer>().IsPositivePowerOf2();
        auto rnd = divisors.rend();
        for (auto it = divisors.rbegin(); it != rnd; ++it) {
            auto& divisor = *it;
            if (eIsPowerOf2) {
                auto value = divisor;
                for (auto power = e; power > constants::one;) {
                    power.shr();
                    value = boost::multiprecision::sqrt(value);
                }
                auto divisorPowerE = value ^ e;
                if (divisorPowerE == divisor) {
                    return {std::move(value), std::move(divisorPowerE.a())};
                }
            } else if (e < constants::zero) {
                LOG_AND_IMPLEMENT(arbitrary << " GreatestCommonExp " << e);
            } else {
                IMPLEMENT
                // auto v = boost::multiprecision::pow(boost::multiprecision::cpp_rational(divisor,1),
                // boost::multiprecision::cpp_rational{1, e.ca()}); if ((v ^ e) == divisor)
                //     return {std::move(v), divisor};
            }
        }
        return {constants::one, constants::one};
    }

    Valuable& Integer::operator^=(const Valuable& v)
    {
        if (arbitrary.is_zero() || (arbitrary == 1 && v.IsInt()))
        {
            if (v.IsZero()) {
                Become(NaN());
            }
            return *this;
        } else if ((arbitrary == 1 || arbitrary == -1) && v.IsSimpleFraction()) {
            if(!v.as<Fraction>().getDenominator().IsEven())
                return *this;
            else
                return Become(Exponentiation{*this,v});
        } else if (v.IsSimple() && v < constants::zero) {
            return reciprocal().operator^=(-v);
        }
        if(v.IsInt())
        {
// FIXME:     arbitrary = boost::multiprecision::pow(a(), v.ca());
//            hash = std::hash<base_int>()(arbitrary);
//            return *this;
            if (!v.IsZero()) {
                if (v > 1) {
                    Valuable x = *this;
                    Valuable n = v;
                    if (n < constants::zero)
                    {
                        x.reciprocal();
                        n = -n;
                    }
                    if (n.IsZero())
                    {
                        arbitrary = 1;
                        hash = std::hash<base_int>()(arbitrary);
                        return *this;
                    }
                    auto y = constants::one;
                    while (n > constants::one)
                    {
                        auto nIsInt = n.IsInt();
                        if (!nIsInt) IMPLEMENT;
                        if (n.ca() & 1)
                        {
                            y *= x;
                            --n;
                        }
                        x.sq();
                        n /= 2;
                    }
                    x *= y;
                    if (x.IsInt()) {
                        arbitrary = std::move(x.a());
                        hash = std::hash<base_int>()(arbitrary);
                    }
                    else
                        return Become(std::move(x));
                } else if (v != 1) {
                    return Become(Exponentiation{*this, v});
                }
            }
            else { // zero
                if (arbitrary == 0)
                    throw "NaN"; // feel free to handle this properly
                else
                {
                    arbitrary = 1;
                    hash = std::hash<base_int>()(arbitrary);
                }
            }
        }
        else if(v.IsSimpleFraction())
        {
            auto& f = v.as<Fraction>();
            auto& nu = f.getNumerator();
            auto& dnm = f.getDenominator();
            Valuable mn;
            auto nlz = nu < constants::zero;
            if(nlz)
                mn = -nu;
            auto n = std::cref(nlz ? mn : nu);
            auto dlz = dnm < constants::zero;
            auto ltz = nlz != dlz;
            auto dn = dlz ? -dnm : dnm;

            auto numeratorIsOne = n == constants::one;
            if (!numeratorIsOne){
                *this ^= n;
                n = std::cref(constants::one);
            }

            auto isNeg = operator<(constants::zero);
            auto signs = 0; //dimmensions
            while(dn.IsEven() == YesNoMaybe::Yes) {
                auto minus = arbitrary < 0;
                auto _ = boost::multiprecision::sqrt(minus ? -arbitrary : arbitrary);
                auto _sq = _ * _;
                if (_sq == boost::multiprecision::abs(arbitrary)){
                    arbitrary = _;
                    hash = std::hash<base_int>()(arbitrary);
    //                Become(isNeg ? -operator-().Sqrt() : Sqrt());
                    dn /= 2;
                    ++signs;
                } else {
//                    IMPLEMENT
                    if(n!=constants::one)
                        IMPLEMENT;
                    auto gce = GreatestCommonExp(dn);
                    auto ee = ltz
                        ? Fraction { constants::minus_1, std::move(dn) }
                        : Fraction { std::move(n), std::move(dn) }; // [-]1/dn
                    return Become(gce.first * Exponentiation{operator/=(gce.second), std::move(ee)});
                }
            }
            if(signs)
                hash = std::hash<base_int>()(arbitrary);

            auto dnSubZ = dn < 0;
            if(dnSubZ)
                dn = -dn;
            if(dn != 1_v) {
                auto even = dn.IsEven();
                if(even == YesNoMaybe::Yes){
                    Valuable x = *this;
                    if(x<constants::zero){
                        Valuable exp;
                        if (!numeratorIsOne) {
                            *this ^= n;
                            exp = dn.Reciprocal();
                        }
                        auto& exponentiating = numeratorIsOne ? v : exp;
                        auto xFactors = FactSet();
                        auto rb = xFactors.rbegin(), re = xFactors.rend();
                        for (auto it = rb; it != re; ++it) {
                            auto& xFactor = *it;
                            if(xFactor > constants::one /* && !operator==(xFactor) */){
                                auto e = xFactor ^ dn;
                                if(operator==(e))
                                    return Become(e*(1_v^exponentiating));
                                auto f = xFactor ^ exponentiating;
                                if(!f.IsInt())
                                    f /= 1_v ^ exponentiating;
                                if(f.IsInt())
                                    return Become(f*((x/xFactor)^exponentiating));
                            }
                        }
                        IMPLEMENT
                    }
                }
//                else if(dn==2_v) {
//                    return Become(Sqrt()*(1^(1_v/2)));
//                }

                Valuable nroot;
                bool rootFound = false;
                Valuable left =0, right = *this;

                while (!rootFound)
                {
                    auto d = right - left;
                    d -= d % 2;
                    if (d!=0) {
                        nroot = left + d / 2;
                        auto result = nroot ^ dn;
                        rootFound = result == *this;
                        if (rootFound)
                            break;
                        else
                            if (result > *this)
                                right = nroot;
                            else
                                left = nroot;
                    }
                    else {
                        nroot = Exponentiation(*this, dn.Reciprocal());
                        break;
                    }
                    // *this ^ 1/dn  == (nroot^dn + t)^ 1/dn
                    // this == nroot^dn +
                    // TODO : IMPLEMENT//return Become(Sum {nroot, (*this-(nroot^dn))^(1/dn)});
                }
                return Become(std::move(nroot));
            }
            if(dnSubZ)
                reciprocal();
            if(signs) {
                return operator*=((isNeg ? constants::minus_1 : constants::one) ^ ((2_v ^ signs).Reciprocal()));
            }
        }
        else
            return Become(Exponentiation(*this, v));

        optimize();
        return *this;
    }

    Valuable& Integer::d(const Variable& x)
    {
        arbitrary = 0;
        hash = std::hash<base_int>()(arbitrary);
        return *this;
    }

    bool Integer::IsComesBefore(const Product& product) const {
        if (product.size() == 1) {
            return IsComesBefore(product.begin()->get());
        } else if (product.size() == 0) {
            return IsComesBefore(constants::one);
        } else {
            return {};
        }
    }

    bool Integer::IsComesBefore(const Sum& sum) const {
        if (sum.size() == 1) {
            return IsComesBefore(sum.begin()->get());
        } else if (sum.size() == 0) {
            return IsComesBefore(constants::zero);
        } else {
            return {};
        }
    }

    bool Integer::IsComesBefore(const Valuable& v) const {
        if (v.IsProduct()) {
            return IsComesBefore(v.as<Product>());
        } else if (v.IsSum()) {
            return IsComesBefore(v.as<Sum>());
        } else if (v.IsPrincipalSurd()) {
            return {};
        } else if (v.IsInt()) {
            return *this > v;
        } else {
            return v.FindVa() != nullptr;
        }
    }

    Valuable Integer::InCommonWith(const Valuable& v) const
    {
        //constexpr 
        auto one = boost::multiprecision::cpp_int(1);
        return arbitrary == one ? Valuable(one) : v.GCD(*this);
    }

    Valuable Integer::GCD(const Valuable& value) const {
        return value.IsInt()
            ? Valuable(std::static_pointer_cast<Valuable>(std::make_shared<Integer>(boost::integer::gcd(value.ca(), ca()))))
            : value.GCD(*this);
    }

    Valuable Integer::LCM(const Valuable& v) const {
        return v.IsInt() ? boost::lcm(v.ca(), ca()) : v.LCM(*this);
    }

    Valuable& Integer::lcm(const Valuable& v) {
        if (v.IsInt()) {
            arbitrary = boost::lcm(v.ca(), ca());
            hash = std::hash<base_int>()(arbitrary);
        } else if (operator==(constants::one)) {
            Become(Valuable(v));
        } else if (operator==(constants::zero)) {
            Become(0);
        } else {
            Become(v.LCM(*this));
        }
        return *this;
    }

    Valuable Integer::Sqrt() const
    {
        auto arbitraryAbsoluteValue = boost::multiprecision::abs(arbitrary);
        auto sqrtIntegerPart = boost::multiprecision::sqrt(arbitraryAbsoluteValue); // integer square root
        if (sqrtIntegerPart * sqrtIntegerPart != arbitraryAbsoluteValue) { // no integer square root
            auto d = GreatestCommonExp(constants::two);
            if (d.second != constants::one) {
                return (*this / d.second).Sqrt() * d.first;
            }
            else
                return Valuable(std::make_shared<PrincipalSurd>(*this));
        }
        return arbitrary.sign() == -1 ? constant::i * sqrtIntegerPart : sqrtIntegerPart;
    }

    // Sqrt using rational root test
//    Valuable Integer::Sqrt() const
//    {
//        auto minus = arbitrary < 0;
//
//        // build an equation, find using RRT:
//        Variable x;
//        Valuable eq = *this;
//        auto _ = x.Sq();
//        eq += minus ? _ : -_;
//        eq.SetView(View::Solving);
//
//        auto esurrto = enforce_solve_using_rational_root_test_only;
//        enforce_solve_using_rational_root_test_only = true;
//        auto sols = eq.Solutions(x);
//        enforce_solve_using_rational_root_test_only = esurrto;
//
//        if(!sols.size()){
//            IMPLEMENT
//        }
//        auto it = sols.begin();
//        if(*it < 0)
//            ++it;
//        _ = *it;
//
//        return minus ? constant::i * _ : _;
//    }

    std::wstring Integer::save(const std::wstring& f) const
    {
        if (!f.empty()) {
            boost::filesystem::path p(f);
            std::ofstream s(p.string(), std::ios::binary);
            boost::archive::binary_oarchive a(s);
            a & arbitrary;
        } else {
            LOG_AND_IMPLEMENT("Attempt to save " << *this << " to empty path file");
        }
        return f;
    }

    Valuable Integer::Sign() const {
        return arbitrary.sign();
    }

    bool Integer::operator <(const Valuable& v) const
    {
        if (v.IsInt())
            return arbitrary < v.ca();
        else if (v.IsFraction())
            return !(v.operator<(*this) || operator==(v));
        else if(v.IsMInfinity())
            return {};
        else if(v.IsInfinity())
            return true;
        else if (v.IsRational() == YesNoMaybe::Yes) 
            return arbitrary < static_cast<a_rational>(v);
        else if (v.Is_pi())
            return arbitrary < 4;
        else
            return v > *this;
    }

    bool Integer::operator ==(const int& i) const
    {
        return arbitrary == i;
    }

    bool Integer::operator ==(const a_int& v) const
    {
        return arbitrary == v;
    }

    bool Integer::operator ==(const Integer& v) const
    {
        return operator ==(v.ca());
    }

    bool Integer::operator ==(const Valuable& v) const
    {
        if (v.IsInt())
            return operator ==(v.as<Integer>());
        else if(v.FindVa() || v.IsConstant())
            return {};
        else
            return v.operator==(*this);
    }

    std::ostream& Integer::print(std::ostream& out) const
    {
        return out << arbitrary;
    }

    std::wostream& Integer::print(std::wostream& out) const
    {
        return out << arbitrary.str().c_str();
    }

    std::ostream& Integer::code(std::ostream& out) const {
        return print(out);
    }

    Valuable::universal_lambda_t Integer::CompileIntoLambda(variables_for_lambda_t variables) const {
        return [copy=Valuable(Clone())](universal_lambda_params_t) { return copy; };
    }

    Valuable Integer::calcFreeMember() const
    {
        return *this;
    }

    std::deque<Valuable> Integer::Divisors() const
    {
        std::deque<Valuable> f;
        Factorization([&](auto& v){
            f.push_back(v);
            return false;
        }, abs());
        return f;
    }

    Valuable& Integer::factorial() {
        if (arbitrary == 0)
            arbitrary = 1;
        else {
            for (decltype(arbitrary) i = boost::multiprecision::abs(arbitrary); i-->1 ;)
            {
                arbitrary *= i;
            }
        }
        hash = std::hash<base_int>()(arbitrary);
        return *this;
    }

    void Integer::gamma() { 
        --arbitrary;
        factorial();
    }

    Valuable& Integer::reciprocal() {
        if(!operator==(constants::one)) {
            Become(Fraction{constants::one, std::move(*this)});
        }
        return *this;
    }

    bool Integer::IsPrime() const
    {
        auto is = //boost::multiprecision::miller_rabin_test(boost::numeric_cast<uint64_t>(ca()), 3) &&
            !Factorization(
            [&](auto& v) {
                auto isPrimeFactor = v.IsZero() || v == constants::one || v == *this;
                auto stop = !isPrimeFactor;
                return stop;
            },
            abs());
        return is;
    }

    std::set<Valuable> Integer::SimpleFactsSet() const {
        std::set<Valuable> f;
        SimpleFactorization(
            [&](auto& v) {
                f.emplace(std::move(v));
                return false;
            },
            abs());
        f.erase(0);
        return f;
    }

    std::set<Valuable> Integer::FactSet() const {
        std::set<Valuable> f;
        Factorization(
            [&](auto& v) {
                f.emplace(std::move(v));
                return false;
            },
            abs());
        f.erase(0);
        return f;
    }

    Valuable Integer::FirstFactor() const {
        if (arbitrary <= 1)
            return arbitrary;

        for (base_int i = 2; i * i <= arbitrary; ++i) {
            if (arbitrary % i == 0) {
                return i;
            }
        }
        return arbitrary; // returns the number itself if it is prime
    }

    bool Integer::SimpleFactorization(const std::function<bool(const Valuable&)>& f, const Valuable& max,
                                const ranges_t& zz) const {
        auto h = arbitrary;
        if(h < 0)
            h = -h;
        if (f(0)) {
            return true;
        }
        bool scanz = zz.second.size();
        auto scanIt = zz.second.end();
        Valuable up = Integer(h);
        if (up > max) up = max;
        auto from = 1_v;
        if (scanz) {
            if (zz.first.second < up) {
                if(zz.first.second.IsInt())
                    up = zz.first.second;
                else
                    up = static_cast<int>(static_cast<double>(zz.first.second));
            }
            if (zz.first.first > from) {
                if(zz.first.first.IsInt())
                    from = zz.first.first;
                else
                    from = static_cast<int>(static_cast<double>(zz.first.first));
            }
            scanIt = zz.second.begin();
            while (scanIt != zz.second.end() && scanIt->second < from) {
                ++scanIt;
            }
        }
        auto absolute = abs();
        if(from==up)
            return f(up);
        else
            for (auto i = from; i < up; ++i) {
                auto a = absolute / i;
                if (a.IsInt()) {
                    if(f(i) || f(a))
                        return true;
                    if (a < up) {
                        up = a;
                    }
                }

                if (scanz && i > scanIt->second) {
                    ++scanIt;
                    if (scanIt == zz.second.end()) {
                        break;
                    }
                    i = scanIt->first;
                    if (!i.IsInt()) {
                        IMPLEMENT;
                    }
                }
            }
        return false;
    }

    namespace {
        StoringTasksQueue factorizationTasks;
    }
    bool Integer::Factorization(const std::function<bool(const Valuable&)>& f, const Valuable& max, const ranges_t& zz) const
    {   // check division by primes
        // iterate by primes to find factor and store it in factors set
        // togather with lowest factor found highest factor as result of division
        // the highest factor reduces the search range to its value exclusively (1)
        // after scan all the primes, need to scan permutations
        //
        // starting from smallest prime factor,
        //   new set of non-prime factors is enreached with muiltiple multiplications to current prime to self and to each member of the new set
        //   new set iterating starting from smallest too and until range out of upper bound (1)
        //  ...

        // Multi-threaded way:
        // for devisible of thouse: isDevisible = (ResultOfDivision = This / iPrime).IsInt():
        // add products of the Divisibles with factorization set numbers of result of division:
        // (add all [ResultOfDivision factors starting from iPrime inclusively] time iPrime)  recursively
        auto absolute = arbitrary;
        if (absolute < 0)
            absolute = -absolute;
        decltype(arbitrary) from = 2;
        if (zz.first.first > from) {
            if(zz.first.first.IsInt())
                from = zz.first.first.ca();
            else
                from = static_cast<int>(static_cast<double>(zz.first.first)); // FIXME: precision issues
        }
        else if (f(0_v) || f(1_v)) {
            return true;
        }
        bool scanz = zz.second.size();
        auto scanIt = zz.second.end();
        Valuable up(absolute);
        if (up > max) up = max;
        omnn::rt::prime_idx_t primeIdx = 0;
        if (zz.first.first < zz.first.second) {
            if (zz.first.second < up) {
                if (zz.first.second.IsInt())
                    up = zz.first.second;
                else
                    up = static_cast<int>(static_cast<double>(zz.first.second)); // FIXME: precision issues
            }
        } else {
            // assuming its current prime index stored
            primeIdx = boost::numeric_cast<decltype(primeIdx)>(zz.first.second.ca());
        }
        if (absolute <= up && f(absolute)) {
            return true;
        }
        if (scanz) {
            scanIt = zz.second.begin();
            while (scanIt != zz.second.end() && scanIt->second < from) {
                ++scanIt;
            }
        }
        if(from==up)
            return f(up);
        else {
            std::set<decltype(arbitrary)> primeFactors, nonPrimeFactors;
            auto maxPrimeIdx = omnn::rt::primes();
            auto& primeUpmost = omnn::rt::prime(maxPrimeIdx);
            primeIdx = omnn::rt::greatest_prime_idx(from);
            auto prime = omnn::rt::prime(primeIdx);
            if (prime != from) { // from is not a prime number
                for (auto i = from; i < prime; ++i) { // slow scan till first prime
                    if (absolute % i == 0) {
                        auto a = absolute;
                        a /= i;
                        if (f(i) || f(a))
                            return true;
                        if (a < up) {
                            up = a;
                        }
                        nonPrimeFactors.emplace(a);
                    }
                    if (scanz && i > scanIt->second) {
                        ++scanIt;
                        if (scanIt == zz.second.end()) {
                            break;
                        }
                        if (!scanIt->first.IsInt()) {
                            IMPLEMENT;
                        }
                        i = scanIt->first.ca();
                    }
                }
            }

            // iterating by primes
            auto primeScanUp = up;
            while (from <= primeUpmost
                && primeScanUp >= prime
				&& primeIdx < maxPrimeIdx)
            {
                if (absolute % prime == 0) {
                    auto a = absolute;
                    a /= prime;
                    if(prime > a)
                        break;
                    if (f(prime) || f(a))
                        return true;
                    primeFactors.emplace(prime);
                    nonPrimeFactors.emplace(a);
                    if (a < primeScanUp)
                        primeScanUp = a;
                }
                prime = omnn::rt::prime(++primeIdx);
            }

            if (primeIdx == maxPrimeIdx)
            {
#ifdef OPENMIND_PRIME_MINING
                static bool OutOfPrimesTableWarning = {};
                if (!OutOfPrimesTableWarning) {
                    std::cerr
                        << primeUpmost
                        << " is the biggest prime number in the hardcoaded primes table used for fast factorization to "
                           "solve equations using RRT. Consider extending primes table to make this perform much "
                           "quicker."
                        << std::endl;
                    OutOfPrimesTableWarning = true;
                    //rt::GrowPrime(absolute,
                    //    [](const decltype(absolute)& v) {
                    //        return Integer(v).IsPrime();
                    //    });
                }
#endif
                // Fallback algorithm
                for (auto i = from; i < up; ++i) {
                    if (absolute % i == 0) {
                        auto a = absolute;
                        a /= i;
                        if (f(i) || f(a))
                            return true;
                        if (a < up) {
                            up = a;
                        }
                    }

                    if (scanz && i > scanIt->second) {
                        ++scanIt;
                        if (scanIt == zz.second.end()) {
                            break;
                        }
                        if (!scanIt->first.IsInt()) {
                            IMPLEMENT;
                        }
                        i = scanIt->first.ca();
                    }
                }
                return false;
            }

            for (auto& prime : primeFactors) {
                auto mul = prime;
                mul *= prime;
                decltype(primeFactors) addNonPrime;
                while (mul <= up)
                {
                    if (absolute % mul == 0) {
                        auto a = absolute;
                        a /= mul;
                        if (nonPrimeFactors.find(mul) == nonPrimeFactors.end()
                            && addNonPrime.emplace(mul).second)
                        {
                            if (f(mul))
                                return true;
                            if (nonPrimeFactors.find(a) == nonPrimeFactors.end()
                                && addNonPrime.emplace(a).second) {
                                if (f(a))
                                    return true;
                            }
                        }
                    } else {
                        break;
                    }
                    mul *= prime;
                }

                for (auto& nonPrime : nonPrimeFactors) {
                    mul = nonPrime;
                    for (;;) {
                        mul *= prime;
                        if (mul > up)
                            break;
                        if (absolute % mul == 0) {
                            auto a = absolute;
                            a /= mul;
                            if (nonPrimeFactors.find(mul) == nonPrimeFactors.end() && addNonPrime.emplace(mul).second) {
                                if (f(mul))
                                    return true;
                                if (nonPrimeFactors.find(a) == nonPrimeFactors.end() && addNonPrime.emplace(a).second) {
                                    if (f(a))
                                        return true;
                                }
                            } else {
                                break;
                            }
                        } else {
                            break;
                        }
                    }
                }

                nonPrimeFactors.merge(std::move(addNonPrime));
            }

        }
        return false;
    }

    Valuable::solutions_t Integer::GetIntegerSolution(const Variable &va) const
    {
        return {};
    }

}
}

BOOST_CLASS_EXPORT(::omnn::math::Integer);
