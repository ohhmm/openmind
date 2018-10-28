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
        static a_int getMaxVaExp(const Valuable& _1, const Valuable& _2) {
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

        Valuable& eval(const std::map<Variable, Valuable>& with) override {
            Valuable::optimized={};
            _1.eval(with);
            _2.eval(with);
            //        optimize();
            // or
            Valuable::hash = _1.Hash() ^ _2.Hash();
            return *this;
        }

        void Eval(const Variable& va, const Valuable& v) override {
            Valuable::optimized={};
            _1.Eval(va, v);
            _2.Eval(va, v);
    //        optimize();
           // or
            Valuable::hash = _1.Hash() ^ _2.Hash();
        }

        a_int getMaxVaExp() const override {
            return Chld::getMaxVaExp(_1, _2);
        }
    };
}

