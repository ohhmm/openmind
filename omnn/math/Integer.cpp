//
// Created by Сергей Кривонос on 01.09.17.
//
#include "Integer.h"

#include "i.h"
#include "Exponentiation.h"
#include "Fraction.h"
#include "Modulo.h"
#include "Product.h"
#include "Sum.h"

#include <algorithm>
#include <codecvt>
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
#include <boost/functional/hash.hpp>
#include <boost/numeric/conversion/converter.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/detail/integer_ops.hpp>
#include <boost/multiprecision/integer.hpp>
//#include <libiomp/omp.h>

using boost::multiprecision::cpp_int;

namespace omnn{
namespace math {

    const Integer::zero_zone_t Integer::empty_zero_zone;

    Integer::Integer(const Fraction& f)
    : arbitrary(Integer::cast(f.getNumerator())->arbitrary / Integer::cast(f.getDenominator())->arbitrary)
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
    
    Valuable::YesNoMaybe Integer::IsEven() const {
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
            if (is.second.Complexity() > v.Complexity())
                IMPLEMENT;
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
            auto s = v.IsSumationSimplifiable(*this);
            is = s.first;
            if(is)
                Become(std::move(s.second));
        }
        return is;
    }

    std::pair<bool,Valuable> Integer::IsSumationSimplifiable(const Valuable& v) const
    {
        std::pair<bool,Valuable> is;
        is.first = v.IsInt();
        if (is.first) {
            is.second = v + *this;
        } else if (v.IsVa() || v.IsExponentiation()) {
        } else if (v.IsProduct()) {
        } else {
            is = v.IsSumationSimplifiable(*this);
            if (is.second.Complexity() > v.Complexity())
                IMPLEMENT;
        }
        return is;
    }

