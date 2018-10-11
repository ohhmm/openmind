//
// Created by Сергей Кривонос on 01.09.17.
//

#pragma once
#include <omnn/math/Valuable.h>

namespace omnn{
namespace math {

    class ValuableDescendantBase : public Valuable
    {
    public:
        using Valuable::Valuable;
        Valuable& operator^=(const Valuable& v) override;
        Valuable& operator+=(const Valuable &v) override;
        Valuable& operator*=(const Valuable &v) override;
        Valuable& operator/=(const Valuable &v) override;
        Valuable& operator%=(const Valuable &v) override;
    };

    template <class Chld>
    class ValuableDescendantContract
            : public ValuableDescendantBase
    {
#ifdef BOOST_TEST_MODULE
    public:
#endif
        using self = ValuableDescendantContract;
        friend Chld;
        friend Valuable;
        
        class DepSz {
        public:
            DepSz(self* ths) {
                ths->setAllocSize(sizeof(Chld));
            }
        };
        DepSz depSz = this;
        
        
#ifndef BOOST_TEST_MODULE
    protected:
#endif
        Valuable* Clone() const override
        {
            return static_cast<Valuable*>(new Chld(*static_cast<const Chld*>(this)));
        }
    
        int getTypeSize() const override { return sizeof(Chld); }
        
        Valuable* Move() override
        {
            return static_cast<Valuable*>(new Chld(std::move(*static_cast<Chld*>(this))));
        }

        void New(void* addr, Valuable&& v) override {
            auto ch = const_cast<Chld*>(Chld::cast(v));
            new(addr) Chld(std::move(*ch));
        }
        
    public:
        // once compiler allow
        // todo :
        //ValuableDescendantContract() : Valuable<>() {}
        // instead of
        ValuableDescendantContract() : ValuableDescendantBase(ValuableDescendantMarker()) {}
        ValuableDescendantContract(ValuableDescendantContract&& c)//        =default;
        : ValuableDescendantBase(std::move(c), ValuableDescendantMarker()) {}
        ValuableDescendantContract(const ValuableDescendantContract& c)//        =default;
         : ValuableDescendantBase(c, ValuableDescendantMarker()) {}
        ValuableDescendantContract& operator=(const ValuableDescendantContract& v) {
            hash = v.hash;
            optimized = v.optimized;
            return *this;
        }
        ValuableDescendantContract& operator=(ValuableDescendantContract&& v) {
            hash = v.hash;
            optimized = v.optimized;
            return *this;
        }
        
//        operator Valuable&&() {
//            return std::move(Valuable(Move()));
//        }

        bool is(const std::type_index& ti) const override {
            return ti==std::type_index(typeid(Chld));
        }

        static const Chld* cast(const Valuable& v){
            return Valuable::cast<Chld>(v);
        }

        static Chld* cast(Valuable& v){
            return Valuable::cast<Chld>(v);
        }
        
        Valuable operator -() const override {
            return *this * -1;
        }

        Valuable& operator --() override { optimized={}; return *this+=-1_v; }
        Valuable& operator ++() override { optimized={}; return *this+=1_v; }

        Valuable abs() const override
        {
            auto i = const_cast<Chld*>(cast(*this));
            if(*i < 0_v)
            {
                return *cast(-*i);
            }
            return *this;
        }
        void optimize() override { }
        //void expand() override { }
        Valuable sqrt() const override { IMPLEMENT }
        Valuable& sq() override { return *this *= *this; }

        bool IsComesBefore(const Valuable& v) const override
        {
            return *this > v;
        }
    };
}}
