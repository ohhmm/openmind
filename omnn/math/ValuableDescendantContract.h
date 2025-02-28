//
// Created by Сергей Кривонос on 01.09.17.
//
#pragma once
#include <omnn/math/Valuable.h>
#include <omnn/math/platform/Macros.h>

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

        Valuable& sq() override;

        void optimize() override { }
        //void expand() override { }
        Valuable Sqrt() const override;

        universal_lambda_t CompileIntoLambda(variables_for_lambda_t) const override;

        bool IsComesBefore(const Valuable& v) const override;

        void Values(const std::function<bool(const Valuable&)> &) const override;

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

        [[nodiscard]]
        NO_APPLE_CONSTEXPR auto CPtr() const noexcept {
            return reinterpret_cast<const Chld*>(this);
        }

        [[nodiscard]]
        NO_APPLE_CONSTEXPR auto Ptr() noexcept {
            return reinterpret_cast<Chld*>(this);
        }

        [[nodiscard]]
        NO_APPLE_CONSTEXPR Chld& Ref() noexcept {
            return *Ptr();
        }

        [[nodiscard]]
        NO_APPLE_CONSTEXPR const Chld& CRef() noexcept {
            return Ref();
        }

        NO_CLANG_CONSTEXPR Valuable* Clone() const override
        {
            return new Chld(*CPtr());
        }

        [[nodiscard]]
        NO_CLANG_CONSTEXPR size_t getTypeSize() const override { return sizeof(Chld); }

        [[nodiscard]]
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
            return ti == get().Type();
        }

        std::type_index Type() const override {
            return typeid(Chld);
        }

        Valuable::encapsulated_instance SharedFromThis() override {
            auto allocated = getAllocSize();
            auto ptr = std::make_shared<Chld>(std::move(as<Chld>()));
            Valuable::~Valuable(); // ensure proper resource reallocation before reinitializing base part of the object
            new (this) Valuable(std::static_pointer_cast<Valuable>(ptr));
            setAllocSize(allocated);
            return exp;
        }
    };
}
}
