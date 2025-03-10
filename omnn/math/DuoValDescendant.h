/*
 * DuoValDescendant.h
 *
 *  Created on: Sep 30, 2018
 *      Author: sergejkrivonos
 */
#pragma once
#include <omnn/math/ValuableDescendantContract.h>

#include <omnn/math/Cache.h>

#include <omnn/rt/antiloop.hpp>

#include <utility>


namespace omnn::math {

    std::ostream& PrintVal(std::ostream&, const Valuable&);
	std::ostream& PrintCodeVal(std::ostream&, const Valuable&);
    Valuable Multiply(const Valuable&, const Valuable&);

    template <typename Type>
    concept BinaryOperationLambdaTemplateRequirement = requires(Type object) {
        decltype(object)::GetBinaryOperationLambdaTemplate();
        { decltype(object)::GetBinaryOperationLambdaTemplate()(constants::one, constants::one) } -> std::same_as<Valuable>;
        { decltype(object)::GetBinaryOperationLambdaTemplate()(1, 1) } -> std::same_as<int>;
        { decltype(object)::GetBinaryOperationLambdaTemplate()(1., 1.) } -> std::same_as<float>;
    };

    template <class Chld>
       //requires BinaryOperationLambdaTemplateRequirement<Chld>
    class DuoValDescendant
            : public ValuableDescendantContract<Chld>
    {
        using base = ValuableDescendantContract<Chld>;

    protected:
        using type = Chld;
        Valuable _1, _2;

        bool IsSubObject(const Valuable& o) const override {
            return this == &o.get() || &_1.get() == &o.get() || &_2.get() == &o.get();
        }

        virtual std::ostream& print_sign(std::ostream& out) const = 0;
        virtual std::ostream& code_sign(std::ostream& out) const {
            return print_sign(out);
        }
        std::ostream& print(std::ostream& out) const override
        {
            out << '(';
            PrintVal(out, _1);
            print_sign(out);
            PrintVal(out, _2);
            out << ')';
            return out;
        }

    public:
        std::ostream& code(std::ostream& out) const override
        {
            out << '(';
            PrintCodeVal(out, _1);
            code_sign(out);
            PrintCodeVal(out, _2);
            out << ')';
            return out;
        }

        static max_exp_t getMaxVaExp(const Valuable& _1, const Valuable& _2) {
            if(_1.FindVa() || _2.FindVa())
                IMPLEMENT // in Child
            return {};
        }

        constexpr const Valuable& get1() const { return _1; }
        template<class T>
        void set1(T&& p) {
            _1 = std::forward<T>(p);
            Valuable::hash = _1.Hash() ^ _2.Hash();
            Valuable::optimized = {};
        }
        template<class T>
        void update1(T&& p) {
            Valuable::hash ^= _1.Hash();
            _1 = std::forward<T>(p);
            Valuable::hash ^= _1.Hash();
            Valuable::optimized = {};
        }
        void update1(std::function<void(decltype(_1)&)>& f) {
            Valuable::hash ^= _1.Hash();
            f(_1);
            Valuable::hash ^= _1.Hash();
            Valuable::optimized = {};
        }

        const Valuable& get2() const { return _2; }
        template<class T>
        void set2(T&& p) {
            _2 = std::forward<T>(p);
            Valuable::hash = _1.Hash() ^ _2.Hash();
            Valuable::optimized = {};
        }
        template<class T>
        void update2(T&& p) {
            Valuable::hash ^= _2.Hash();
            _2 = std::forward<T>(p);
            Valuable::hash ^= _2.Hash();
            Valuable::optimized = {};
        }
        void update2(std::function<void(decltype(_2)&)>& f) {
            Valuable::hash ^= _2.Hash();
            f(_2);
            Valuable::hash ^= _2.Hash();
            Valuable::optimized = {};
        }
        constexpr auto extract1() { return std::move(_1); }
        constexpr auto extract2() { return std::move(_2); }

        using base::base;

        template <class T1, class T2>
        constexpr DuoValDescendant(T1&& v1, T2&& v2)
        : _1(std::forward<T1>(v1))
        , _2(std::forward<T2>(v2))
        {
            Valuable::hash = _1.Hash() ^ _2.Hash();
            Valuable::maxVaExp = Chld::getMaxVaExp(_1, _2);
        }

        Valuable operator-() const override { return Multiply(-1, *this); }

        bool operator==(const Chld& other) const {
            return Valuable::hash == other.Hash()
                && _1 == other._1
                && _2 == other._2;
        }

        bool operator==(const Valuable& other) const override {
            return (other.Is<Chld>() && operator==(other.as<Chld>()))
                || ((other.IsSum() || other.IsProduct()) && other.operator==(*this));
        }

        bool Same(const Valuable& value) const override {
            auto same = this->OfSameType(value) && this->Hash() == value.Hash();
            if (same) {
                auto& other = value.as<type>();
                same = _1.Same(other._1) && _2.Same(other._2);
            }
            return same;
        }

        const Variable* FindVa() const override {
            auto va = _1.FindVa();
            return va ? va : _2.FindVa();
        }

        bool HasVa(const Variable& va) const override {
            return _1.HasVa(va) || _2.HasVa(va);
        }

        void CollectVa(std::set<Variable>& s) const override {
            _1.CollectVa(s);
            _2.CollectVa(s);
        }
        void CollectVaNames(Valuable::va_names_t& s) const override {
            _1.CollectVaNames(s);
            _2.CollectVaNames(s);
        }

        bool eval(const std::map<Variable, Valuable>& with) override {
            auto evaluated = _1.eval(with);
            evaluated = _2.eval(with) || evaluated;
            if(evaluated){
                Valuable::optimized={};
                this->optimize();
            }
            return evaluated;
        }

        void Eval(const Variable& va, const Valuable& v) override {
            Valuable::optimized={};
            _1.Eval(va, v);
            _2.Eval(va, v);
    //        optimize();
           // or
            Valuable::hash = _1.Hash() ^ _2.Hash();
        }

        Valuable::universal_lambda_t CompileIntoLambda(Valuable::variables_for_lambda_t vars) const override {
            return [
                lambda1 = _1.CompileIntoLambda(vars),
                lambda2 = _2.CompileIntoLambda(vars)
                ]
                (auto params)
                {
                    return Chld::GetBinaryOperationLambdaTemplate()(lambda1(params), lambda2(params));
                };
        }

        operator a_rational() const override {
            return static_cast<a_rational>(Chld::GetBinaryOperationLambdaTemplate()(_1, _2));
        }

        max_exp_t getMaxVaExp() const override {
            return Chld::getMaxVaExp(_1, _2);
        }

        bool IsSimple() const override {
            return IsMultival() == YesNoMaybe::No
                && _1.IsSimple() && _2.IsSimple();
        }

        YesNoMaybe IsRational() const override {
            return _1.IsRational() && _2.IsRational();
        }

        bool IsNaN() const override {
            return _1.IsNaN() || _2.IsNaN();
        }

        YesNoMaybe IsMultival() const override {
            return _1.IsMultival() || _2.IsMultival();
        }

        void Values(const std::function<bool(const Valuable&)>& f) const override
        {
            if (f) {
                if(this->IsSimpleFraction()){
                    f(*this);
                    return;
                }

                // TODO: multivalve caching (inspect all optimized and optimization transitions)


                Valuable::solutions_t vals, thisValues;
                _1.Values([&](auto& thisVal){
                    thisValues.insert(thisVal);
                    return true;
                });

                _2.Values([&](auto&vVal){
                    for(auto& tv:thisValues){
                        Valuable v = Chld{tv,vVal};
                        v.optimize();
                        vals.insert(std::move(v));
                    }
                    return true;
                });

                for(auto& v:vals)
                    f(v);
            }
        }

        a_int Complexity() const override {
            return _1.Complexity() + _2.Complexity();
        }


        Valuable::solutions_t Distinct() const override {
            Valuable::solutions_t branches;
            for (auto&& f : _1.Distinct()) {
                for (auto&& s : _2.Distinct()) {
                    branches.emplace(Chld{f, s});
                }
            }
            return branches;
        }

        [[nodiscard]]
        Valuable varless() const override {
            return Chld::GetBinaryOperationLambdaTemplate()(_1.varless(), _2.varless());
        }
    };
}

#if !defined(NDEBUG) && !defined(NOOMDEBUG)
#define DUO_OPT_PFX                                                                                                    \
    if (!optimizations && !IsSimple()) {                                                                               \
        hash = _1.Hash() ^ _2.Hash();                                                                                  \
        return;                                                                                                        \
    }                                                                                                                  \
    if (optimized) {                                                                                                   \
        auto h = _1.Hash() ^ _2.Hash();                                                                                \
        if (h != hash) {                                                                                               \
            LOG_AND_IMPLEMENT("Fix hash updating for " << *this);                                                      \
        }                                                                                                              \
        return;                                                                                                        \
    }                                                                                                                  \
    ANTILOOP(base::type)
#else
#define DUO_OPT_PFX                                                                                                    \
    if (!optimizations && !IsSimple()) {                                                                               \
        hash = _1.Hash() ^ _2.Hash();                                                                                  \
        return;                                                                                                        \
    }                                                                                                                  \
    if (optimized) {                                                                                                   \
        return;                                                                                                        \
    }                                                                                                                  \
    ANTILOOP(base::type)
#endif

#define DUO_USE_CACHE(name) auto doCheckCache = _1.Complexity() + _2.Complexity() > 10; USE_CACHE(name);
