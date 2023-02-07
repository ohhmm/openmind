//
// Created by Сергей Кривонос on 01.09.17.
//
#include "Valuable.h"

#include "e.h"
#include "i.h"
#include "Infinity.h"
#include "pi.h"
#include "Fraction.h"
#include "Integer.h"
#include "VarHost.h"
#include "Sum.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>
//#include <ranges>   uncomment once github action image is new enough
#include <sstream>
#include <string>
#include <string_view>
#include <stack>
#include <map>
#include <type_traits>

#ifndef NDEBUG
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#endif
#include <boost/core/demangle.hpp>
#include <boost/numeric/conversion/converter.hpp>
#ifndef __APPLE__
#include <boost/stacktrace.hpp>
#endif
#include <rt/GC.h>


#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

namespace omnn{
namespace math {
    const a_int Valuable::a_int_cz = 0;
    const max_exp_t Valuable::max_exp_cz(a_int_cz);

    namespace constants {
    constexpr const Valuable& e = constant::e;
    constexpr const Valuable& i = constant::i;
    constexpr const Valuable& zero = vo<0>();
    constexpr const Valuable& one = vo<1>();
    constexpr const Valuable& two = vo<2>();
#ifdef MSVC
    constexpr const Valuable& half = vf<.5>();
#else
    const Valuable& half = 1_v / 2;
#endif
    constexpr const Valuable& minus_1 = vo<-1>();
    const Valuable& plus_minus_1 = Exponentiation{1_v, 1_v / 2};          // ±1
    const Valuable& zero_or_1 = Sum{Exponentiation{1_v/4, 1_v/2}, 1_v/2}; // (1±1)/2
    constexpr const Valuable& pi = constant::pi;
    constexpr const Valuable& infinity = Infinity::GlobalObject;
    constexpr const Valuable& minfinity = MInfinity::GlobalObject;
    const Variable& integration_result_constant = "integration_result_constant"_va;

        std::map<std::string_view, Valuable> Constants ={
            {"e", constant::e},
            {"i", constant::i},
            {"pi", constant::pi},
        };
    } // namespace constants

    thread_local bool Valuable::optimizations = true;
    thread_local bool Valuable::bit_operation_optimizations = {};
    thread_local bool Valuable::enforce_solve_using_rational_root_test_only = {};


    Valuable implement(const char* str)
    {
        std::cerr << str << std::endl;
        throw std::string(str) + " Implement!";
    }

    bool Valuable::IsSubObject(const Valuable& o) const {
        if (exp)
            return exp->IsSubObject(o);
        else
            IMPLEMENT
    }

