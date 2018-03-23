//
// Created by Сергей Кривонос on 01.09.17.
//

#pragma once
#include "Valuable.h"

namespace omnn{
namespace math {

    template <class Chld>
    class ValuableDescendantContract
            : public Valuable
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
            return new Chld(*static_cast<const Chld*>(this));
        }
    
        int getTypeSize() const override { return sizeof(Chld); }
        
        Valuable* Move() override
        {
            return new Chld(std::move(*static_cast<Chld*>(this)));
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
        ValuableDescendantContract() : Valuable(ValuableDescendantMarker()) {}
        ValuableDescendantContract(ValuableDescendantContract&& c)//        =default;
        : Valuable(std::move(c), ValuableDescendantMarker()) {}
        ValuableDescendantContract(const ValuableDescendantContract& c)//        =default;
         : Valuable(c, ValuableDescendantMarker()) {}
        ValuableDescendantContract& operator=(const ValuableDescendantContract& f) { hash = f.Hash(); return *this; }
        ValuableDescendantContract& operator=(ValuableDescendantContract&& f) { hash = f.Hash(); return *this; }
        
//        operator Valuable&&() {
//            return std::move(Valuable(Move()));
//        }
        
        static const Chld* cast(const Valuable& v){
            return Valuable::cast<Chld>(v);
        }

        static Chld* cast(Valuable& v){
            return Valuable::cast<Chld>(v);
        }
        
        Valuable operator -() const override {
            return *this * -1;
        }
        
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
        Valuable sqrt() const override { throw "Implement!"; }
        
        bool IsComesBefore(const Valuable& v) const override
        {
            return *this > v;
        }
    };
}}
