//
// Created by Сергей Кривонос on 01.09.17.
//
#include "Valuable.h"

#include "Euler.h"
#include "i.h"
#include "Infinity.h"
#include "NaN.h"
#include "pi.h"
#include "Fraction.h"
#include "Modulo.h"
#include "Integer.h"
#include "VarHost.h"
#include "Sum.h"
#include "PrincipalSurd.h"
#include "StateProxyComparator.h"
#include "Limit.h"
#include "Logarithm.h"

#include <omnn/rt/GC.h>
#include <omnn/rt/strhash.hpp>
#include <omnn/rt/tasq.h>

#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <numeric>
#include <regex>
#include <sstream>
#include <stack>
#include <type_traits>

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/core/demangle.hpp>
#include <boost/numeric/conversion/converter.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#ifndef __APPLE__
#include <boost/stacktrace.hpp>
#endif

using namespace ::omnn::rt;
using namespace ::std::string_view_literals;

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

namespace omnn::math {
    std::ostream& operator<<(std::ostream& out, const vars_cont_t& vars) {
        out << "{";
        bool first = true;
        for (const auto& [var, val] : vars) {
            if (!first) out << ", ";
            out << var << ": " << val;
            first = false;
        }
        out << "}";
        return out;
    }

    const a_int Valuable::a_int_cz = 0;
    const max_exp_t Valuable::max_exp_cz(a_int_cz);

    namespace constants {
    constexpr const Valuable& e = constant::e;
    constexpr const Valuable& i = constant::i;
    constexpr const Valuable& zero = vo<0>();
    constexpr const Valuable& one = vo<1>();
    constexpr const Valuable& two = vo<2>();
    const Fraction Half{1_v, 2_v};
    constexpr const Valuable& half = Half;
    const Fraction Quarter {1, 4};
    constexpr const Valuable& quarter = Quarter;
    constexpr const Valuable& minus_1 = vo<-1>();

    const Exponentiation PlusMinusOne(1, Fraction{1_v, 2_v}); // ±1
    constexpr const Valuable& plus_minus_1 = PlusMinusOne;                                                  // ±1
    const auto ZeroOrOne = Sum{Exponentiation{Fraction{1_v, 4_v}, Fraction{1_v, 2_v}}, Fraction{1_v, 2_v}}; // (1±1)/2
    const Valuable& zero_or_1 = ZeroOrOne; // (1±1)/2
    constexpr const Valuable& pi = constant::pi;
    constexpr const Valuable& infinity = Infinity::GlobalObject;
    constexpr const Valuable& minfinity = MInfinity::GlobalObject;
    const Variable& integration_result_constant = "integration_result_constant"_va;

    const std::map<std::string_view, Valuable> Constants = {
        {Euler::SerializationName, constant::e},
        {MinusOneSurd::SerializationName, constant::i},
        {Pi::SerializationName, constant::pi},
        {NaN::SerializationName, constant::nan},
    };
    } // namespace constants

    thread_local bool Valuable::optimizations = true;
    thread_local bool Valuable::bit_operation_optimizations = {};
    thread_local bool Valuable::enforce_solve_using_rational_root_test_only = {};

//    [[noreturn]] 'return' statement is useful for debugging purposes here
    Valuable implement(const char* str)
    {
        std::cerr << str << std::endl;
        throw std::string(str) + " Implement!";
        return {};
    }

    Valuable& Valuable::call_polymorphic_method(Valuable::method_t method, const Valuable& arg) {
        if (exp) {
            auto view = GetView();
            auto equation = IsEquation();
            if (equation) {
                SetView(View::None);
            }
            auto& obj = ((*exp).*method)(arg);
            if (equation) {
                obj.SetView(view);
            }
            if (obj.exp) {
                auto dispose = std::move(exp);
                exp = std::move(obj.exp);
                DispatchDispose(std::move(dispose));
            }
            if (exp->getAllocSize() <= getAllocSize()) {
                Become(std::move(*exp));
            } else if (equation) {
                optimize();
            }
            return *this;
        } else {
            LOG_AND_IMPLEMENT(typeid(method).name() << " for " << *this);
        }
    }