    const Valuable Valuable::Link() const {
        if(exp)
            return Valuable(exp);
        IMPLEMENT
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

    std::type_index Valuable::Type() const
    {
    	if (exp)
    		return exp->Type();
#ifdef __APPLE__
        LOG_AND_IMPLEMENT(" Implement Type() ");
#else
        LOG_AND_IMPLEMENT(" Implement Type() " << boost::stacktrace::stacktrace());
#endif
    }

    Valuable& Valuable::Become(Valuable&& i)
    {
        if (Same(i))
            return *this;
        auto newWasView = GetView(); // TODO: fix it, supervise all View usages
        i.SetView(newWasView);
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
                SetView(newWasView);
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
        if(GetView() != newWasView){
            SetView(newWasView);
            IMPLEMENT
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

    Valuable::Valuable(const Valuable& v) : exp(v.Clone()) {}
    Valuable::Valuable(Valuable* v) : exp(v) {}
    Valuable::Valuable(const encapsulated_instance& e) : exp(e) {}
    Valuable::Valuable(): exp(new Integer(Valuable::a_int_cz)) {}
    Valuable::Valuable(double d) : exp(new Fraction(d)) { exp->optimize(); }
    Valuable::Valuable(a_int&& i) : exp(new Integer(std::move(i))) {}
    Valuable::Valuable(const a_int& i) : exp(new Integer(i)) {}

    Valuable::Valuable(boost::rational<a_int>&& r) : exp(new Fraction(std::move(r))) { exp->optimize(); }

    Valuable::Valuable(const a_rational& r)
        : exp(new Fraction{::boost::multiprecision::numerator(r), boost::multiprecision::denominator(r)})
    {
        exp->optimize();
    }

    Valuable::Valuable(a_rational&& r) : exp(new Fraction(std::move(r))) { exp->optimize(); }

//    auto MergeOrF = x.Equals((Exponentiation((b ^ 2) - 4_v * a * c, 1_v/2)-b)/(a*2));
//    auto aMergeOrF = MergeOrF(a);
//    auto bMergeOrF = MergeOrF(b);
//    auto cMergeOrF = MergeOrF(c);
    Valuable Valuable::MergeOr(const Valuable& v1, const Valuable& v2)
    {
        Valuable merged;
        if(v1 == v2)
            merged = v1;
        else if (v1 == -v2)
            merged = v1 * constants::plus_minus_1;
        else {
            // a = 1;
            auto s = v1 + v2;
            // b = -s;
            if(s==0)
                merged = (!v1.IsProduct() ? v1 : v2) * constants::plus_minus_1;
            else{
                auto c = v1 * v2;
                auto d = s.Sq() - c*4;
                merged = (Exponentiation(d, constants::half) + s) / constants::two;
            }
        }
        return merged;
    }

	// FIXME : generates six distiinct results instead of expected three distinct-value equivalent
	Valuable Valuable::MergeOr(const Valuable& v1, const Valuable& v2, const Valuable& v3) {
        // 1,2,3:  1 + (1 or 2) * (1 or 0)   =>   1st + ((2nd or 3rd) - 1st) * (0 or 1)
        auto merged = MergeOr(v3-v1, v2-v1);
        merged *= constants::zero_or_1;
        merged += v1;
        return merged;
//        return MergeOr(merged, v3); // FIXME : 3-way emerge needs working implementation https://github.com/ohhmm/openmind/issues/41
    }

    Valuable Valuable::MergeOr(const Valuable& v1, const Valuable& v2, const Valuable& v3, const Valuable& v4) {
        auto _1 = MergeOr(v1, v2);
        auto _2 = MergeOr(v3, v4);
#ifndef NDEBUG
        if(_1.IsMultival() != YesNoMaybe::Yes || _2.IsMultival() != YesNoMaybe::Yes) {
            LOG_AND_IMPLEMENT(v1 << 'v' << v2 << 'v' << v3 << 'v' << v4 << " - emerging difficulty: "
                              << v1 << 'v' << v2 << '=' << _1 << ", " << v3 << 'v' << v4 << '=' << _2);
        }
#endif
        return MergeOr(_1, _2);
    }

    Valuable Valuable::MergeAnd(const Valuable& v1, const Valuable& v2)
    {
        return ((v1+v2)+(constants::minus_1^constants::half)*(v1-v2))/2;
    }

    Valuable::Valuable(solutions_t&& s)
    {
        auto it = s.begin();
        switch (s.size()) {
        case 0: IMPLEMENT; break;
        case 1: operator=(*it); break;
        case 2: {
            auto& _1 = *it++;
            auto& _2 = *it;
            operator=(MergeOr(_1, _2));
            break;
        }
        case 3: {
            auto& _1 = *it++;
            auto& _2 = *it++;
            auto& _3 = *it;
            operator=(MergeOr(_1, _2, _3));
            break;
        }
        case 4: {
            auto& _1 = *it++;
            auto& _2 = *it++;
            auto& _3 = *it++;
            auto& _4 = *it;
            operator=(MergeOr(_1, _2, _3, _4));
            break;
        }
        default:
            solutions_t pairs;
            for (; it != s.end();) {
                auto it2 = it;
                ++it2;
                auto neg = -*it;
                bool found = {};
                for (; it2 != s.end();) {
                    found = it2->operator==(neg);
                    if (found) {
                        pairs.emplace(MergeOr(*it, neg));
                        s.erase(it2);
                        s.erase(it++);
                        break;
                    } else {
                        ++it2;
                    }
                }
                if (!found) {
                    ++it;
                }
            }

            Valuable mergedPairs(std::move(pairs));

#ifndef NDEBUG
            std::stringstream ss;
            ss << '(';
            for (auto& v : s)
                ss << ' ' << v;
            ss << " )";
            std::cout << ss.str();
            LOG_AND_IMPLEMENT("Implement disjunctive merging algorithm for " << ss.str());
#else
            IMPLEMENT // implement MergeOr for three items and research if we could combine with case 2 for each couple in the set in paralell and then to the resulting set 'recoursively'
#endif
        }
        
#ifndef NDEBUG
        if(s.size() > 1){
            auto distinct = Distinct();
            if (distinct != s) {
                std::stringstream ss;
                ss << '(';
                for (auto& v : s)
                    ss << ' ' << v;
                ss << " ) <> (";
                for (auto& v : distinct)
                    ss << ' ' << v;
                ss << " )";
                std::cout << ss.str();
                LOG_AND_IMPLEMENT("Fix merge algorithm:" << ss.str());
            }
        }
#endif
    }

namespace{
auto BracketsMap(const std::string_view& s){
    auto l = s.length();
    using index_t = decltype(l);
    std::stack <index_t> st;
    std::map<index_t, index_t> bracketsmap;
    decltype(l) c = 0;
    while (c < l)
    {
        if (s[c] == '(')
            st.push(c);
        else if (s[c] == ')')
        {
            if (st.empty()) {
                throw "parentneses relation missmatch";
            }
            bracketsmap.emplace(st.top(), c);
            st.pop();
        }
        ++c;
    }
    if (!st.empty())
        throw "parentneses relation missmatch";
    return bracketsmap;
}

std::string_view& Trim(std::string_view& s) {
    s.remove_prefix(::std::min(s.find_first_not_of(" \t\r\v\n"), s.size()));
    s.remove_suffix((s.size() - 1) - ::std::min(s.find_last_not_of(" \t\r\v\n"), s.size() - 1));
    return s;
}

auto OmitOuterBrackets(std::string_view& s){
    decltype(BracketsMap({})) bracketsmap;
    bool outerBracketsDetected;
    do{
        outerBracketsDetected = {};
        Trim(s);
        bracketsmap = BracketsMap(s);
        auto l = s.length();
        auto first = bracketsmap.find(0);
        outerBracketsDetected = first != bracketsmap.end() && first->second == l-1;
        if (outerBracketsDetected)
            s = s.substr(1,l-2);
    } while(outerBracketsDetected);
    return bracketsmap;
}

std::string Spaceless(std::string s) {
    s.erase(std::remove(s.begin(), s.end(), ' '), s.end());
    return s;
}
}

    Valuable::Valuable(const std::string_view& s, std::shared_ptr<VarHost> h, bool itIsOptimized // = false
	) {
        auto l = s.length();
        using index_t = decltype(l);
        std::stack <index_t> st;
        std::map<index_t, index_t> bracketsmap;
        decltype(l) c = 0;
        while (c < l)
        {
            if (s[c] == '(')
                st.push(c);
            else if (s[c] == ')')
            {
                bracketsmap.emplace(st.top(), c);
                st.pop();
            }
            c++;
        }
        if (!st.empty())
            throw "parentneses relation missmatch";
        auto ok = bracketsmap.empty();
        if (ok)
        {
            auto copy = s;
            auto s = Trim(copy);
            auto offs = 0;
            while (s[offs]=='-')
                ++offs;
            auto found = s.find_first_not_of("0123456789", offs);
            if (found == std::string::npos)
            {
                exp = std::make_shared<Integer>(s);
            }
            else if (s[0] == 'v' && s.size() > 1 && h->IsIntegerId()
                && std::all_of(s.begin() + 1, s.end(),
					[](auto ch) { return std::isdigit(ch); }
					)
				)
                Become(Valuable(h->Host(Valuable(a_int(s.substr(1))))));
            else if (s.length() > 2
                && s[0] == '0'
                && (s[1] == 'x' || s[1] == 'X')
                && std::isxdigit(s[2])
                && s.find_first_not_of("0123456789ABCDEFabcdef", 2) == std::string::npos
                )
                Become(Integer(s));
            else if(std::all_of(s.begin(), s.end(), [](auto c){return std::isalnum(c);}))
                Become(Valuable(h->Host(s)));
            else
                ok = {};
        }
		
        if (!ok)
        {
            Valuable sum = Sum{};
            Valuable v;
            auto mulByNeg = false;
            using op_t = std::function<void(Valuable &&)>;
            op_t o_mov = [&](Valuable&& val) {
				v = std::move(val);
                if (mulByNeg) {
                    v *= -1;
                    mulByNeg = {};
                }
			};
            op_t o_sum, o_mul, o_div, o_exp;
            if (itIsOptimized) {
                o_sum = [&](Valuable&& val) {
                    if (val != 0) {
                        if (mulByNeg) {
                            val *= -1;
                            mulByNeg = {};
                        }

                        Sum s{std::move(sum), std::move(val)};
                        if(itIsOptimized)
                            s.MarkAsOptimized();
                        sum = std::move(s);
                    }
                };
                o_mul = [&](Valuable&& val) {
                    Product p{std::move(v)};
                    if (mulByNeg) {
                        p.Add(-1);
                        mulByNeg = {};
                    }
                    if(itIsOptimized)
                        p.MarkAsOptimized();
                    p.Add(std::move(val));
                    v = std::move(p);
                };
                o_div = [&](Valuable&& val) {
                    if (mulByNeg) {
                        val *= -1;
                        mulByNeg = {};
                    }
                    Fraction f{std::move(v), std::move(val)};
                    if(itIsOptimized)
                        f.MarkAsOptimized();
                    v = std::move(f);
                };
                o_exp = [&](Valuable&& val) {
                    if (mulByNeg) {
                        val *= -1;
                        mulByNeg = {};
                    }
                    Exponentiation e{std::move(v), std::move(val)};
                    if(itIsOptimized)
                        e.MarkAsOptimized();
                    v = std::move(e);
                };
            } else {
                o_sum = [&](Valuable&& val) {
                    if (mulByNeg) {
                        val *= -1;
                        mulByNeg = {};
                    }
                    sum += std::move(val);
                };
                o_mul = [&](Valuable&& val) {
                    if (mulByNeg) {
                        val *= -1;
                        mulByNeg = {};
                    }
					v *= std::move(val);
				};
                o_div = [&](Valuable&& val) {
                    if (mulByNeg) {
                        val *= -1;
                        mulByNeg = {};
                    }
                    v /= std::move(val);
                };
                o_exp = [&](Valuable&& val) {
                    if (mulByNeg) {
                        val *= -1;
                        mulByNeg = {};
                    }
                    v ^= std::move(val);
                };
            }

            auto o = std::ref(o_mov);
            //std::stack<char> op;
            for (index_t i = s.find_first_not_of(" \t\n\r"); i < l; ++i)
            {
                auto c = s[i];
                if (c == '(')
                {
                    auto cb = bracketsmap[i];
                    auto next = i + 1;
                    o(Valuable(s.substr(next, cb - next), h, itIsOptimized));
                    i = cb;
                }
                else if (c == 'v') {
                    auto idStart = i + 1;
                    auto next = s.find_first_not_of("0123456789", idStart);
                    auto id = s.substr(idStart, next - idStart);
                    if (id.empty()) {
                        auto to = s.find_first_of(" */+-^()", idStart);
                        auto id = to == std::string::npos ? s.substr(i) : s.substr(i, to - i);
                        o(Valuable(h->Host(id)));
                        i = to - 1;
                    } else if (h->IsIntegerId()){
                        o(Valuable(h->Host(Valuable(a_int(id)))));
                        i = next - 1;
                    } else {
                        id = s.substr(i, next - i);
                        o(Valuable(h->Host(id)));
                    }
                } else if (c == '-') {
                    o_sum(std::move(v));
                    v = 0;
                    o = o_mov;
                    mulByNeg ^= true;
                } else if ((c >= '0' && c <= '9')) {
                    auto next = s.find_first_not_of(" 0123456789", i+1);
                    auto ss = s.substr(i, next - i);
                    Trim(ss);
                    Valuable integer = ss.find(' ') == std::string::npos
						? Integer(ss)
						: Integer(Spaceless(std::string(ss)));
                    o(std::move(integer));
                    i = next - 1;
                }
                else if (c == '+') {
                    o_sum(std::move(v));
                    v = 0;
                    o = o_mov;
                }
                else if (c == '*') {
                    o = o_mul;
                    while (s[i + 1] == ' ')
                        ++i;
                    mulByNeg ^= s[i + 1] == '-';
                }
                else if (c == '/') {
                    o = o_div;
                    while (s[i + 1] == ' ')
                        ++i;
                }
                else if (c == '^') {
                    o = o_exp;
                }
                else if (c == ' ') {
                }
                else if (std::isalpha(c)){
                    auto to = s.find_first_of(" */+-^()", i+1);
                    auto id = to == std::string::npos ? s.substr(i) : s.substr(i, to - i);
                    Valuable val(h->Host(id));
                    if (mulByNeg) {
                        val *= -1;
                        mulByNeg = {};
                    }
                    o(std::move(val));
                    i = to - 1;
                } else {
                    IMPLEMENT
                }
            }

            o_sum(std::move(v));
            Become(std::move(sum));
        }

#if !defined(NDEBUG) && !defined(NOOMDEBUG)
        auto _ = str();
        auto same = s == _
            || (_.front() == '(' && _.back() == ')' && s == _.substr(1, _.length() - 2));
        if (!same) {
            auto _1 = Spaceless(str()), _2 = Spaceless(std::string(s));
            same = _1 == _2 || (_1.front() == '(' && _1.back() == ')' && _2 == _1.substr(1, _1.length() - 2));
            if (!same && IsInt() && s.length() > 2 && (s[1] == 'x' || s[1] == 'X')) {
                _2 = a_int(s).str();
                same = _1 == _2;
			}
            if (!same) {
#ifdef __APPLE__
                using split_t = std::set<std::string>;
#else
                using split_t = std::set<std::string_view>;
#endif
                std::set<split_t> sumOfProducts1, sumOfProducts2;
                // waiting for C++ ranges support:
                //for(auto& sumMember : words | std::ranges::views::split(plusDelimeter)){
                split_t sumItemsSubstrings;
                boost::split(sumItemsSubstrings, _1, [](auto c){return c=='+';});
//                for(auto& sumItemSubstring: sumItemsSubstrings){
//                    split_t productItemsSubstrings;
//                    boost::split(productItemsSubstrings, sumItemSubstring, [](auto c){return c=='*';});
//                    sumOfProducts1.emplace(productItemsSubstrings);
//                }
            }
            if (!same) {
                boost::replace_all(_1, "+-", "-");
                boost::replace_all(_2, "+-", "-");
                same = _1 == _2;
            }
            if (!same && _1.front() == '(' && _1.back() == ')' && (_2.front() != '(' || _2.back() != ')')) {
                _1 = _1.substr(1, _1.length() - 2);
                same = _1 == _2;
            }
            if (!same) {
                std::map<char, a_int> m1, m2;
                for (char c : _1)
                    if (c != '+')
                        ++m1[c];
                for (char c : _2)
                    if (c != '+')
                        ++m2[c];
                same = m1 == m2;
            }
            if (!same) {
                LOG_AND_IMPLEMENT("Deserialization check: "
                    << _ << " != " << s << std::endl
                    << std::endl
                    << _1 << "\n !=\n"
                    << _2 << std::endl
                    << " potential reasons:\n"
                        "  text expression ordering differs from this software expression building ordering"
                        "  var host type is changed between integer-type and string-type var host"
                        "  this software code changed expression building ordering or other changes that could be a cause for this deserialization check error message (optimization changes for example)"
                    );
			}
        }
#endif // !NDEBUG
    }

    Valuable::Valuable(const std::string& str, const Valuable::va_names_t& vaNames, bool itIsOptimized)
    :Valuable(std::string_view(str), vaNames, itIsOptimized)
    {
#if !defined(NDEBUG) && !defined(NOOMDEBUG)
#ifndef ALLOW_CACHE_UPGRADE
      auto _ = this->str();
//      if ((_.front() == '(' && _.back() == ')') && !(s.front() == '(' && s.back() == ')') )
//        _ = _.substr(1, _.length()-2);
//      _.erase(remove_if(_.begin(), _.end(), isspace), _.end());
      if (str != _)
        IMPLEMENT;
#endif
#endif
    }

    Valuable::Valuable(std::string_view s, const Valuable::va_names_t& vaNames, bool itIsOptimized){
		auto optimizationsWas = Valuable::optimizations;
		Valuable::optimizations = !itIsOptimized && optimizationsWas;

        auto bracketsmap = OmitOuterBrackets(s);

        //        if (bracketsmap.empty())
        {
            size_t search_start = 0;
            auto FindSkippingParentneses = [&](char symbol){
                // skip '(' for searching top level operations
                auto offs = search_start;
                char matches[] = { '(', symbol, 0 };
                while((offs = s.find_first_of(matches, offs)) != std::string_view::npos){
                    if(s[offs]=='('){
                        auto match = bracketsmap[offs];
                        if (match == s.length()-1 && offs==0) {
                            ++offs;
                            ++search_start;
                        } else
                            offs = match;
                    } else if (offs) {
                        break;
                    } else {
                        ++offs;
                    }
                }
                return offs;
            };

            std::string_view lpart, rpart;
            auto found = FindSkippingParentneses('+');
            if (found != std::string::npos) {
                lpart = s.substr(search_start, found-search_start);
                rpart = s.substr(found + 1, s.length() - (found + 1) - (search_start*2));
                auto l = Valuable(lpart, vaNames, itIsOptimized);
                if (itIsOptimized)
                    l.MarkAsOptimized();
                auto r = Valuable(rpart, vaNames, itIsOptimized);
                if (itIsOptimized)
                    r.MarkAsOptimized();
                auto sum = itIsOptimized ? Valuable(Sum{l, r}) : l + r;
                if (itIsOptimized)
                    sum.MarkAsOptimized();
                Become(std::move(sum));
            } else if ((found = FindSkippingParentneses('-')) != std::string::npos
                    && !Trim(lpart = s.substr(search_start, found - search_start)).empty()
                    && !lpart.ends_with("*/^"))
			{
                rpart = s.substr(found, s.length() - found - search_start*2);
                Valuable l(lpart, vaNames, itIsOptimized);
                Valuable r(rpart, vaNames, itIsOptimized);
                auto sum = itIsOptimized ? Valuable(Sum{l, r}) : l + r;
                if (itIsOptimized)
                    sum.MarkAsOptimized();
                Become(std::move(sum));
            } else if ((found = FindSkippingParentneses('*')) != std::string::npos) {
                lpart = s.substr(0, found);
                rpart = s.substr(found + 1, s.length() - found);
                auto l = Valuable(lpart, vaNames, itIsOptimized);
                if (itIsOptimized)
                    l.MarkAsOptimized();
                auto r = Valuable(rpart, vaNames, itIsOptimized);
                if (itIsOptimized)
                    r.MarkAsOptimized();
                auto product = itIsOptimized ? Valuable(Product{l, r}) : l * r;
                if (itIsOptimized)
                    product.MarkAsOptimized();
                Become(std::move(product));
            } else if ((found = FindSkippingParentneses('/')) != std::string::npos) {
                lpart = s.substr(0, found);
                rpart = s.substr(found + 1, s.length() - found);
                auto l = Valuable(lpart, vaNames, itIsOptimized);
                if (itIsOptimized)
                    l.MarkAsOptimized();
                auto r = Valuable(rpart, vaNames, itIsOptimized);
                if (itIsOptimized)
                    r.MarkAsOptimized();
                auto devided = itIsOptimized ? Valuable(Fraction{l, r}) : l / r;
                if (itIsOptimized)
                    devided.MarkAsOptimized();
                Become(std::move(devided));
            } else if((found = FindSkippingParentneses('^')) != std::string::npos) {
                lpart = s.substr(0, found);
                rpart = s.substr(found + 1, s.length() - found);
                auto l = Valuable(lpart, vaNames, itIsOptimized);
                if (itIsOptimized)
                    l.MarkAsOptimized();
                auto r = Valuable(rpart, vaNames, itIsOptimized);
                if (itIsOptimized)
                    r.MarkAsOptimized();
                auto exp = itIsOptimized ? Valuable(Exponentiation{l, r}) : l ^ r;
                if (itIsOptimized)
                    exp.MarkAsOptimized();
                Become(std::move(exp));
            }
            else
            {
                size_t offs = 0;
                while (s[offs] == ' ')
                    ++offs;
                if (s[offs] == '-')
                    ++offs;
                found = s.find_first_not_of(" 0123456789", offs);
                if (found == std::string::npos) {
                    Trim(s);
                    auto integer = s.find(' ') == std::string::npos
						? Integer(s)
						: Integer(Spaceless(std::string(s)));
                    Become(std::move(integer));
                }
                else
                {
                    auto it = vaNames.find(s);
                    if (it != vaNames.end()) {
                        Valuable v(it->second);
                        if (itIsOptimized)
                            v.MarkAsOptimized();
                        Become(std::move(v));
                    }
                    else {
                        auto it = constants::Constants.find(s);
                        if (it != constants::Constants.end()) {
                            Valuable v(it->second);
                            if (itIsOptimized)
                                v.MarkAsOptimized();
                            Become(std::move(v));
                        } else {
                            Become(Valuable(s, vaNames.begin()->second.getVaHost(), itIsOptimized));
                        }
                    }
                }
            }
        }

        Valuable::optimizations = optimizationsWas;
    }

    Valuable::~Valuable()
    {
#ifdef OPENMIND_BUILD_GC
        if (exp) {
			#if 0
            std::cout << *this << std::endl;
			#endif
            rt::GC::DispatchDispose(std::move(exp));
        }
#endif
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
        if (operator==(v))
        {
            sq();
        }
        else if (IsMultival() == YesNoMaybe::Yes && v.IsMultival() == YesNoMaybe::Yes)
        {
            solutions_t s;
            for (auto& m : Distinct())
                for (auto& item : v.Distinct())
                    s.emplace(m * item);
            Become(Valuable(std::move(s)));
        }
        else if (exp)
        {
            auto& o = exp->operator*=(v);
            if (o.exp) {
                exp = o.exp;
            }
        }
        else
            LOG_AND_IMPLEMENT(*this << " *= " << v);
        return *this;
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
        else {
            auto is = IsMultiplicationSimplifiable(v);
            if (is.first)
                Become(std::move(is.second));
            return is.first;
        }
    }

    std::pair<bool,Valuable> Valuable::IsMultiplicationSimplifiable(const Valuable& v) const
    {
        if(exp)
            return exp->IsMultiplicationSimplifiable(v);
        else {
            LOG_AND_IMPLEMENT(str() << "  *  " << v);
            auto m = *this * v;
            return { m.Complexity() < Complexity() + v.Complexity(), m };
        }
    }


    bool Valuable::SumIfSimplifiable(const Valuable& v)
    {
        if(exp)
            return exp->SumIfSimplifiable(v);
        IMPLEMENT
    }

    std::pair<bool,Valuable> Valuable::IsSummationSimplifiable(const Valuable& v) const
    {
        if(exp)
            return exp->IsSummationSimplifiable(v);
        else {
            LOG_AND_IMPLEMENT(str() << " IsSummationSimplifiable " << v);
            auto m = *this + v;
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

	Valuable Valuable::Integral(const Variable& x, const Variable& C // = constants::integration_result_constant
    ) const {
        auto t = *this;
        t.integral(x, C);
        return t;
    }

	void Valuable::integral(const Variable& x, const Valuable& from, const Valuable& to, const Variable& C) {
        if (exp) {
            exp->integral(x, from, to, C);
        } else
            IMPLEMENT
    }

    Valuable Valuable::Integral(const Variable& x,
		const Valuable& from // = constants::minfinity
		,
		const Valuable& to // = constants::infinity
		,
		const Variable& C // = constants::integration_result_constant
	) const {
        auto t = *this;
        t.integral(x, from, to, C);
        return t;
    }

	void Valuable::integral(const Variable& x, const Variable& C)
    {
        if(exp) {
            exp->integral(x, C);
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

    Valuable::solutions_t Valuable::Distinct() const
    {
        if(exp)
            return exp->Distinct();
        else if (IsSimple())
            return { *this };
        else
            LOG_AND_IMPLEMENT("Distinct " << str());
    }

    bool Valuable::IsUnivariable() const
    {
        std::set<Variable> vars;
        CollectVa(vars);
        return (vars.size() == 1);
    }

    Valuable Valuable::Univariate() const
    {
        if(exp)
            return exp->Univariate();
        else
            IMPLEMENT
    }

    bool Valuable::IsNormalizedPolynomial(const Variable& v) const {
        if(exp)
            return exp->IsNormalizedPolynomial(v);
        else
            IMPLEMENT
    }

    Valuable::solutions_t Valuable::Solutions() const
    {
        std::set<Variable> vars;
        CollectVa(vars);
        if (vars.size() == 1)
            return Solutions(*vars.begin());
        else
            IMPLEMENT
    }

    Valuable::solutions_t Valuable::Solutions(const Variable& v) const
    {
        solutions_t solutions;
        if(this->view == View::Solving || IsNormalizedPolynomial(v))
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
    using ranges_t = std::pair<zone_t/*whole*/,std::deque<zone_t>/*subranges*/>;

    ranges_t Valuable::get_zeros_zones(const Variable& v, solutions_t& some) const
    {
        auto fm = calcFreeMember().abs();
        ranges_t z {{-fm, fm},{}};

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

	Valuable Valuable::Sign() const {
        if (exp)
            return exp->Sign();
        else {
            LOG_AND_IMPLEMENT("Implement Sign determination for " << *this << ", sign(x) = (2/pi) * integral from 0 to +infinity of (sine(t*x)/t) dt");
        }
	}

    bool Valuable::operator<(const Valuable& v) const
    {
        if (exp)
            return exp->operator<(v);
        else if (!FindVa()) {
            double _1 = operator double();
            double _2 = static_cast<double>(v);
            if (_1 == _2) {
                LOG_AND_IMPLEMENT(*this << " looks optimizable");
            }
            return _1 < _2;
        } else {
            LOG_AND_IMPLEMENT(*this << " < " << v);
        }
    }

    bool Valuable::operator==(const Valuable& v) const
    {
        if(exp)
            return
// NO:                   Hash()==v.Hash() &&     // example: empty sum hash differs;  product 1*x*y == x*y ; etc
                    exp->operator==(v);
        else
            IMPLEMENT
    }

    bool Valuable::IsConstant() const { return exp && exp->IsConstant(); }
    bool Valuable::IsInt() const { return exp && exp->IsInt(); }
    bool Valuable::IsRadical() const { return exp && exp->IsRadical(); }
    bool Valuable::IsPrincipalSurd() const { return exp && exp->IsPrincipalSurd(); }
    bool Valuable::IsFraction() const { return exp && exp->IsFraction(); }
    bool Valuable::IsSimpleFraction() const { return exp && exp->IsSimpleFraction(); }
    bool Valuable::IsFormula() const { return exp && exp->IsFormula(); }
    bool Valuable::IsExponentiation() const { return exp && exp->IsExponentiation(); }
    bool Valuable::IsVa() const { return exp && exp->IsVa(); }
    bool Valuable::IsVaExp() const { return exp && exp->IsVaExp(); }
    bool Valuable::IsProduct() const { return exp && exp->IsProduct(); }
    bool Valuable::IsSum() const { return exp && exp->IsSum(); }
    bool Valuable::IsInfinity() const { return exp && exp->IsInfinity(); }
    bool Valuable::IsModulo() const { return exp && exp->IsModulo(); }
    bool Valuable::IsMInfinity() const { return exp && exp->IsMInfinity(); }
    bool Valuable::IsNaN() const { return exp && exp->IsNaN(); }
    bool Valuable::Is_e() const { return exp && exp->Is_e(); }
    bool Valuable::Is_i() const { return exp && exp->Is_i(); }
    bool Valuable::Is_pi() const { return exp && exp->Is_pi(); }

    bool Valuable::IsSimple() const {
        if(exp)
            return exp->IsSimple();
        else
            IMPLEMENT
    }

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
        else {
#ifdef __APPLE__
            LOG_AND_IMPLEMENT("Implement print(std::ostream&) for " << boost::core::demangle(Type().name()) << '\n');
#else
            LOG_AND_IMPLEMENT("Implement print(std::ostream&) for " << boost::core::demangle(Type().name()) << '\n'
                                                                    << boost::stacktrace::stacktrace());
#endif
        }
    }

    std::wostream& Valuable::print(std::wostream& out) const {
        if (exp)
            return exp->print(out);
        else
            IMPLEMENT
    }

    std::ostream& Valuable::code(std::ostream& out) const {
        if (exp)
            return exp->code(out);
        else
            IMPLEMENT
    }

    std::string Valuable::OpenCLuint() const {
        std::stringstream source;
        source << "__kernel void f(__global uint *Result";
        auto vars = Vars();
        for (auto& v : vars)
            if (v.str() != "i")
                source << ",__global uint *_" << v;
        source << "){const uint _i = get_global_id(0);";
        for (auto& v : vars) {
            source << "uint " << v << "=_" << v;
            if (v.str() != "i")
                source << "[_i]";
            source << ';';
        }
        source << "Result[_i]=";
        code(source);
        source << ";}";
        return source.str();
    }

    std::string Valuable::OpenCL() const {
        std::stringstream source;
        source << "__kernel void f(__global float *Result";
        auto vars = Vars();
        for(auto& v: vars)
            if (v.str() != "i")
                source << ",__global float *_" << v;
        source << "){const uint _i = get_global_id(0);";
        for(auto& v: vars) {
            source << "float " << v << "=_" << v;
            if (v.str() != "i")
                source << "[_i]";
            source << ';';
        }
        source << "Result[_i]=";
        code(source);
        source << ";}";
        return source.str();
    }

    std::ostream& operator<<(std::ostream& out, const Valuable& obj)
    {
        return obj.print(out);
    }

    std::wostream& operator<<(std::wostream& out, const Valuable& obj) { return obj.print(out); }

    std::ostream& operator<<(std::ostream& out, const Valuable::solutions_t& obj)
    {
        std::stringstream s;
        s << '<';
        for (auto& o : obj)
            s << o << '|';
        auto str = s.str();
        auto cstr = const_cast<char*>(str.c_str());
        cstr[str.size() - 1] = '>';
        out << cstr;
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

	Valuable Valuable::Cos() const {
		if (exp)
			return exp->Cos();
		else {
			return ((constant::e ^ Product{ constant::i, *this }) + (constant::e ^ Product{ -1_v, constant::i, *this })) / 2;
		}
	}

	Valuable Valuable::Sin() const {
		if (exp)
			return exp->Sin();
		else {
			static const Product _2i{ 2, constant::i };
			return ((constant::e ^ Product{ constant::i, *this }) - (constant::e ^ Product{ -1_v, constant::i, *this })) / _2i;
		}
	}

	Valuable Valuable::Sqrt() const {
        if(exp)
            return exp->Sqrt();
        else
            IMPLEMENT
    }

    Valuable& Valuable::sqrt() {
        return Become(Sqrt());
    }

	Valuable Valuable::Tg() const {
		if (exp)
			return exp->Tg();
		else {
			return Sin() / Cos();
		}
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
        t.sq();
        return t;
    }

	void Valuable::gamma() { // https://en.wikipedia.org/wiki/Gamma_function
        if (exp)
            exp->gamma();
        else {
            IMPLEMENT
            //Integral({});
        }
    }
	
	Valuable Valuable::Gamma() const {
        if (exp)
            return exp->Gamma();
        else {
            auto g = *this;
            g.gamma();
            return g;
        }
    }

	void Valuable::factorial() {
        if (exp)
            exp->factorial();
        else {
            operator++().gamma();
        }
    }
    
	Valuable Valuable::Factorial() const {
        if (exp)
            return exp->Factorial();
        else {
            auto f = *this;
            f.factorial();
            return f;
        }
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

    void Valuable::CollectVaNames(Valuable::va_names_t&  s) const
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
                Variable t(va.getVaHost());
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
            LOG_AND_IMPLEMENT("Implement " << boost::core::demangle(Type().name()) << "::InCommonWith() for " << *this
                                           << " InCommonWith " << v);
    }

    Valuable Valuable::varless() const
    {
        auto _ = exp ? exp->varless() : *this / getVaVal();
        if (_.FindVa()) {
            if (_.IsProduct()) {
                Product p;
                for(auto&& m: _.as<Product>()){
                    if (!m.FindVa()) {
                        p.Add(std::move(m));
                    }
                }
                _ = std::move(p);
            } else {
                LOG_AND_IMPLEMENT("Implement "<< boost::core::demangle(Type().name()) << "::varless() for " << *this
                    << "\n\t getVaVal() = " << getVaVal()
                    << "\n\t _ = " << _);
            }
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

    Valuable Valuable::operator!() const
    {
        // if current expression equals to zero then it is
        // we need to know why not
        Variable whyNot(getVaHost()); // whyNot==1 when this!=0

        auto is = LogicAnd(whyNot); // THIS==0 AND WHYNOT==0
        auto isNot = whyNot.Equals(1); // WHYNOT==1
        auto orNot = is.LogicOr(isNot).logic_and(isNot); // ((THIS==0 AND WHYNOT==0) OR (WHYNOT==1)) AND (WHYNOT==1)
        //std::cout << "orNot = " << orNot << std::endl;
        return orNot (whyNot); // try to express out the WHYNOT va and leave only f(x)==0
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
            LOG_AND_IMPLEMENT("Implement " << *this << " to double conversion");
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
        //return Equals(v) ^ -1;
		return IfEq(v, 1, 0);
    }

    // TODO: constexpr
    Valuable Valuable::Abet(const Variable& x, std::initializer_list<Valuable> l)
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

    //    distinctZeros
    //
    //    distinct intersect
    //
    //    _1 = (x-1)(x-2),   _2 =(x-2)(x-3)
    //
    //    distinct union of (x-1)(x-2), (x-2)(x-3)   ==  (x-1)(x-2)(x-3)
    //
    //    (x-1)(x-2)   (x-1)                         (x-1)
    //    ---------- = -----    =>  equation form :  ----- = 0    solve : x=1
    //    (x-2)(x-3)   (x-3)                         (x-3)
    //
    //    _1 / solve = intersect:
    //
    //    (x-1)(x-2) / (x-1) == (x-2)
    Valuable& Valuable::intersect(const Valuable& with, const Variable& va) {
        return Become(va - (logic_and(with)(va))); // logic_and(with)(va).equals(va)
    }
    Valuable Valuable::Intersect(const Valuable& with, const Variable& va) const {
        return va.Equals(LogicAnd(with)(va));
    }

    Valuable Valuable::Ifz(const Valuable& Then, const Valuable& Else) const
    {
        auto thisAndThen = LogicAnd(Then);
        return thisAndThen.LogicOr(thisAndThen.LogicOr(Else).LogicAnd(Else));
    }

    Valuable Valuable::IfNZ(const Valuable& Then, const Valuable& Else) const
    {
        return Ifz(Else, Then);
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

    Valuable Valuable::MustBeInt() const {
        using namespace constants;
        return
            //one.Equals(*this % one);
		    //or
            (e ^ (*this * 2 * i * pi)) - 1;        // FIXME: check if all required optimizations ready to make this useful https://math.stackexchange.com/a/1598552/118612
    }

    std::function<bool(std::initializer_list<Valuable>)> Valuable::Functor() const {
        if (exp)
            return exp->Functor();
        else
            IMPLEMENT
    }

    Valuable Valuable::bit(const Valuable& n) const
    {
        if (exp)
            return exp->bit(n);
        else if (n > 0) {
            return Shr().bit(n-1);
        } else if (n == 0){
            // bit0 = (this & 1) == (this % 2) == ((-1)^this)-1)/-2
            // for this=0: ((-1)^0)-1)/-2 = 0
            // this=1: ((-1^1)-1)/-2 = 1
            // this=2: ((-1^2)-1)/-2 = 0
            // ...
            // TODO: this=-1:
            // TODO: this=-2:
            // TODO: this=-3:
            // ...
            auto bit0 = (-1_v) ^ *this;
            --bit0;
            bit0 /= -2;
            return bit0;
        }else
            IMPLEMENT;
    }

    Valuable Valuable::bits(int n, int l) const
    {
        if(n<0)
            IMPLEMENT
        return Shr(n).And(l, -1);
    }

	namespace { // TODO: FIXME: uncomment & debug
		// n bits masking:
		//DECL_VA(x);
		//Valuable NbitSolidMasks[] = {
		//	0_v,
		//	"(((-1)/2)*((-1)^x) + ((-1)/(-2)))"_v,
		//	"(-1*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-1)/2)*((-1)^x) + (3/2)),"_v
		//	"(-1*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + -2*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-1)/2)*((-1)^x) + (7/2)),"_v
		//	"(-1*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + -2*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + -4*((-1)^((1/8)*x + (1/16)*((-1)^x) + (1/8)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/4)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-7)/16))) + ((-1)/2)*((-1)^x) + (15/2)),"_v
		//	"(-1*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + -2*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + -4*((-1)^((1/8)*x + (1/16)*((-1)^x) + (1/8)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/4)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-7)/16))) + -8*((-1)^((1/16)*x + (1/32)*((-1)^x) + (1/16)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/8)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + (1/4)*((-1)^((1/8)*x + (1/16)*((-1)^x) + (1/8)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/4)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-7)/16))) + ((-15)/32))) + ((-1)/2)*((-1)^x) + (31/2)),"_v
		//	"(-1*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + -2*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + -4*((-1)^((1/8)*x + (1/16)*((-1)^x) + (1/8)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/4)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-7)/16))) + -8*((-1)^((1/16)*x + (1/32)*((-1)^x) + (1/16)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/8)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + (1/4)*((-1)^((1/8)*x + (1/16)*((-1)^x) + (1/8)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/4)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-7)/16))) + ((-15)/32))) + -16*((-1)^((1/32)*x + (1/64)*((-1)^x) + (1/32)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/16)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + (1/8)*((-1)^((1/8)*x + (1/16)*((-1)^x) + (1/8)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/4)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-7)/16))) + (1/4)*((-1)^((1/16)*x + (1/32)*((-1)^x) + (1/16)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/8)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + (1/4)*((-1)^((1/8)*x + (1/16)*((-1)^x) + (1/8)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/4)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-7)/16))) + ((-15)/32))) + ((-31)/64))) + ((-1)/2)*((-1)^x) + (63/2)),"_v
		//	"(-1*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + -2*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + -4*((-1)^((1/8)*x + (1/16)*((-1)^x) + (1/8)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/4)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-7)/16))) + -8*((-1)^((1/16)*x + (1/32)*((-1)^x) + (1/16)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/8)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + (1/4)*((-1)^((1/8)*x + (1/16)*((-1)^x) + (1/8)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/4)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-7)/16))) + ((-15)/32))) + -16*((-1)^((1/32)*x + (1/64)*((-1)^x) + (1/32)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/16)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + (1/8)*((-1)^((1/8)*x + (1/16)*((-1)^x) + (1/8)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/4)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-7)/16))) + (1/4)*((-1)^((1/16)*x + (1/32)*((-1)^x) + (1/16)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/8)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + (1/4)*((-1)^((1/8)*x + (1/16)*((-1)^x) + (1/8)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/4)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-7)/16))) + ((-15)/32))) + ((-31)/64))) + -32*((-1)^((1/64)*x + (1/128)*((-1)^x) + (1/64)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/32)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + (1/16)*((-1)^((1/8)*x + (1/16)*((-1)^x) + (1/8)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/4)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-7)/16))) + (1/8)*((-1)^((1/16)*x + (1/32)*((-1)^x) + (1/16)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/8)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + (1/4)*((-1)^((1/8)*x + (1/16)*((-1)^x) + (1/8)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/4)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-7)/16))) + ((-15)/32))) + (1/4)*((-1)^((1/32)*x + (1/64)*((-1)^x) + (1/32)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/16)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + (1/8)*((-1)^((1/8)*x + (1/16)*((-1)^x) + (1/8)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/4)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-7)/16))) + (1/4)*((-1)^((1/16)*x + (1/32)*((-1)^x) + (1/16)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/8)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + (1/4)*((-1)^((1/8)*x + (1/16)*((-1)^x) + (1/8)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/4)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-7)/16))) + ((-15)/32))) + ((-31)/64))) + ((-63)/128))) + ((-1)/2)*((-1)^x) + (127/2)),"_v
		//	"(-1*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + -2*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + -4*((-1)^((1/8)*x + (1/16)*((-1)^x) + (1/8)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/4)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-7)/16))) + -8*((-1)^((1/16)*x + (1/32)*((-1)^x) + (1/16)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/8)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + (1/4)*((-1)^((1/8)*x + (1/16)*((-1)^x) + (1/8)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/4)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-7)/16))) + ((-15)/32))) + -16*((-1)^((1/32)*x + (1/64)*((-1)^x) + (1/32)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/16)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + (1/8)*((-1)^((1/8)*x + (1/16)*((-1)^x) + (1/8)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/4)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-7)/16))) + (1/4)*((-1)^((1/16)*x + (1/32)*((-1)^x) + (1/16)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/8)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + (1/4)*((-1)^((1/8)*x + (1/16)*((-1)^x) + (1/8)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/4)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-7)/16))) + ((-15)/32))) + ((-31)/64))) + -32*((-1)^((1/64)*x + (1/128)*((-1)^x) + (1/64)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/32)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + (1/16)*((-1)^((1/8)*x + (1/16)*((-1)^x) + (1/8)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/4)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-7)/16))) + (1/8)*((-1)^((1/16)*x + (1/32)*((-1)^x) + (1/16)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/8)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + (1/4)*((-1)^((1/8)*x + (1/16)*((-1)^x) + (1/8)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/4)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-7)/16))) + ((-15)/32))) + (1/4)*((-1)^((1/32)*x + (1/64)*((-1)^x) + (1/32)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/16)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + (1/8)*((-1)^((1/8)*x + (1/16)*((-1)^x) + (1/8)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/4)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-7)/16))) + (1/4)*((-1)^((1/16)*x + (1/32)*((-1)^x) + (1/16)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/8)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + (1/4)*((-1)^((1/8)*x + (1/16)*((-1)^x) + (1/8)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/4)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-7)/16))) + ((-15)/32))) + ((-31)/64))) + ((-63)/128))) + -64*((-1)^((1/128)*x + (1/256)*((-1)^x) + (1/128)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/64)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + (1/32)*((-1)^((1/8)*x + (1/16)*((-1)^x) + (1/8)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/4)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-7)/16))) + (1/16)*((-1)^((1/16)*x + (1/32)*((-1)^x) + (1/16)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/8)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + (1/4)*((-1)^((1/8)*x + (1/16)*((-1)^x) + (1/8)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/4)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-7)/16))) + ((-15)/32))) + (1/8)*((-1)^((1/32)*x + (1/64)*((-1)^x) + (1/32)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/16)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + (1/8)*((-1)^((1/8)*x + (1/16)*((-1)^x) + (1/8)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/4)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-7)/16))) + (1/4)*((-1)^((1/16)*x + (1/32)*((-1)^x) + (1/16)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/8)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + (1/4)*((-1)^((1/8)*x + (1/16)*((-1)^x) + (1/8)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/4)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-7)/16))) + ((-15)/32))) + ((-31)/64))) + (1/4)*((-1)^((1/64)*x + (1/128)*((-1)^x) + (1/64)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/32)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + (1/16)*((-1)^((1/8)*x + (1/16)*((-1)^x) + (1/8)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/4)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-7)/16))) + (1/8)*((-1)^((1/16)*x + (1/32)*((-1)^x) + (1/16)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/8)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + (1/4)*((-1)^((1/8)*x + (1/16)*((-1)^x) + (1/8)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/4)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-7)/16))) + ((-15)/32))) + (1/4)*((-1)^((1/32)*x + (1/64)*((-1)^x) + (1/32)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/16)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + (1/8)*((-1)^((1/8)*x + (1/16)*((-1)^x) + (1/8)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/4)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-7)/16))) + (1/4)*((-1)^((1/16)*x + (1/32)*((-1)^x) + (1/16)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/8)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + (1/4)*((-1)^((1/8)*x + (1/16)*((-1)^x) + (1/8)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + (1/4)*((-1)^((1/4)*x + (1/8)*((-1)^x) + (1/4)*((-1)^((1/2)*x + (1/4)*((-1)^x) + ((-1)/4))) + ((-3)/8))) + ((-7)/16))) + ((-15)/32))) + ((-31)/64))) + ((-63)/128))) + ((-127)/256))) + ((-1)/2)*((-1)^x) + (255/2))"_v
		//};
	}
    Valuable Valuable::And(const Valuable& n, const Valuable& v) const
    {
        auto s = 0_v;
        {
            OptimizeOff oo;
            auto a = *this;
            auto b = v;
            for (Valuable i; i < n;) {
                auto ab = a.bit(0);
                auto bb = b.bit(0);
                auto mul = ab * bb;
                s += mul * (2_v ^ i);
                if (++i < n) {
                    a -= ab;
                    a /= 2;
                    b -= bb;
                    b /= 2;
                }
            }
        }
        if (bit_operation_optimizations) {
            s.optimize();
        }
        return s;
    }

    Valuable Valuable::Or(const Valuable& n, const Valuable& v) const
    {
        auto s = 0_v;
        {
            OptimizeOff oo;
            auto a = *this;
            auto b = v;
            for (Valuable i; i < n;) {
                auto ab = a.bit(0);
                auto bb = b.bit(0);
                auto mul = ab * bb;
                auto sum = ab + bb;
                s += (sum - mul) * (2_v ^ i);
                if (++i < n) {
                    a -= ab;
                    a /= 2;
                    b -= bb;
                    b /= 2;
                }
            }
        }
        if (bit_operation_optimizations) {
            s.optimize();
        }
        return s;
    }

    Valuable Valuable::Xor(const Valuable& n, const Valuable& v) const
    {
        auto s = 0_v;
        {
            OptimizeOff oo;
            for (auto i = n; i--;) {
                s *= 2;
                auto _1 = bit(i);
                auto _2 = v.bit(i);
                s += (_1 + _2).bit(0);
            }
        }
        if (bit_operation_optimizations) {
            s.optimize();
        }
        return s;
    }

    Valuable Valuable::Not(const Valuable& n) const
    {
        auto s = 0_v;
        {
            OptimizeOff oo;
            for (auto i = n; i--;) {
                s *= 2;
                auto _1 = 1 - bit(i);
                s += (_1).bit(0);
            }
        }
        if (bit_operation_optimizations) {
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
        {
            OptimizeOff oo;
            for (auto i = total; i--;) {
                auto shi = i + shiftLeft;
                if (shi >= total)
                    shi -= total;
                else if (shi < 0)
                    shi += total;
                auto _1 = bit(i);
                s += bit(i) * (2 ^ (shi));
            }
        }
        if (bit_operation_optimizations) {
            s.optimize();
        }
        return s;
    }

    auto sh_const =   "0x428a2f9871374491b5c0fbcfe9b5dba5" //  0
                        "3956c25b59f111f1923f82a4ab1c5ed5"
                        "d807aa9812835b01243185be550c7dc3" //  8
                        "72be5d7480deb1fe9bdc06a7c19bf174"
                        "e49b69c1efbe47860fc19dc6240ca1cc" // 16
                        "2de92c6f4a7484aa5cb0a9dc76f988da"
                        "983e5152a831c66db00327c8bf597fc7" // 24
                        "c6e00bf3d5a7914706ca635114292967"
                        "27b70a852e1b21384d2c6dfc53380d13" // 32
                        "650a7354766a0abb81c2c92e92722c85"
                        "a2bfe8a1a81a664bc24b8b70c76c51a3" // 40
                        "d192e819d6990624f40e3585106aa070"
                        "19a4c1161e376c082748774c34b0bcb5" // 48
                        "391c0cb34ed8aa4a5b9cca4f682e6ff3"
                        "748f82ee78a5636f84c878148cc70208" // 56
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
    }

    size_t Valuable::Hash() const
    {
        return exp
            ? exp->Hash()
            : hash;
    }

    void Valuable::MarkAsOptimized() {
        if (exp)
            exp->MarkAsOptimized();
        else
            optimized = true;
    }

    bool Valuable::is_optimized() const {
        return exp
            ? exp->is_optimized()
            : optimized;
    }

    std::string Valuable::str() const
    {
        std::stringstream s;
        print(s);
        return s.str();
    }

    std::wstring Valuable::wstr() const {
        std::wstringstream s;
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

    size_t hash_value(const Valuable& v) { return v.Hash(); }
    size_t hash_value(const Sum& v) { return v.Hash(); }

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

::omnn::math::Valuable operator"" _v(const char* v, std::size_t l)
{
    static auto StrVaHost = ::omnn::math::VarHost::Global<std::string>().shared_from_this();
    return {{v, l}, StrVaHost, true};
}

const ::omnn::math::Variable& operator"" _va(const char* v, std::size_t l)
{
    return ::omnn::math::VarHost::Global<std::string>().Host(std::string_view(v, l));
}

//APPLE_CONSTEXPR
const boost::multiprecision::cpp_int ull2cppint(unsigned long long v) {
    return v;
}

::omnn::math::Valuable operator"" _v(unsigned long long v)
{
    using namespace ::omnn::math;
    const auto va = ull2cppint(v);
    return Valuable(Integer(va));
}

//constexpr const ::omnn::math::Valuable& operator"" _const(unsigned long long v)
//{
//    return ::omnn::math::vo<v>();
//}

::omnn::math::Valuable operator"" _v(long double v)
{
    return ::omnn::math::Fraction(boost::multiprecision::cpp_dec_float_100(v));
}
