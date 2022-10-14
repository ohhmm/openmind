/*
 * DuoValDescendant.h
 *
 *  Created on: Sep 30, 2018
 *      Author: sergejkrivonos
 */
#pragma once
#include <omnn/math/ValuableDescendantContract.h>
#include <utility>

namespace omnn::math {

    std::ostream& PrintVal(std::ostream&, const Valuable&);
    Valuable Multiply(const Valuable&, const Valuable&);

    template <class Chld>
    class DuoValDescendant
            : public ValuableDescendantContract<Chld>
    {
        using base = ValuableDescendantContract<Chld>;

    protected:
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
            PrintVal(out, _1);
            code_sign(out);
            PrintVal(out, _2);
            out << ')';
            return out;
        }

        static max_exp_t getMaxVaExp(const Valuable& _1, const Valuable& _2) {
            if(_1.FindVa() || _2.FindVa())
                IMPLEMENT // in Child
            return {};
        }

        const Valuable& get1() const { return _1; }
        template<class T>
        void set1(T&& p) {
            _1 = std::forward<T>(p);
            Valuable::hash = _1.Hash() ^ _2.Hash();
        }
        template<class T>
        void update1(T&& p) {
            Valuable::hash ^= _1.Hash();
            _1 = std::forward<T>(p);
            Valuable::hash ^= _1.Hash();
        }
        void update1(std::function<void(decltype(_1)&)>& f) {
            Valuable::hash ^= _1.Hash();
            f(_1);
            Valuable::hash ^= _1.Hash();
        }
        
        const Valuable& get2() const { return _2; }
        template<class T>
        void set2(T&& p) {
            _2 = std::forward<T>(p);
            Valuable::hash = _1.Hash() ^ _2.Hash();
        }
        template<class T>
        void update2(T&& p) {
            Valuable::hash ^= _2.Hash();
            _2 = std::forward<T>(p);
            Valuable::hash ^= _2.Hash();
        }
        void update2(std::function<void(decltype(_2)&)>& f) {
            Valuable::hash ^= _2.Hash();
            f(_2);
            Valuable::hash ^= _2.Hash();
        }
        
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

        bool operator ==(const Valuable& v) const override{
            auto eq = v.Is<Chld>() && Valuable::hash == v.Hash();
            if (eq) {
                auto& ch = v.as<Chld>();
                eq = _1.Hash() == ch._1.Hash()
                    && _2.Hash() == ch._2.Hash()
                    && _1 == ch._1
                    && _2 == ch._2;
            }
            return eq;
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
                // or
                //            Valuable::hash = _1.Hash() ^ _2.Hash();
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

        max_exp_t getMaxVaExp()  const override {
            return Chld::getMaxVaExp(_1, _2);
        }
        
        bool IsSimple() const override {
            return IsMultival() == Valuable::YesNoMaybe::No
                && _1.IsSimple() && _2.IsSimple();
        }
        
        Valuable::YesNoMaybe IsMultival() const override {
            return _1.IsMultival() || _2.IsMultival();
        }
        
        void Values(const std::function<bool(const Valuable&)>& f) const override
        {
            if (f) {
                if(this->IsSimpleFraction()){
                    f(*this);
                    return;
                }
                auto cache = Valuable::optimized;
                // TODO: multival caching (inspect all optimized and optimization transisions) auto isCached = cach
                
                
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
    };
}