    #define VALUABLE_POLYMORPHIC_METHOD(method)                                                                        \
        Valuable& Valuable::method(const Valuable& value) {                                                            \
            if(exp) {                                                                                                  \
                return call_polymorphic_method(&Valuable::method, value);                                              \
            } else {                                                                                                   \
                LOG_AND_IMPLEMENT(#method " for " << *this);                                                           \
            }                                                                                                          \
        }

    bool Valuable::IsSubObject(const Valuable& o) const {
        if (exp)
            return exp->IsSubObject(o);
        else
            LOG_AND_IMPLEMENT("IsSubObject for " << *this)
    }

    const Valuable Valuable::Link() const {
        if(exp)
            return Valuable(exp);
        LOG_AND_IMPLEMENT("Link for " << *this)
    }

    Valuable* Valuable::Clone() const
    {
        if (exp)
            return exp->Clone();
        else
            LOG_AND_IMPLEMENT("Clone for " << *this)
    }

    Valuable* Valuable::Move()
    {
        if (exp)
            return exp->Move();
        else
            LOG_AND_IMPLEMENT("Move for " << *this)
    }

    void Valuable::New(void*, Valuable&&)
    {
        LOG_AND_IMPLEMENT("New for " << *this)
    }

    Valuable::encapsulated_instance Valuable::SharedFromThis() {
        if (exp)
            return exp;
        else
            LOG_AND_IMPLEMENT("SharedFromThis for " << *this);
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

    void Valuable::DispatchDispose(encapsulated_instance&& e) {
#ifdef OPENMIND_BUILD_GC
        if (e) {
            rt::GC::DispatchDispose(std::move(e));
        }
#endif
    }

    Valuable& Valuable::Become(Valuable&& i)
    {
        if (Same(i))
            return *this;
        auto newWasView = this->GetView();
        i.SetView(newWasView);
        auto h = i.Hash();
        if (i.exp)
        {
            auto impl = std::move(i.exp);
            while (impl->exp) {
                impl = std::move(impl->exp);
            }

            auto isEncapsulatedInstance = Is<Valuable>();
            if (exp
                || isEncapsulatedInstance
                || impl->getTypeSize() > getAllocSize()
                )
            {
                if (isEncapsulatedInstance) {
                    DispatchDispose(std::move(exp));
                    exp = std::move(impl);
                } else {
                    auto allocSize = getAllocSize();
                    this->~Valuable();
                    new (this) Valuable(std::move(impl));
                    setAllocSize(allocSize);
                }
                if (Hash() != h) {
                    LOG_AND_IMPLEMENT("Hash mismatch in Become for " << *this)
                }
            }
            else
            {
                Become(std::move(*impl));
            }
        }
        else
        {
            auto sizeWas = getAllocSize();
            auto newSize = i.getTypeSize();

            if (newSize <= sizeWas) {
                assert(DefaultAllocSize >= newSize && "Increase DefaultAllocSize");
                char buf[DefaultAllocSize];
                i.New(buf, std::move(i));
                Valuable& bufv = *reinterpret_cast<Valuable*>(buf);
                this->~Valuable();
                bufv.New(this, std::move(bufv));
                setAllocSize(sizeWas);
                if (Hash() != h) {
                    LOG_AND_IMPLEMENT("Hash mismatch in Become for " << *this)
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
                    LOG_AND_IMPLEMENT("Hash mismatch in Become for " << *this)
                }
                optimize();
            }
        }
        if(GetView() != newWasView){
            SetView(newWasView);
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
    Valuable::Valuable(encapsulated_instance&& enc) : exp(enc) {}
    Valuable::Valuable(const encapsulated_instance& enc) : exp(enc) {}
    Valuable::Valuable(): exp(new Integer(Valuable::a_int_cz)) {}
    Valuable::Valuable(double d) : exp(new Fraction(d)) { exp->optimize(); }
    Valuable::Valuable(a_int&& i) : exp(std::move(std::make_shared<Integer>(std::move(i)))) {}
    Valuable::Valuable(const a_int& i) : exp(new Integer(i)) {}

    Valuable::Valuable(const a_rational& r)
    : exp(std::move(std::make_shared<Fraction>(r)))
    {
        exp->optimize();
    }

    Valuable::Valuable(a_rational&& r)
    : exp(std::move(std::make_shared<Fraction>(std::move(r))))
    { exp->optimize(); }

    namespace{
        template<typename T>
        constexpr T bits_in_use(T v) {
            T bits = 0;
            while (v) {
                ++bits;
                v = v >> 1;
            }
            return bits;
        }

        //    auto MergeOrF = x.Equals((Exponentiation((b ^ 2) - 4_v * a * c, constants::half)-b)/(a*2));
        //    auto aMergeOrF = MergeOrF(a);
        //    auto bMergeOrF = MergeOrF(b);
        //    auto cMergeOrF = MergeOrF(c);
    }
    Valuable Valuable::MergeOr(const Valuable& _1, const Valuable& _2)
    {
        Valuable merged;
        if(_1 == _2)
            merged = _1;
        else if (_1 == -_2)
        {
            merged = _1 * constants::plus_minus_1;
        }
        else
        {
            // a = 1;
            auto s = _1 + _2;
            if(s.IsZero())
            {
                merged = (!_1.IsProduct() ? _1 : _2) * constants::plus_minus_1;
            }
            else
            {
                OptimizeOff off;
                // b = -s;
                auto c = _1 * _2;
                auto d = s.Sq() + c * -4;
                if (_1.IsMultival() == YesNoMaybe::No && _2.IsMultival() == YesNoMaybe::No) {
                    merged = (Exponentiation(d, constants::half) + s) / constants::two;
                } else {
                    auto dist = _1.Distinct(); // FIXME : not efficient branch, prefer better specializations
                    dist.merge(_2.Distinct());
                    auto grade = dist.size();
                    auto targetGrade = constants::one.Shl(bits_in_use(grade));
                    merged = (Exponentiation(d, targetGrade.Reciprocal()) + s) / targetGrade;
                }
            }
        }
        {
            OptimizeOn oo;
			merged.optimize();
		}
        return merged;
    }

	Valuable Valuable::MergeOr(const Valuable& v1, const Valuable& v2, const Valuable& v3) {
        // 1,2,3:  1 + (1 or 2) * (1 or 0)   =>   1st + ((2nd or 3rd) - 1st) * (0 or 1)
        return Sum{Product{constants::zero_or_1, MergeOr(v3 - v1, v2 - v1)}, v1};
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

    namespace {
    void Optimize(Valuable::solutions_t& s) {
        Valuable::solutions_t distinct;
        Valuable::OptimizeOn enable;
        while (s.size()) {
			auto it = s.begin();
			auto v = std::move(s.extract(it).value());
            v.optimize();
            distinct.emplace(std::move(v));
		}
		std::swap(s, distinct);
    }
    } // namespace
    Valuable::Valuable(solutions_t&& s)
    {
        if (!optimizations
            || !std::all_of(s.begin(), s.end(), [](auto& v){ return v.is_optimized(); })
        ) {
            Optimize(s);
        }

        auto it = s.begin();
#if !defined(NDEBUG) && !defined(NOOMDEBUG)
		std::cout << " Merging [ ";
        for(auto& item: s){
            std::cout << item << ' ';
        }
        std::cout << ']' << std::endl;
#endif
        switch (s.size()) {
        case 0: LOG_AND_IMPLEMENT("Empty solutions set"); break;
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
            if (s.size() == 0) {
                s = std::move(pairs);
            }

            if (pairs.size()) {
                operator=(MergeOr(Valuable(std::move(pairs)), Valuable(std::move(s))));
            } else {
                while(s.size() > 1){
                    solutions_t ss;
				    while(s.size() >= 4){
					    auto it = s.begin();
					    auto& _1 = *it++;
					    auto& _2 = *it++;
                        auto& _3 = *it++;
                        auto& _4 = *it++;
                        ss.emplace(MergeOr(_1, _2, _3, _4));
				    }
                    if(s.size()){
					    ss.emplace(std::move(s));
                    }
                    s = std::move(ss);
                }
                operator=(std::move(s.extract(s.begin()).value()));
			}

#if !defined(NDEBUG) && !defined(NOOMDEBUG)
            std::stringstream ss;
            ss << '(';
            for (auto& v : s)
                ss << ' ' << v;
            ss << " )";
            std::cout << ss.str();
            LOG_AND_IMPLEMENT("Implement disjunctive merging algorithm for " << s.size() << " items " << ss.str());
#else
            LOG_AND_IMPLEMENT("Implement MergeOr for three items and research if we could combine with case 2 for each couple in the set in paralell and then to the resulting set 'recoursively'")
#endif
        }

#if !defined(NDEBUG) && !defined(NOOMDEBUG)
        if(s.size() > 1){
            std::stringstream ss;
            OptimizeOn oo;
            auto distinct = Distinct();
            if (distinct != s) {
                ss << '(';
                for (auto& v : s)
                    ss << ' ' << v;
                ss << " ) <> (";
                for (auto& v : distinct)
                    ss << ' ' << v;
                ss << " ), ";
                std::cout << ss.str();

                constexpr auto isMultival = [](auto& item) {
                    return item.IsMultival() == YesNoMaybe::Yes;
                };
                auto someWasMultival = std::any_of(s.begin(), s.end(), isMultival);
                if (someWasMultival) {
                    solutions_t sDistinct;
                    while (s.size()) {
                        auto item = std::move(s.extract(s.begin()).value());
                        item.optimize();
                        if (item.IsMultival() == YesNoMaybe::Yes) {
                            auto itemDistinct = item.Distinct();
                            while (itemDistinct.size()) {
                                auto subitem = std::move(itemDistinct.extract(itemDistinct.begin()).value());
                                subitem.optimize();
                                sDistinct.emplace(std::move(subitem));
                            }
                        } else {
                            sDistinct.emplace(std::move(item));
                        }
                    }
                    s = std::move(sDistinct);
                }
            }

            if (distinct != s) {
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


struct hash_tokens_collection_t {
    static constexpr HashStrOmitOuterBrackets hasher = {};
    static size_t reduce(size_t s, std::string_view str) {
        auto strhash = hasher(str);
        return s ^ strhash;
    }
    size_t operator()(const ::omnn::math::StateProxyComparator::tokens_collection_t& c) const {
        size_t hash = 0;
        hash = std::accumulate(c.begin(), c.end(), hash, &hash_tokens_collection_t::reduce);
        return hash;
    }
};

namespace {
// Function to convert a decimal number to a fraction
std::string decimalToFraction(std::string_view decimal) {
    // Split the number into integer and fractional parts
    auto dotPos = decimal.find('.');
    if (dotPos == std::string_view::npos) {
        return std::string(decimal); // Not a decimal number, return as is
    }

    // Handle negative numbers
    auto isNegative = decimal[0] == '-';
    if (isNegative) {
        decimal = decimal.substr(1); // Remove the negative sign for now
    }


    // Convert parts to cpp_int
    a_int integerValue(decimal.substr(0, dotPos - 1));
    auto fractionalPart = decimal.substr(dotPos);
    a_int fractionalValue(fractionalPart);

    // Calculate the denominator based on the number of digits in the fractional part
    a_int denominator = 1;
    for (size_t i = 0; i < fractionalPart.length(); ++i) {
        denominator *= 10;
    }

    a_rational ratio(integerValue * denominator + fractionalValue, denominator);
    a_int numerator = boost::multiprecision::numerator(ratio);
    denominator = boost::multiprecision::denominator(ratio);

    // Add back the negative sign if needed
    std::stringstream result;
    if (isNegative) {
        result << "(-" << numerator << ')';
    } else {
        result << numerator;
    }
    result << '/' << denominator;

    return result.str();
}

// Function to replace all decimal numbers in a string with their rational forms
std::regex DecimalRegex(R"(-?\d+\.\d+)");
std::string replaceDecimalsWithFractions(std::string_view input) {
    std::string result(input); // Make a copy of the input string
    std::smatch match;

    // Find all matches and replace them
    while (std::regex_search(result, match, DecimalRegex)) {
        auto decimalNumber = match.str();
        auto fraction = decimalToFraction(decimalNumber);
        result.replace(match.position(), match.length(), fraction);
    }

    return result;
}
}
bool Valuable::SerializedStrEqual(const std::string_view& s) const {
    auto _ = str();
    auto same = s == _ || (_.front() == '(' && _.back() == ')' && s == _.substr(1, _.length() - 2));
    if (!same) {
        auto _1 = Solid(_), _2 = Solid(std::string(s));
        same = _1 == _2 || (_1.front() == '(' && _1.back() == ')' && _2 == _1.substr(1, _1.length() - 2));
        if (!same && IsInt() && s.length() > 2 && (s[1] == 'x' || s[1] == 'X')) {
            _2 = a_int(s).str();
            same = _1 == _2;
        }
        if (!same) {
            boost::replace_all(_1, "+-", "-");
            boost::replace_all(_2, "+-", "-");
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
            (void)OmitOuterBrackets(_1);
            (void)OmitOuterBrackets(_2);
            same = _1 == _2;
        }
        if (!same) {
            auto str2set = [this](auto& str) {
                StateProxyComparator strCmpPassthrough(this);
                auto sumItemsSubstrings = strCmpPassthrough.TokenizeStringViewToMultisetKeepBraces(str, '+');
                std::unordered_multiset<decltype(sumItemsSubstrings), hash_tokens_collection_t> sets;
                for (auto& s : sumItemsSubstrings) {
                    sets.insert(strCmpPassthrough.TokenizeStringViewToMultisetKeepBraces(s, '*'));
                }
                return sets;
            };
            auto svSet1 = str2set(_1);
            auto svSet2 = str2set(_2);
            same = svSet1 == svSet2;
        }
        if (!same && _2.find('.') != std::string::npos) {
            same = SerializedStrEqual(replaceDecimalsWithFractions(_2));
        }
#if !defined(NDEBUG) && !defined(NOOMDEBUG)
        if (!same) {
            HashStrIgnoringAnyParentheses hasher;
            if (hasher(_1) == hasher(_2)) {
                std::cout << "Fix SerializedStrEqual: " << _ << " != " << s << std::endl
                      << _1 << "\n !=\n"
                      << _2 << std::endl;
                same = true;
            } else if (!_1.empty()) {
                std::cout << "SerializedStrEqual detected deserialization issue: " << _ << " != " << s << std::endl
                          << _1 << "\n !=\n"
                          << _2 << std::endl;
                ::omnn::rt::LoopDetectionGuard<Valuable> antilooper(*this);
                if (antilooper.isLoopDetected()) {
                    std::cout << "Loop of optimizing detected in " << *this << std::endl;
                } else {
                    Valuable v(s, getVaHost(), true);
                    same = operator==(v);
                    if (same) {
                        std::cout << " operator==(" << s << ") == true" << std::endl;
                    }
                }
            }
        }
#endif // !NDEBUG
    }
    return same;
}

    namespace {
        constexpr char SupportedOps[] = " */%+-^()!";
    }
    Valuable::Valuable(const std::string_view& str
        , std::shared_ptr<VarHost> host // = nullptr
        , bool itIsOptimized // = false
	) {
        if (!host)
            host = VarHost::Global<std::string>().shared_from_this();
        auto l = str.length();
        using index_t = decltype(l);
        std::stack <index_t> st;
        std::map<index_t, index_t> bracketsmap;
        decltype(l) c = 0;
        while (c < l)
        {
            if (str[c] == '(')
                st.push(c);
            else if (str[c] == ')')
            {
                bracketsmap.emplace(st.top(), c);
                st.pop();
            } else if(str[c] == '=') {
                Valuable r(str.substr(c+1), host, itIsOptimized);
                if (c && str[c-1] == '<') {
                    Valuable l(str.substr(0, c - 1), host, itIsOptimized);
                    Become(l.LessOrEqual(r));
                } else if (c && str[c - 1] == '>') {
                    Valuable l(str.substr(0, c - 1), host, itIsOptimized);
                    Become(r.LessOrEqual(l));
                } else {
                    Valuable l(str.substr(0, c), host, itIsOptimized);
                    l.equals(std::move(r));
                    Become(std::move(l));
                }
                SetView(View::Equation);
                return;
            } else if (str[c] == '<' && c + 1 < l && str[c + 1] != '=') {
                Valuable l(str.substr(0, c), host, itIsOptimized);
                if(str[c + 1] == '>'){
                    Valuable r(str.substr(c + 2), host, itIsOptimized);
                    Become(l.NotEquals(r));
                } else {
                    Valuable r(str.substr(c + 1), host, itIsOptimized);
                    Become(l.Less(r));
                }
                SetView(View::Equation);
                return;
            } else if (str[c] == '>' && c + 1 < l && str[c + 1] != '=' && str[c - 1] != '-') {
                Valuable l(str.substr(0, c), host, itIsOptimized);
                Valuable r(str.substr(c + 1), host, itIsOptimized);
                Become(r.Less(l));
                SetView(View::Equation);
                return;
            }
            c++;
        }
        if (!st.empty())
            throw "parentheses relation mismatch";
        auto ok = bracketsmap.empty();
        if (ok)
        {
            auto copy = str;
            auto s = Trim(copy);
            auto offs = 0;
            while (s[offs]=='-')
                ++offs;
            auto found = s.find_first_not_of("0123456789", offs);
            if (found == std::string::npos)
            {
                exp = std::make_shared<Integer>(s);
            } else if (s.size() > found + 1 && s[found] == 'r'
                && std::all_of(s.begin(), s.begin() + found, [](auto ch) { return std::isdigit(ch); })
                && std::all_of(s.begin() + found + 1, s.end(), [](auto ch) { return std::isdigit(ch); })
                )
                exp = std::make_shared<PrincipalSurd>(
                    Integer(a_int(s.substr(found+1))),
                    Integer(a_int(s.substr(0, found))));
            else if (s[0] == 'v' && s.size() > 1 && host && host->IsIntegerId()
                && std::all_of(s.begin() + 1, s.end(),
					[](auto ch) { return std::isdigit(ch); }
					)
				)
                Become(Valuable(host->Host(Valuable(a_int(s.substr(1))))));
            else if (s.length() > 2
                && s[0] == '0'
                && (s[1] == 'x' || s[1] == 'X')
                && std::isxdigit(s[2])
                && s.find_first_not_of("0123456789ABCDEFabcdef", 2) == std::string::npos
                )
                Become(Integer(s));
            else if (!isdigit(s[0])
                && std::all_of(s.begin(), s.end(), [](auto c) { return std::isalnum(c); })
                )
            {
                auto it = constants::Constants.find(s);
                if (it != constants::Constants.end()) {
                    Valuable v(it->second);
                    if (itIsOptimized)
                        v.MarkAsOptimized();
                    Become(std::move(v));
                } else if (host != nullptr) {
                    Become(Valuable(host->Host(s)));
                } else {
                    LOG_AND_IMPLEMENT("Parse " << s);
                }
            }
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
            op_t o_mod;
            op_t o_pSurd;
            if (itIsOptimized) {
                sum.MarkAsOptimized();
                v.MarkAsOptimized();
                o_sum = [&](Valuable&& val) {
                    if (val != 0) {
                        if (mulByNeg) {
                            val *= -1;
                            mulByNeg = {};
                        }
                        if (sum.IsSum())
                        {
                            sum.as<Sum>().Add(std::move(val));
                            sum.MarkAsOptimized();
                        }
                        else {
                            Sum s;
                            s.Add(std::move(sum));
                            s.Add(std::move(val));
                            s.MarkAsOptimized();
                            sum = std::move(s);
                        }
                    }
                };
                o_mul = [&](Valuable&& val) {
                    if (mulByNeg) {
                        val *= -1;
                        mulByNeg = {};
                    }
                    if (v.IsProduct()) {
                        v.as<Product>().Add(std::move(val));
                        v.MarkAsOptimized();
                    } else {
                        Product p({});
                        p.Add(std::move(v));
                        p.Add(std::move(val));
                        p.MarkAsOptimized();
                        v = std::move(p);
                    }
                };
                o_div = [&](Valuable&& val) {
                    if (mulByNeg) {
                        val *= -1;
                        mulByNeg = {};
                    }
                    Fraction f{std::move(v), std::move(val)};
                    f.MarkAsOptimized();
                    v = std::move(f);
                };
                o_mod = [&](Valuable&& val) {
                    if (mulByNeg) {
                        val *= -1;
                        mulByNeg = {};
                    }
                    Modulo m{std::move(v), std::move(val)};
                    m.MarkAsOptimized();
                    v = std::move(m);
                };
                o_pSurd = [&](Valuable&& val) {
                    if (mulByNeg) {
                        val *= constants::minus_1;
                        mulByNeg = {};
                    }
                    PrincipalSurd ps{std::move(v), std::move(val)};
                    ps.MarkAsOptimized();
                    v = std::move(ps);
                };
                o_exp = [&](Valuable&& val) {
                    if (mulByNeg) {
                        val *= -1;
                        mulByNeg = {};
                    }
                    Exponentiation e{std::move(v), std::move(val)};
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
                o_mod = [&](Valuable&& val) {
                    if (mulByNeg) {
                        val *= -1;
                        mulByNeg = {};
                    }
                    v %= std::move(val);
                };
                o_pSurd = [&](Valuable&& val) {
                    if (mulByNeg) {
                        val *= constants::minus_1;
                        mulByNeg = {};
                    }
                    v = PrincipalSurd{std::move(val), std::move(v)};
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
            for (index_t i = str.find_first_not_of(" \t\n\r"); i < l; ++i)
            {
                auto c = str[i];
                if (c == '(')
                {
                    auto cb = bracketsmap[i];
                    auto next = i + 1;
                    o(Valuable(str.substr(next, cb - next), host, itIsOptimized));
                    i = cb;
                }
                else if (c == 'v') {
                    auto idStart = i + 1;
                    auto next = str.find_first_not_of("0123456789", idStart);
                    auto id = str.substr(idStart, next - idStart);
                    if (id.empty()) {
                        auto to = str.find_first_of(SupportedOps, idStart);
                        auto id = to == std::string::npos ? str.substr(i) : str.substr(i, to - i);
                        o(Valuable(host->Host(id)));
                        i = to - 1;
                    } else if (host->IsIntegerId()){
                        o(Valuable(host->Host(Valuable(a_int(id)))));
                        i = next - 1;
                    } else {
                        next = str.find_first_of(SupportedOps, idStart);
                        if (next == std::string::npos) {
                            id = str.substr(i);
                            next = i + id.size();
                        } else {
                            id = str.substr(i, next - i);
                        }
                        o(Valuable(host->Host(id)));
                        i = next - 1;
                    }
                } else if (c == '-') {
                    o_sum(std::move(v));
                    v = 0;
                    o = o_mov;
                    mulByNeg ^= true;
                } else if ((c >= '0' && c <= '9') || c =='.') {
                    auto next = str.find_first_not_of(" 0123456789.", i+1);
                    auto ss = str.substr(i, next - i);
                    Trim(ss);
                    auto hasSpace = ss.find(' ') != std::string::npos;
                    auto dot = ss.find('.');
                    i = next - 1;
                    if (dot != std::string::npos) {
                        std::string s;
                        if(hasSpace){
                            s = ss;
                            s = Solid(s);
                            ss = s;
                        }
                        auto beforedot = ss.substr(0, dot);
                        auto afterdot = ss.substr(dot + 1);
                        auto f = Integer(beforedot) + Integer(afterdot) / (10_v ^ afterdot.length());
                        o(std::move(f));
                    } else {
                        Valuable integer = hasSpace
                            ? Integer(Solid(std::string(ss)))
                            : Integer(ss);
                        if (i < str.length() && str[next] == 'r') {
                            o = o_pSurd;
                        } else {
                            o(std::move(integer));
                        }
                    }
                }
                else if (c == '+') {
                    o_sum(std::move(v));
                    v = 0;
                    o = o_mov;
                }
                else if (c == '*') {
                    o = o_mul;
                    while (str[i + 1] == ' ')
                        ++i;
                    mulByNeg ^= str[i + 1] == '-';
                }
                else if (c == '/') {
                    o = o_div;
                    while (str[i + 1] == ' ')
                        ++i;
                }
                else if (c == '%') {
                    o = o_mod;
                    while (str[i + 1] == ' ')
                        ++i;
                }
                else if (c == '^') {
                    o = o_exp;
                }
                else if (c == '!') {
                    v.factorial();
                    if (mulByNeg) {
                        v *= -1;
                        mulByNeg = {};
                    }
                }
                else if (c == ' ') {
                }
                else if (std::isalpha(c)){
                    auto to = str.find_first_of(SupportedOps, i+1);
                    auto id = str.substr(i);
                    if (to != std::string::npos) {
                        id = str.substr(i, to - i);
                        if (str[to] == '(') { // functions
                            if (to == 0) {
                                IMPLEMENT
                            }
                            auto cb = bracketsmap[to];
                            if (id == "sqrt"sv) {
                                auto next = to + 1;
                                Valuable value(str.substr(next, cb - next), host, itIsOptimized);
                                value = PrincipalSurd{std::move(value)};
                                if (mulByNeg) {
                                    value *= -1;
                                    mulByNeg = {};
                                }
                                o(std::move(value));
                            } else if (id == "factorial"sv) {
                                auto next = to + 1;
                                auto arg = Valuable(str.substr(next, cb - next), host, itIsOptimized);
                                arg.factorial();
                                if (mulByNeg) {
                                    arg *= -1;
                                    mulByNeg = {};
                                }
                                o(std::move(arg));
                            } else if (id == "gamma"sv) {
                                auto next = to + 1;
                                auto arg = Valuable(str.substr(next, cb - next), host, itIsOptimized);
                                arg.gamma();
                                if (mulByNeg) {
                                    arg *= -1;
                                    mulByNeg = {};
                                }
                                o(std::move(arg));
                            } else if (id == "log"sv) {
                                auto next = to + 1;
                                auto args = str.substr(next, cb - next);
                                o(Logarithm(args, host, itIsOptimized));
                            } else if (id == "ln"sv) {
                                auto next = to + 1;
                                auto arg = str.substr(next, cb - next);
                                o(Logarithm(Euler(), Valuable(arg, host, itIsOptimized)));
                            } else if (id == "lim"sv || id == "limit"sv) {
                                auto next = to + 1;
                                auto args = str.substr(next, cb - next);
                                o(Limit(args, host, itIsOptimized));
                            }
							i = cb;
                            continue;
                        }
                    }
                    Valuable val(id, host, itIsOptimized);
                    if (mulByNeg) {
                        val *= -1;
                        mulByNeg = {};
                    }
                    o(std::move(val));
                    i = to - 1;
                } else {
                    LOG_AND_IMPLEMENT("Unexpected char " << c << " in " << str << " position " << i);
                }
            }

            o_sum(std::move(v));
            Become(std::move(sum));
            if(IsSum()){
                auto& sum = as<Sum>();
                if(sum.size()==1){
                    Become(sum.Extract());
                }
            }
        }

#if !defined(NDEBUG) && !defined(NOOMDEBUG) && defined(TEST_SRC_DIR)
        if (!SerializedStrEqual(str)) {
            LOG_AND_IMPLEMENT(
                "Deserialization check failed. "
                " potential reasons:\n"
                "  text expression ordering differs from this software expression building ordering"
                "  var host type is changed between integer-type and string-type var host"
                "  this software code changed expression building ordering or other changes that could be a cause "
                "for this deserialization check error message (optimization changes for example)");
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

    Valuable::Valuable(std::string_view s, const Valuable::va_names_t& vaNames, bool itIsOptimized)
    {
#if !defined(NDEBUG) && !defined(NOOMDEBUG)
      if (s.empty()) {
        LOG_AND_IMPLEMENT("Empty string in Valuable constructor")
      }
#endif // NOOMDEBUG

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
					&& (found == 0 || s[found-1] != '*')
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
                Exponentiation exp{l, r};
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
						: Integer(Solid(std::string(s)));
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
                        } else if (vaNames.empty()) {
                            Valuable varless(s, nullptr, itIsOptimized);
                            Become(std::move(varless));
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
        DispatchDispose(std::move(exp));
    }

    Valuable Valuable::operator -() const
    {
        if(exp)
            return exp->operator-();
        else
            return Product{constants::minus_1, *this}; // Default negation
    }

    VALUABLE_POLYMORPHIC_METHOD(operator+=)

    Valuable& Valuable::operator +=(int value)
    {
        Integer integer(value);
        return call_polymorphic_method(&Valuable::operator+=, integer);
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
        else
        {
            call_polymorphic_method(&Valuable::operator*=, v);
        }
        return *this;
    }

    a_int Valuable::Complexity() const
    {
        if(exp)
            return exp->Complexity();
        IMPLEMENT
    }

    bool Valuable::MultiplyIfSimplifiable(const Integer& integer) {
        return MultiplyIfSimplifiable(static_cast<const Valuable&>(integer));
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
        if (!optimizations)
            return {};
        else if (exp)
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
        if (!optimizations)
            return {};
        else if(exp)
            return exp->IsSummationSimplifiable(v);
        else {
            LOG_AND_IMPLEMENT(str() << " IsSummationSimplifiable " << v);
            auto m = *this + v;
            return { m.Complexity() < Complexity() + v.Complexity(), m };
        }
    }

    std::pair<bool, Valuable> Valuable::IsModSimplifiable(const Valuable& value) const
    {
        if (!optimizations)
            return {};
        else if(exp)
            return exp->IsModSimplifiable(value);
        else {
            LOG_AND_IMPLEMENT(*this << " IsModSimplifiable " << value);
            auto mod = *this % value;
            return {mod.Complexity() < Complexity() + value.Complexity(), mod};
        }
    }

    VALUABLE_POLYMORPHIC_METHOD(operator/=)

    VALUABLE_POLYMORPHIC_METHOD(operator%=) // a - (n * int(a/n)) https://math.stackexchange.com/a/2027475/118612

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

    VALUABLE_POLYMORPHIC_METHOD(operator^=)

    Valuable Valuable::GCD(const Valuable& v) const {
        if (exp) {
            return exp->GCD(v);
        }
        if (v.IsInt() && FindVa() != nullptr) {
            return varless().GCD(v);
        }
        auto isEqual = operator==(v);
        auto thisMoreComplex = !isEqual && Complexity() >= v.Complexity();
        Valuable a = isEqual || thisMoreComplex ? *this : v.GCD(*this);
        if (thisMoreComplex) {
            Valuable b = v;
            while (!b.IsZero()) {
                Valuable temp = b;
                b = a % b;
                if (b.IsModulo()) {
                    b = std::move(b.as<Modulo>().get1());
                    if (b == a) {
                        a = IsSum() ? as<Sum>().GCDofMembers() : constants::one;
                        if (a != constants::one && v.IsSum())
                            a.gcd(v.as<Sum>().GCDofMembers());
                        break;
                    }
                }
                a = temp;
            }
        }
        return a;
    }

    Valuable& Valuable::gcd(const Valuable& v) {
        if (exp) {
            exp->gcd(v);
        } else {
            Become(GCD(v));
        }
        return *this;
    }

    Valuable Valuable::LCM(const Valuable& v) const {
        if (exp) {
            return exp->LCM(v);
        } else {
            auto copy = *this;
            copy.lcm(v);
            return copy;
        }
    }

    Valuable& Valuable::lcm(const Valuable& v) {
        if (exp) {
            Valuable& o = exp->lcm(v);
            if (o.exp) {
                exp = o.exp;
            }
        } else if (IsZero() || v.IsZero()) {
            Become(0);
        } else if (operator==(v)) {
        } else {
            auto gcd = GCD(v);
            operator*=(v);
            // FIXME : abs();
            Become(abs());
            operator/=(gcd);
        }
        return *this;
    }

    Valuable& Valuable::d(const Variable& x)
    {
        if(exp) {
            return exp->d(x);
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

	Valuable& Valuable::integral(const Variable& x, const Valuable& from, const Valuable& to, const Variable& C) {
        integral(x, C);
        return Become(Eval({{x, to}}) - Eval({{x, from}}));
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

	Valuable& Valuable::integral(const Variable& x, const Variable& C)
    {
        if(exp) {
            return exp->integral(x, C);
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

    Valuable::solutions_t Valuable::solve(const Variable& variable) const
    {
        solutions_t solutions;
        solve(variable, solutions);
        return solutions;
    }

    const PrincipalSurd* Valuable::PrincipalSurdFactor() const {
        if (exp) {
            return exp->PrincipalSurdFactor();
        } else {
            LOG_AND_IMPLEMENT("PrincipalSurdFactor " << str());
        }
    }

    Valuable Valuable::operator()(const Variable& va) const
    {
        if(exp) {
            return exp->operator()(va);
        }
        else
            return operator()(va, constants::zero);
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

    bool Valuable::IsPolynomial(const Variable& v) const {
        if(exp)
            return exp->IsPolynomial(v);
        else
            IMPLEMENT
    }

    size_t Valuable::FillPolynomialCoefficients(std::vector<Valuable>& coefficients, const Variable& v) const {
        if(exp)
            return exp->FillPolynomialCoefficients(coefficients, v);
        else
            IMPLEMENT
    }

    std::vector<Valuable> Valuable::Coefficients(const Variable& v) const
    {
        std::vector<Valuable> coefficients;
        (void) FillPolynomialCoefficients(coefficients, v);
        return coefficients;
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
        if(this->view == View::Solving || IsPolynomial(v))
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
        Valuable::solutions_t solutions;
        std::set<Variable> vars;
        CollectVa(vars);
        switch (vars.size()) {
        case 0:
            if (IsZero()) {
                LOG_AND_IMPLEMENT("Implement Any value");
                // return Any();
            }
            break;
        case 1:
            solutions = IntSolutions(*vars.begin());
            break;
        default:
            IMPLEMENT
        }
        return solutions;
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
            LOG_AND_IMPLEMENT("GetIntegerSolution for multiple variables in " << *this)
    }

    Valuable::solutions_t Valuable::GetIntegerSolution(const Variable& va) const
    {
        if(exp) {
            return exp->GetIntegerSolution(va);
        }
        else
            LOG_AND_IMPLEMENT("GetIntegerSolution for " << *this << " with variable " << va)
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
        return t.IsZero()
            && va.getVaHost()->TestRootConsistencyWithNonZeroLog(va, v);
    }

    Valuable Valuable::SumOfRoots() const {
        if (exp) {
            return exp->SumOfRoots();
        } else
            LOG_AND_IMPLEMENT("SumOfRoots for " << *this)
    }

    Valuable Valuable::ProductOfRoots() const {
        if (exp) {
            return exp->ProductOfRoots();
        } else
            LOG_AND_IMPLEMENT("ProductOfRoots for " << *this)
    }

    using zone_t = std::pair<Valuable/*from*/,Valuable/*to*/>;
    using ranges_t = std::pair<zone_t/*whole*/,std::deque<zone_t>/*subranges*/>;

    ranges_t Valuable::get_zeros_zones(const Variable& v, solutions_t& some) const
    {
        auto fm = std::abs(calcFreeMember());
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
            auto sign = Abs();
            sign /= *this;
            return sign;
            //return GreaterOrEqual(constants::zero).ToBool() - LessOrEqual(constants::zero).ToBool();
            // sign(x) = (2/pi) * integral from 0 to +infinity of (sine(t*x)/t) dt")
        }
	}

    bool Valuable::operator<(const Valuable& v) const
    {
        if (exp)
            return exp->operator<(v);
        else if (operator==(v))
			return false;
        else if ((IsRational() && v.IsRational()) == YesNoMaybe::Yes)
        {
            auto _1 = operator a_rational();
            auto _2 = static_cast<a_rational>(v);
            return _1 < _2;
        }
        else if (v.IsZero())
        {
            return Sign() == constants::minus_1;
        }
        else {
            auto diff = *this - v;
            if (!diff.FindVa()) {
                return diff < constants::zero;
            } else {
                LOG_AND_IMPLEMENT(diff << " < 0");
            }
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
    bool Valuable::IsSurd() const { return exp && exp->IsSurd(); }
    bool Valuable::IsFraction() const { return exp && exp->IsFraction(); }
    bool Valuable::IsSimpleFraction() const { return exp && exp->IsSimpleFraction(); }
    bool Valuable::IsFormula() const { return exp && exp->IsFormula(); }
    bool Valuable::IsExponentiation() const { return exp && exp->IsExponentiation(); }
    bool Valuable::IsLogarithm() const { return exp && exp->IsLogarithm(); }
    bool Valuable::IsLimit() const { return exp && exp->IsLimit(); }
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

    bool Valuable::IsZero() const { return exp && exp->IsZero(); }

    bool Valuable::IsSimple() const {
        if(exp)
            return exp->IsSimple();
        else
            LOG_AND_IMPLEMENT("IsSimple for " << *this)
    }

    YesNoMaybe Valuable::IsMultival() const {
        if(exp)
            return exp->IsMultival();
        else
            LOG_AND_IMPLEMENT("IsMultival for " << *this)
    }

    void Valuable::Values(const std::function<bool(const Valuable&)>& f) const {
        if(exp)
            exp->Values(f);
        else
            LOG_AND_IMPLEMENT("Values for " << *this)
    }

    YesNoMaybe Valuable::IsRational() const {
        if (exp)
            return exp->IsRational();
        else
            LOG_AND_IMPLEMENT(*this << " IsRational method");
    }

    YesNoMaybe Valuable::IsEven() const {
        if(exp)
            return exp->IsEven();
        else
            LOG_AND_IMPLEMENT("IsEven for " << *this)
    }

    bool Valuable::is(const std::type_index& ti) const {
#if !defined(NDEBUG) && !defined(NOOMDEBUG)
        if(exp)
            LOG_AND_IMPLEMENT("is() for " << *this << " with type " << ti.name())
#endif
        return ti == std::type_index(typeid(Valuable));
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
            LOG_AND_IMPLEMENT("print(std::wostream&) for " << *this)
    }

    std::ostream& Valuable::code(std::ostream& out) const {
        if (exp)
            return exp->code(out);
        else
            LOG_AND_IMPLEMENT("code(std::ostream&) for " << *this)
    }

    Valuable::universal_lambda_t Valuable::CompileIntoLambda(variables_for_lambda_t variablesOfParams) const {
        if (exp)
            return exp->CompileIntoLambda(variablesOfParams);
        else
            IMPLEMENT
    }

    std::string Valuable::OpenCLuint() const {
        return OpenCL("uint"sv);
    }

    std::string Valuable::OpenCL(const std::string_view& TypeName) const {
        std::stringstream source;
        source << "__kernel void f(__global " << TypeName << " * Result ";
        auto vars = Vars();
        for (auto& v : vars) {
            auto vname = v.str();
            if (vname != "i" && vname != "Result" && vname != "was")
                source << ",__global " << TypeName << " * _ " << v;
        }
        source << "){const " << TypeName << " _i = get_global_id(0);";
        for (auto& v : vars) {
            auto vname = v.str();
            if (vname != "Result") {
                source << TypeName;
                if (vname == "was")
                    source << " was=Result";
                else
                    source << ' ' << vname << "=_" << vname;
                if (vname != "i")
                    source << "[_i]";
                source << ';';
            }
        }
        source << "Result[_i]=(" << TypeName << ")(";
        code(source);
        source << ");}";
        return source.str();
    }

	Valuable::va_names_t Valuable::OpenCLparamVarNames() const {
		return {{"i", {}}, {"n", {}}, {"Result", {}}};
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
            return Sq().Sqrt();
    }

    Valuable Valuable::Abs() const
    {
        if(exp)
            return exp->Abs();
        else
            return Sq().Sqrt();
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
            if (IsEquation()
                && v == View::Equation
            ) {
                std::swap(view, v);
            }
            optimized = optimized && view == v;
            view = v;
        }
    }

    void Valuable::optimize()
    {
        if (exp) {
            if (optimizations) {
                while (exp->exp) {
                    exp = exp->exp;
                }
                exp->optimize();
                while (exp->exp) {
                    exp = exp->exp;
                }
                return;
            }
        }
        else
            LOG_AND_IMPLEMENT("Implement optimize() for " << *this);
    }

	Valuable Valuable::Cos() const {
		if (exp)
			return exp->Cos();
		else {
			return ((constant::e ^ Product{ constant::i, *this }) + (constant::e ^ Product{ constants::minus_1, constant::i, *this })) / 2;
		}
	}

	Valuable Valuable::Sin() const {
		if (exp)
			return exp->Sin();
		else {
			static const Product _2i{ 2, constant::i };
			return ((constant::e ^ Product{ constant::i, *this }) - (constant::e ^ Product{ constants::minus_1, constant::i, *this })) / _2i;
		}
	}

    Valuable Valuable::Sqrt() const {
        if (exp)
            return exp->Sqrt();
        else
            return PrincipalSurd(*this, 2);
    }

    Valuable& Valuable::sqrt() {
        if (exp)
            return exp->sqrt();
        else
            return Become(Sqrt());
    }

	Valuable Valuable::Tg() const {
		if (exp)
			return exp->Tg();
		else {
			return Sin() / Cos();
		}
	}

    Valuable Valuable::Tanh() const {
        if (exp)
            return exp->Tanh();
        else {
            return constants::one - constants::two / ((constants::e ^ Shl()) + constants::minus_1);
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

	Valuable& Valuable::factorial() {
        if (exp)
            exp->factorial();
        else {
            operator++().gamma();
        }
        return *this;
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

    Valuable Valuable::Factors(const Variable& factor) const {
        return constants::two.LessOrEqual(factor)
            .gcd(GreaterOrEqual(factor.Sq()))
            .gcd((*this % factor).Equals(constants::zero));
    }

	Valuable Valuable::FirstFactor() const {
        if (exp)
            return exp->FirstFactor();
        else {
            auto host = getVaHost();
            Variable factor(host);
            auto factors = Factors(factor);
            // TODO: implement GetMinRoot: return factors.MinimalRoot();

            auto foundFactors = factors.Solutions(factor);
            return *std::min_element(foundFactors.begin(), foundFactors.end()
                , [](auto item1, auto item2) { return item1 <= item2; }
                );
        }
	}

    Valuable Valuable::LastFactor() const {
        return *this / FirstFactor();
	}

	Valuable& Valuable::reciprocal() {
        if (exp)
            return exp->reciprocal();
        else {
            return Become(Exponentiation{Move(), constants::minus_1});
        }
    }

	Valuable Valuable::Reciprocal() const {
        if (exp)
            return exp->Reciprocal();
        else {
            auto f = *this;
            f.reciprocal();
            return f;
        }
	}

    const Variable* Valuable::FindVa() const
    {
        if (exp) {
            return exp->FindVa();
        }
        LOG_AND_IMPLEMENT("Implement FindVa for " << *this);
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
            LOG_AND_IMPLEMENT("CollectVaNames for " << *this)
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

    bool Valuable::eval(const vars_cont_t& with) {
        if (exp)
            return exp->eval(with);
        else
            LOG_AND_IMPLEMENT("eval for " << *this << " with " << with)
    }

    Valuable Valuable::Eval(const vars_cont_t& with) const {
        auto evaluate = *this;
        evaluate.eval(with);
        return evaluate;
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
        LOG_AND_IMPLEMENT("Eval for " << *this << " with variable " << va << " and value " << v)
    }

    bool Valuable::OfSameType(const Valuable& v) const
    {
        const Valuable& v1 = get();
        const Valuable& v2 = v.get();
        return typeid(v1) == typeid(v2);
    }

    bool Valuable::Same(const Valuable& value) const
    {
        if (exp)
            return exp->Same(value);
        return Hash() == value.Hash()
            && OfSameType(value)
            && operator==(value);
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

    Valuable::vars_cont_t Valuable::GetVaExps() const
    {
        if (exp)
            return exp->GetVaExps();
        else
            LOG_AND_IMPLEMENT("Implement " << boost::core::demangle(Type().name()) << "::GetVaExps() for " << *this);
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
            LOG_AND_IMPLEMENT("IsComesBefore for " << *this << " and " << v)
    }

    const Valuable::vars_cont_t& Valuable::getCommonVars() const
    {
        if (exp)
            return exp->getCommonVars();
        else
            LOG_AND_IMPLEMENT("getCommonVars for " << *this)
    }

    Valuable::vars_cont_t Valuable::calcCommonVars() const
    {
        if (exp)
            return exp->calcCommonVars();
        else
            return {};
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
                Product p({});
                for(auto&& m: _.as<Product>()){
                    if (!m.FindVa()) {
                        p.Add(std::move(m));
                    } else {
                        LOG_AND_IMPLEMENT("varless for " << *this << " with FindVa in Product")
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

    Valuable Valuable::VaVal(const vars_cont_t& value)
    {
        auto product = std::make_shared<Product>();
        if (value.size()) {
            for (auto& kv : value) {
                product->Add(Exponentiation{kv.first, kv.second});
            }
        }
        return product->Optimized();
    }

    Valuable Valuable::getVaVal() const
    {
        return VaVal(getCommonVars());
    }

    Valuable Valuable::commonVarsWith(const Valuable& value) const {
        return getVaVal().InCommonWith(value.getVaVal());
    }

    const Valuable::vars_cont_t& Valuable::emptyCommonVars()
    {
        static const Valuable::vars_cont_t _;
        return _;
    }

    Valuable::operator bool() const
    {
        return !IsZero();
    }

    Valuable Valuable::operator!() const
    {
        return NotEquals(constants::zero);
    }

    Valuable::operator int() const
    {
        if (exp)
            return exp->operator int();
        else
            LOG_AND_IMPLEMENT("int conversion for " << *this)
    }

    Valuable::operator a_int() const
    {
        if (exp)
            return exp->operator a_int();
        else
            LOG_AND_IMPLEMENT("a_int conversion for " << *this)
    }

    a_int& Valuable::a()
    {
        if (exp)
            return exp->a();
        else
            LOG_AND_IMPLEMENT("a() for " << *this)
    }

    const a_int& Valuable::ca() const
    {
        if (exp)
            return exp->ca();
        else
            LOG_AND_IMPLEMENT("Implement " << boost::core::demangle(Type().name()) << "::ca() for " << *this);
    }

    Valuable::operator uint64_t() const
    {
        if (exp)
            return exp->operator uint64_t();
        else
            LOG_AND_IMPLEMENT("uint64_t conversion for " << *this)
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
            LOG_AND_IMPLEMENT("long double conversion for " << *this)
    }

    Valuable::operator a_rational() const
    {
        if (exp)
            return exp->operator a_rational();
        else
            LOG_AND_IMPLEMENT(*this << " operator a_rational");
    }

    Valuable::operator uint32_t() const
    {
        if (exp)
            return exp->operator uint32_t();
        else
            LOG_AND_IMPLEMENT("uint32_t conversion for " << *this)
    }

    Valuable::operator unsigned char() const
    {
        if (exp)
            return exp->operator unsigned char();
        else
            LOG_AND_IMPLEMENT("unsigned char conversion for " << *this)
    }

    Valuable Valuable::Equals(const Valuable& v) const {
        auto copy = *this;
        return copy.equals(v);
    }

    Valuable Valuable::NotEquals(const Valuable& than) const {
        Product either = {Less(than), than.Less(*this)};
        either.MarkAsOptimized(); // Fixme: either optimized into 0
        return either;
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

    Valuable& Valuable::logic_and(const Valuable& expression)
    {
        if (HasSameVars(expression)) {
            gcd(expression);
        } else {
            sq() += expression.Sq(); // equal to zero if both are equal to zero only
        }
        return *this;
    }

    Valuable Valuable::LogicAnd(const Valuable& expression) const
    {
        auto copy = *this;
        return copy.logic_and(expression);
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
        VarHost::NonZeroLogOffScope off;
        auto intersection = Intersect(with);
        if(!intersection.HasVa(va))
            intersection = va.Equals(GCD(with)(va));
        return intersection;
    }
    Valuable& Valuable::intersect(const Valuable& with) {
        VarHost::NonZeroLogOffScope off;
        return gcd(with);
    }
    Valuable Valuable::Intersect(const Valuable& with) const {
        VarHost::NonZeroLogOffScope off;
        return GCD(with);
    }

    Valuable& Valuable::unionize(const Valuable& with) {
        VarHost::NonZeroLogOffScope off;
        auto intersection = Intersect(with);
		return operator*=(with) /= intersection;
	}

    Valuable Valuable::Union(const Valuable& with) const {
        VarHost::NonZeroLogOffScope off;
        auto intersection = Intersect(with);
        return *this * with / intersection;
    }

    Valuable& Valuable::remove(const Valuable& v) {
        VarHost::NonZeroLogOffScope off;
        for (auto gcd = GCD(v);
            !gcd.IsZero() && gcd != constants::one;
            gcd.gcd(*this))
        {
			operator/=(gcd);
		}
        return *this;
    }

    Valuable Valuable::Remove(const Valuable& expression) const {
        auto copy = *this;
        return copy.remove(expression);
    }

    Valuable Valuable::RootSetDifference(const Valuable& v) const {
		auto t = *this;
		t.remove(v);
		return t;
	}

    Valuable Valuable::RootsSymetricDifference(const Valuable& v) const {
        auto setSymmetricDiff = *this * v;
        setSymmetricDiff.remove(GCD(v));
        return setSymmetricDiff;
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

    Valuable Valuable::BoolNot() const
	{
		return constants::one - *this;
	}

	Valuable Valuable::BoolIntModNotZero() const
	{
        if (exp)
            return exp->BoolIntModNotZero();
        else
            return IntMod_Sign().sq();
	}

    Valuable Valuable::IntMod_IsNegativeOrZero() const {
        if (exp)
            return exp->IntMod_IsNegativeOrZero();
        else
            return Equals(constants::zero)
                || ((*this + constants::minus_1) % *this).Equals(constants::minus_1);
    }

    Valuable Valuable::IntMod_Negative() const
	{
        if (exp)
            return exp->IntMod_Negative();
        else
            // 1 - (*this % (*this - 1)) * ((*this - 1) % *this);
			return (*this * constants::minus_1).IntMod_IsPositive();
	}

    Valuable Valuable::IntMod_Sign() const
	{
        if (exp)
            return exp->IntMod_Sign();
        else
            // ((*this % a) - *this) * -1 / a;
			return IntMod_IsPositive().ToBool() - IntMod_Negative().ToBool();
	}

    Valuable Valuable::IntMod_IsPositive() const
	{
        if (exp)
            return exp->IntMod_IsPositive();
        else {
			return Equals(constants::one)
				|| Equals(constants::two)
				|| (*this % (*this + constants::minus_1)).Equals(constants::one);
		}
    }

    Valuable Valuable::ToBool() const
	{
        if (exp)
            return exp->ToBool();
        else {
            OptimizeOff off;
            auto delta = constants::one - Sign().Abs();
            delta.MarkAsOptimized();
            return delta; // https://math.stackexchange.com/a/2063238/118612
        }
	}

    Valuable Valuable::IfzToBool() const {
        return Ifz(constants::one, constants::zero);
    }

    Valuable Valuable::IntMod_Less(const Valuable& than) const
	{
        if (exp)
            return exp->IntMod_Less(than);
        else {
            //return //(than - *this).IntMod_IsPositive(); // evaluated to
                   //(-1*(Y^2)-1*(X^2)+(Y^2)*((-1*X+Y)%(-1*X+Y-1))+(X^2)*((-1*X+Y)%(-1*X+Y-1))-2*Y*X*((-1*X+Y)%(-1*X+Y-1))+2*Y*X+3*X*((-1*X+Y)%(-1*X+Y-1))-3*Y*((-1*X+Y)%(-1*X+Y-1))+3*Y-3*X+2*((-1*X+Y)%(-1*X+Y-1))-2)
			// https://www.wolframalpha.com/input?i=%28-1*%28Y%5E2%29-1*%28X%5E2%29%2B%28Y%5E2%29*%28%28-1*X%2BY%29%25%28-1*X%2BY-1%29%29%2B%28X%5E2%29*%28%28-1*X%2BY%29%25%28-1*X%2BY-1%29%29-2*Y*X*%28%28-1*X%2BY%29%25%28-1*X%2BY-1%29%29%2B2*Y*X%2B3*X*%28%28-1*X%2BY%29%25%28-1*X%2BY-1%29%29-3*Y*%28%28-1*X%2BY%29%25%28-1*X%2BY-1%29%29%2B3*Y-3*X%2B2*%28%28-1*X%2BY%29%25%28-1*X%2BY-1%29%29-2%29
            OptimizeOff off;
            Product less;
            less.MarkAsOptimized();
            less.Add(*this - than + constants::one);
            less.Add(*this - than + constants::two);
            less.Add(((than - *this) % (than - *this - constants::one)) - constants::one);
            return less;
        }
			//
			// ChatGPT simplified to:
			// X^2 - Y^2 - X + Y + 1
			// which seem less valid but lets check
	}

    Valuable Valuable::NegativeOrZero() const
    {
        if (exp)
            return exp->NegativeOrZero();
        else {
            return Equals(Minimum(constants::zero));
        }
    }

    Valuable Valuable::IsNegativeThan(const Valuable& than) const {
        return LessOrEqual(than) / Equals(than);
    }

    Valuable Valuable::IsNegative() const {
        return (Abs() / *this).equals(constants::minus_1);
    }

    Valuable Valuable::IsPositive() const {
        return (Abs() / *this).equals(constants::one);
    }

    Valuable Valuable::Less(const Valuable& than) const
    {
        auto negative = *this - than;  //  this < than   <=>   this-than < 0
        return negative.IsNegative();
    }

    Valuable Valuable::LessOrEqual(const Valuable& than) const
    {
        auto lessOrEqual = Minimum(than) - *this;
        lessOrEqual.SetView(View::Equation);
        return lessOrEqual;
    }

    Valuable Valuable::GreaterOrEqual(const Valuable& than) const
    {
        return than.LessOrEqual(*this);
    }

    Valuable Valuable::Distance(const Valuable& with) const {
        return (with - *this).abs();
    }

    Valuable Valuable::Minimum(const Valuable& second) const {
//        // Initial Source left for reference (deduced from quadratic discriminant):
//        auto a = constants::one;
//        auto b = -(second + *this);
//        auto c = second * *this;
//#if !defined(NDEBUG) && !defined(NOOMDEBUG)
//        auto v = FindVa();
//        if (!v) {
//            v = second.FindVa();
//        }
//        static DECL_VA(X);
//        auto host = v ? v->getVaHost() : X.getVaHost();
//        auto x = v ? host->Host(host->NewVarId()) : X;
//
//        auto quadratic = Equals(x) || x.Equals(second);
//        auto& sum = quadratic.as<Sum>();
//        std::vector<Valuable> coefficients;
//        auto grade = sum.FillPolyCoeff(coefficients, x);
//        if (grade != 2)
//            IMPLEMENT
//        if (coefficients.size() != 3)
//            IMPLEMENT
//        auto ok = a == coefficients[2] && b == coefficients[1] && c == coefficients[0];
//        if (!ok) {
//            ok = a == -coefficients[2] && b == -coefficients[1] && c == -coefficients[0];
//            if (!ok) {
//                if (a != -coefficients[2]) {
//                    LOG_AND_IMPLEMENT("Check comparator: " << a << "!=" << -coefficients[2]);
//                }
//                if (b != -coefficients[1]) {
//                    LOG_AND_IMPLEMENT("Check comparator: " << b << "!=" << -coefficients[1]);
//                }
//                if (c != -coefficients[0]) {
//                    LOG_AND_IMPLEMENT("Check comparator: " << c << "!=" << -coefficients[0]);
//                }
//            }
//        }
//        if (!ok) {
//            LOG_AND_IMPLEMENT(a << "!=" << coefficients[2] << ", " << b << "!=" << coefficients[1] << ", or " << c
//                                << "!=" << coefficients[0] << " for " << quadratic << " = 0");
//
//        }
//#endif
//        auto discriminant = b.Sq() - a * c * 4;
//        auto sqrt = discriminant.Sqrt();
//        auto xmin = (-b - sqrt) / (a * 2);
//#if !defined(NDEBUG) && !defined(NOOMDEBUG)
//        if (IsSimple() && second.IsSimple()) {
//            if (std::min(*this, second) != xmin)
//                IMPLEMENT
//        }
//#endif
//
//        return xmin;


        // simplified to formula:
        // (Y + X -sqrt(Y^2 + X^2 -2YX))/2
        // expressed through Abs:
        // (Y + X -sqrt((Y-X)^2))/2 = (X+Y-|X-Y|)/2
        return ((second + *this) - Distance(second)) / constants::two;
    }

    Valuable Valuable::Maximum(const Valuable& with) const {
        return ((with + *this) + Distance(with)) / constants::two;
    }

    Valuable Valuable::For(const Valuable& initialValue, const Valuable& lambda) const
    {
        LOG_AND_IMPLEMENT("For loop with initialValue " << initialValue << " and lambda " << lambda)
    }

    Valuable Valuable::MustBeInt() const {
        if (exp)
            return exp->MustBeInt();
        else {
			using namespace constants;
			return
				//one.Equals(*this % one);
				//or
				(e ^ (*this * 2 * i * pi)) - 1;   // FIXME: check if all required optimizations ready to make this to be useful https://math.stackexchange.com/a/1598552/118612
		}
    }

    std::function<bool(std::initializer_list<Valuable>)> Valuable::Functor() const {
        if (exp)
            return exp->Functor();
        else
            LOG_AND_IMPLEMENT("Functor for " << *this)
    }

    Valuable Valuable::bit(const Valuable& n) const
    {
        if (exp)
            return exp->bit(n);
        else if (n > 0) {
            return Shr().bit(n-1);
        } else if (n.IsZero()){
            // bit0 = (this & 1) == (this % 2) == ((-1)^this)-1)/-2
            // for this=0: ((-1)^0)-1)/-2 = 0
            // this=1: ((-1^1)-1)/-2 = 1
            // this=2: ((-1^2)-1)/-2 = 0
            // ...
            // TODO: this=-1:
            // TODO: this=-2:
            // TODO: this=-3:
            // ...
            auto bit0 = constants::minus_1 ^ *this;
            --bit0;
            bit0 /= -2;
            return bit0;
        }else
            LOG_AND_IMPLEMENT("bit(" << n << ") for " << *this);
    }

    Valuable Valuable::bits(int n, int l) const
    {
        if(n<0)
            LOG_AND_IMPLEMENT("bits(" << n << ", " << l << ") for negative n in " << *this)
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
        auto s = constants::zero;
        {
            OptimizeOff off;
            auto a = *this;
            auto b = v;
            for (Valuable i; i < n;) {
                auto ab = a.bit();
                auto bb = b.bit();
                auto mul = ab * bb;
                s += mul.shl(i);
                if (++i < n) {
                    a -= ab;
                    a /= constants::two;
                    b -= bb;
                    b /= constants::two;
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
        auto s = constants::zero;
        {
            OptimizeOff off;
            auto a = *this;
            auto b = v;
            for (Valuable i; i < n;) {
                auto ab = a.bit();
                auto bb = b.bit();
                auto mul = ab * bb;
                auto sum = ab + bb;
                s += (sum - mul).shl(i);
                if (++i < n) {
                    a -= ab;
                    a /= constants::two;
                    b -= bb;
                    b /= constants::two;
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
        auto s = constants::zero;
        {
            OptimizeOff off;
            for (auto i = n; i--;) {
                s *= constants::two;
                auto _1 = bit(i);
                auto _2 = v.bit(i);
                s += (_1 + _2).bit();
            }
        }
        if (bit_operation_optimizations) {
            s.optimize();
        }
        return s;
    }

    Valuable Valuable::Not(const Valuable& n) const
    {
        auto s = constants::zero;
        {
            OptimizeOff off;
            for (auto i = n; i--;) {
                s *= constants::two;
                auto _1 = constants::one - bit(i);
                s += (_1).bit();
            }
        }
        if (bit_operation_optimizations) {
            s.optimize();
        }
        return s;
    }

    Valuable& Valuable::shl() {
        return exp
            ? exp->shl()
            : *this *= constants::two;
    }

    Valuable& Valuable::shl(const Valuable& n) {
        return exp && n.IsInt()
            ? exp->shl(n)
            : *this *= constants::two ^ n;
    }

    Valuable& Valuable::shr(const Valuable& n)
    {
        if(!n.IsInt()){
            IMPLEMENT
        } else if (exp)
            return exp->shr(n);
        else if (n > constants::one)
            return shr(n + constants::minus_1).shr();
        else if (n != constants::zero)
            return shr();
        else
            return *this;
    }

    Valuable& Valuable::shr()
    {
        return exp
            ? exp->shr()
            : operator+=(-bit(constants::zero)).operator/=(constants::two);
    }

    Valuable Valuable::Shl() const
    {
        return exp
            ? exp->Shl()
            : *this * constants::two;
    }

    Valuable Valuable::Shl(const Valuable& n) const
    {
        return exp && n.IsInt()
            ? exp->Shl(n)
            : *this * (constants::two ^ n);
    }

    Valuable Valuable::Shr(const Valuable& n) const
    {
        auto v = Valuable(get());
        return v.shr(n);
    }

    Valuable Valuable::Shr() const {

        return exp
            ? exp->Shr()
            : (*this - bit(constants::zero)) / constants::two;
    }

    Valuable Valuable::Cyclic(const Valuable& total, const Valuable& shiftLeft) const
    {
        if (exp)
            return exp->Cyclic(total, shiftLeft);
        auto s = constants::zero;
        {
            OptimizeOff off;
            for (auto i = total; i--;) {
                auto shi = i + shiftLeft;
                if (shi >= total)
                    shi -= total;
                else if (shi < constants::zero)
                    shi += total;
                s += bit(i).shl(shi);
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

    void Valuable::MarkNotOptimized() {
        if (exp)
            exp->MarkNotOptimized();
        else
            optimized = {};
    }

    bool Valuable::is_optimized() const {
        return exp
            ? exp->is_optimized()
            : optimized;
    }

    Valuable Valuable::Optimized(Valuable::View view) const {
        Valuable copy(Clone());
        copy.SetView(view);
        if (!copy.is_optimized()) {
            OptimizeOn on;
            copy.optimize();
        }
        return copy;
    }

    Valuable Valuable::Optimized() const {
        return Optimized(GetView());
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

}

namespace std
{
    ::omnn::math::Valuable abs(const ::omnn::math::Valuable& v)
    {
        return v.abs();
    }

    ::omnn::math::Valuable log(const ::omnn::math::Valuable& value)
    {
        return ::omnn::math::Logarithm(::omnn::math::constant::e, value);
    }

    ::omnn::math::Valuable sqrt(const ::omnn::math::Valuable& v)
    {
        return v.Sqrt();
    }

    ::omnn::math::Valuable tanh(const ::omnn::math::Valuable& value)
    {
        return value.Tanh();
    }

    ::omnn::math::Valuable pow(const ::omnn::math::Valuable& base, const ::omnn::math::Valuable& exp)
    {
        return base ^ exp;
    }

    bool equal_to<::omnn::math::Valuable>::operator()(const ::omnn::math::Valuable& v1,
                                                      const ::omnn::math::Valuable& v2) const
    {
        return v1.Same(v2);
    }

} // namespace std

::omnn::math::Valuable operator"" _v(const char* v, std::size_t l)
{
    using namespace ::omnn::math;
    static auto StrVaHost = VarHost::Global<std::string>().shared_from_this();
    Valuable::OptimizeOff off;
    return {{v, l}, StrVaHost, {}};
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
