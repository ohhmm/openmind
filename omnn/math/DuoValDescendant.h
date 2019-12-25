/*
 * DuoValDescendant.h
 *
 *  Created on: Sep 30, 2018
 *      Author: sergejkrivonos
 */
#pragma once
#include <omnn/math/ValuableDescendantContract.h>

namespace omnn::math {

    std::ostream& PrintVal(std::ostream&, const Valuable&);

    template <class Chld>
    class DuoValDescendant
            : public ValuableDescendantContract<Chld>
    {
        using base = ValuableDescendantContract<Chld>;

    protected:
        Valuable _1, _2;

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
        std::ostream& code(std::ostream& out) const override
        {
            out << '(';
            PrintVal(out, _1);
            code_sign(out);
            PrintVal(out, _2);
            out << ')';
            return out;
        }

    public:
        static max_exp_t getMaxVaExp(const Valuable& _1, const Valuable& _2) {
            if(_1.FindVa() || _2.FindVa())
                IMPLEMENT // in Child
            return {};
        }

        const Valuable& get1() const { return _1; }
        const Valuable& get2() const { return _2; }
        
        using base::base;

        DuoValDescendant(const Valuable& v1, const Valuable& v2)
        : _1(v1), _2(v2)
        {
            Valuable::hash = _1.Hash() ^ _2.Hash();
            Valuable::maxVaExp = Chld::getMaxVaExp(_1, _2);
        }

        bool operator ==(const Valuable& v) const override{
            const Chld* ch;
            return Valuable::Hash()==v.Hash()
                    && v.Is<Chld>()
                    && (ch = Chld::cast(v))
                    && _1.Hash() == ch->_1.Hash()
                    && _2.Hash() == ch->_2.Hash()
                    && _1 == ch->_1
                    && _2 == ch->_2;
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
        void CollectVaNames(std::map<std::string, Variable>& s) const override {
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

