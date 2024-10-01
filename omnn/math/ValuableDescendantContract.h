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

        bool IsSurd() const override { return {}; }

        Valuable& operator^=(const Valuable& v) override;
        Valuable& operator+=(const Valuable &v) override;
        Valuable& operator*=(const Valuable &v) override;
        Valuable& operator/=(const Valuable &v) override;
        Valuable& operator%=(const Valuable &v) override;
        
        Valuable operator -() const override;
        
        constexpr Valuable& operator --() override { return operator+=(constants::minus_1); }
        constexpr Valuable& operator ++() override { return operator+=(constants::one); }

        Valuable& sq() override { return *this *= *this; }

        void optimize() override { }
        //void expand() override { }
        Valuable Sqrt() const override;

        bool IsComesBefore(const Valuable& v) const override
        {
            return *this > v;
        }
        
        void Values(const std::function<bool(const Valuable&)> &fun) const override;

        Valuable Univariate() const override;
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
            constexpr DepSz(self* ths) {
                ths->setAllocSize(sizeof(Chld));
#ifndef NOOMDEBUG
                assert(DefaultAllocSize >= sizeof(Chld) && "Increase DefaultAllocSize");
#endif
            }
        };
        DepSz depSz = this;
        
        
#ifndef BOOST_TEST_MODULE
    protected:
#endif

        NO_APPLE_CONSTEXPR auto CPtr() const noexcept {
            return reinterpret_cast<const Chld*>(this);
        }

        NO_APPLE_CONSTEXPR auto Ptr() noexcept {
            return reinterpret_cast<Chld*>(this);
        }

        [[nodiscard]]
        NO_APPLE_CONSTEXPR auto& Ref() noexcept { return *Ptr(); }

        NO_CLANG_CONSTEXPR Valuable* Clone() const override
        {
            return new Chld(*CPtr());
        }
    
        NO_CLANG_CONSTEXPR size_t getTypeSize() const override { return sizeof(Chld); }
        
        Valuable* Move() override
        {
            return static_cast<Valuable*>(new Chld(std::move(*Ptr())));
        }

        void New(void* addr, Valuable&& v) override {
            new(addr) Chld(std::move(v.as<Chld>()));
        }
        
    public:
        // once compiler allow
        // todo :
        //ValuableDescendantContract() : Valuable<>() {}
        // instead of
        constexpr ValuableDescendantContract() : ValuableDescendantBase(ValuableDescendantMarker()) {}
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

        std::type_index Type() const override {
        	return typeid(Chld);
        }

        Valuable::encapsulated_instance SharedFromThis() override {
            auto ptr = std::make_shared<Chld>(std::move(as<Chld>()));
            exp = std::static_pointer_cast<Valuable>(ptr);
            return exp;
        }

        Chld& Reset() {
            OptimizeOff off;
            Become(Chld());
            return Ref();
        }
    };
}
}