    Valuable& Integer::operator /=(const Valuable& v)
    {
        if (v.IsInt())
        {
            auto div = arbitrary / v.ca();
            if (div*v.ca() == arbitrary)
            {
                arbitrary = div;
                hash = std::hash<base_int>()(arbitrary);
            }
            else
                Become(Fraction(*this, v));
        }
        else if(v.FindVa())
			*this *= v^-1;
        else
            Become(Fraction(*this,v));
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

    Valuable& Integer::operator --()
    {
        arbitrary--;
        hash = std::hash<base_int>()(arbitrary);
        return *this;
    }

    Valuable& Integer::operator ++()
    {
        arbitrary++;
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
                }
                IMPLEMENT
            }
            unsigned N = static_cast<unsigned>(n);
            return static_cast<int>(bit_test(arbitrary, N));
        }
        else
            IMPLEMENT;
    }
    
    Valuable& Integer::shl(const Valuable& n)
    {
        if (n.IsInt())
            arbitrary = arbitrary << static_cast<int>(n);
        else
            base::shl(n);
        return *this;
    }

    Valuable& Integer::shr(const Valuable& n)
    {
        if (n.IsInt())
            arbitrary = arbitrary >> static_cast<int>(n);
        else
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
    
    std::pair<Valuable,Valuable> Integer::GreatestCommonExp(const Valuable &e) const {
        // test : 50_v.GCE(2) == 5_v
        //        32_v.GCE(2) == 4_v
        //  which is needed to be powered into 2 to get gretest devisor that may be powered into 2 to get devisor of the initial value
        if(e==1_v)
            return {*this,*this};

        auto xFactors = Facts();
        std::sort(xFactors.begin(), xFactors.end());
        while(xFactors.size() > 1) {
            auto&& xFactor = std::move(xFactors.back());
            if(xFactor > 1_v) {
                if(e == 2_v){
                    Valuable v = boost::multiprecision::sqrt(xFactor.ca());
                    if((v^e) == xFactor)
                        return {v,xFactor};
                }else{
                    IMPLEMENT
//                    auto v = boost::multiprecision::pow(xFactor, 1/e);
                }
            }
            xFactors.pop_back();
        }
        return {1_v,1_v};
    }

    Valuable& Integer::operator^=(const Valuable& v)
    {
        if(arbitrary == 0 || (arbitrary == 1 && v.IsInt()))
        {
            if (v == 0) {
                IMPLEMENT; //NaN
            }
            return *this;
        } else if ((arbitrary == 1 || arbitrary == -1) && v.IsSimpleFraction()) {
            auto f = Fraction::cast(v);
            if(!f->getDenominator().IsEven())
                return *this;
            else
                return Become(Exponentiation{*this,v});
        }
        if(v.IsInt())
        {
            if (v != 0_v) {
                if (v > 1) {
                    Valuable x = *this;
                    Valuable n = v;
                    if (n < 0_v)
                    {
                        x = 1_v / x;
                        n = -n;
                    }
                    if (n == 0_v)
                    {
                        arbitrary = 1;
                        hash = std::hash<base_int>()(arbitrary);
                        return *this;
                    }
                    auto y = 1_v;
                    while(n > 1)
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
            auto f = Fraction::cast(v);
            auto& nu = f->getNumerator();
            Valuable mn;
            auto nlz = nu < 0;
            if(nlz)
                mn = -nu;
            auto& n = nlz ? mn : nu;
            auto dn = nlz ? -f->getDenominator() : f->getDenominator();

            auto numeratorIsOne = n == 1_v;
            if (!numeratorIsOne)
                *this ^= n;

            auto isNeg = operator<(0_v);
            auto signs = 0; //dimmensions
            while(dn.IsEven() == YesNoMaybe::Yes) {
                auto minus = arbitrary < 0;
                auto _ = boost::multiprecision::sqrt(minus ? -arbitrary : arbitrary);
                auto _sq = _ * _;
                if (_sq == boost::multiprecision::abs(arbitrary)){
                    arbitrary = _;
    //                Become(isNeg ? -operator-().Sqrt() : Sqrt());
                    dn /= 2;
                    ++signs;
                } else {
//                    IMPLEMENT
                    if(n!=1_v)
                        IMPLEMENT;
                    auto gce = GreatestCommonExp(dn);
                    return Become(gce.first*Exponentiation{operator/=(gce.second),n/dn});
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
                    if(x<0_v){
                        Valuable exp;
                        if (!numeratorIsOne) {
                            *this ^= n;
                            exp = 1_v / dn;
                        }
                        auto& exponentiating = numeratorIsOne ? v : exp;
                        auto xFactors = Facts();
                        std::sort(xFactors.begin(), xFactors.end());
                        while(xFactors.size() > 1) {
                            auto xFactor = std::move(xFactors.back());
                            if(xFactor > 1_v /* && !operator==(xFactor) */){
                                auto e = xFactor ^ dn;
                                if(operator==(e))
                                    return Become(e*(1_v^exponentiating));
                                auto f = xFactor ^ exponentiating;
                                if(!f.IsInt())
                                    f /= 1_v ^ exponentiating;
                                if(f.IsInt())
                                    return Become(f*((x/xFactor)^exponentiating));
                            }
                            xFactors.pop_back();
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
                        nroot = Exponentiation(*this, 1_v/dn);
                        break;
                    }
                    // *this ^ 1/dn  == (nroot^dn + t)^ 1/dn
                    // this == nroot^dn +
                    // TODO : IMPLEMENT//return Become(Sum {nroot, (*this-(nroot^dn))^(1/dn)});
                }
                return Become(std::move(nroot));
            }
            if(dnSubZ)
                Become(1_v / *this);
            if(signs) {
                return operator*=((isNeg?-1_v:1_v)^(1_v/(1<<signs)));
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
        return *this;
    }
    
    bool Integer::IsComesBefore(const Valuable& v) const
    {
        return v.IsInt() && *this > v;
    }
    
    Valuable Integer::InCommonWith(const Valuable& v) const
    {
        return (v.IsInt() && v!=0_v && *this!=0_v) ? boost::gcd(v.ca(),ca()) : 1_v;
    }

    // sqrt using boost
    Valuable Integer::Sqrt() const
    {
        auto minus = arbitrary < 0;
        auto _ = boost::multiprecision::sqrt(minus ? -arbitrary : arbitrary);
        auto _sq = _ * _;
        if (_sq != boost::multiprecision::abs(arbitrary)){
            auto d = GreatestCommonExp(2);
            if(d.second != 1)
                return (*this / d.second).Sqrt() * d.first;
        }
        return minus ? constant::i * _ : _;
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
        using namespace std;
        using convert_typeX = codecvt_utf8<wchar_t>;
        std::wstring_convert<convert_typeX, wchar_t> c;
        std::ofstream s(c.to_bytes(f));
        boost::archive::binary_oarchive a(s);
        a & arbitrary;
        return f;
    }
    
    bool Integer::operator <(const Valuable& v) const
    {
        if (v.IsInt())
            return arbitrary < v.ca();
        else if (v.IsFraction())
            return !(v < *this) && *this!=v;
        else if(v.IsMInfinity())
            return {};
        else if(v.IsInfinity())
            return true;
        else if (!v.FindVa()) {
            double _1 = boost::numeric_cast<double>(arbitrary);
            double _2 = static_cast<double>(v);
            if(_1 == _2) {
                IMPLEMENT
            }
            return _1 < _2;
       } else
            return base::operator <(v);
    }

    bool Integer::operator ==(const Valuable& v) const
    {
        if (v.IsInt())
            return Hash() == v.Hash() && arbitrary == v.ca();
        else if(v.FindVa())
            return false;
        else
            return v == *this;
    }

    std::ostream& Integer::print(std::ostream& out) const
    {
        return out << arbitrary;
    }
    
    Valuable Integer::calcFreeMember() const
    {
        return *this;
    }

    std::deque<Valuable> Integer::Facts() const
    {
        std::deque<Valuable> f;
        Factorization([&](auto& v){
            f.push_back(v);
            return false;
        }, abs());
        return f;
    }

    bool Integer::Factorization(const std::function<bool(const Valuable&)>& f, const Valuable& max, const zero_zone_t& zz) const
    {
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
//        if (!zz.second.empty()) {
//            IMPLEMENT
//        }
//        if (arbitrary > std::numeric_limits<cl_long>::max()) {
//        #pragma omp parallel for shared(up)
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
////            while (scai > scanIt.s) {
////                <#statements#>
////            }
        }
//        }
//        else
//        {
//            // build OpenCL kernel
//            using namespace boost::compute;
//            auto copy = *this;
//            copy.optimize();
//            std::stringstream source;
//            source << "__kernel void f(__global long16 a, __global long16 *c) {"
//                << "    const uint i = get_global_id(0);"
//                << "    c[i] = a/i;"
//                << ";}";
//
//            device cuwinner = system::default_device();
//            for(auto& p: system::platforms())
//                for(auto& d: p.devices())
//                    if (d.compute_units() > cuwinner.compute_units())
//                        cuwinner = d;
//            auto wgsz = cuwinner.max_work_group_size();
//            context context(cuwinner);
//
//            kernel k(program::build_with_source(source.str(), context), "f");
//            auto sz = wgsz * sizeof(cl_long16);
//            buffer c(context, sz);
//            k.set_arg(0, c);
//
//            command_queue queue(context, cuwinner);
//            // run the add kernel
//            queue.enqueue_1d_range_kernel(k, 0, wgsz, 0);
//
//            // transfer results to the host array 'c'
//            std::vector<cl_long> z(wgsz);
//            queue.enqueue_read_buffer(c, 0, sz, &z[0]);
//            queue.finish();
        
//        }

        
//#pragma omp for
        return false;
    }
}}

