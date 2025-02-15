//
// Created by Сергей Кривонос on 01.09.17.
//
#pragma once

// Platform macros must be defined before any other includes
#include <omnn/math/Platform.h>

// Other includes
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

        Valuable& operator --() override { return operator+=(constants::minus_1); }
        Valuable& operator ++() override { return operator+=(constants::one); }

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
        const Chld* CPtr() const noexcept {
            return static_cast<const Chld*>(this);
        }

        [[nodiscard]]
        Chld* Ptr() noexcept {
            return static_cast<Chld*>(this);
        }

        [[nodiscard]]
        Chld& Ref() noexcept {
            return *Ptr();
        }

        [[nodiscard]]
        const Chld& CRef() const noexcept {
            return *CPtr();
        }

        Valuable* Clone() const noexcept override {
            return new Chld(*CPtr());
        }

        [[nodiscard]]
        size_t getTypeSize() const noexcept override { return sizeof(Chld); }

        [[nodiscard]]
        Valuable* Move() noexcept override {
            return static_cast<Valuable*>(new Chld(std::move(*Ptr())));
        }

        void New(void* addr, Valuable&& v) override {
            new(addr) Chld(std::move(v.as<Chld>()));
        }

    public:
        // once compiler allow
        ValuableDescendantContract() noexcept 
            : ValuableDescendantBase(ValuableDescendantMarker()) 
        {
            optimized = true;
            hash = 0;
        }
        
        ValuableDescendantContract(ValuableDescendantContract&& c) noexcept
            : ValuableDescendantBase(std::move(c))
        {
            this->hash = c.hash;
            this->optimized = c.optimized;
        }
        
        ValuableDescendantContract(const ValuableDescendantContract& c) noexcept
            : ValuableDescendantBase(c)
        {
            this->hash = c.hash;
            this->optimized = c.optimized;
        }
        
        ValuableDescendantContract& operator=(const ValuableDescendantContract& v) noexcept {
            if (this != &v) {
                ValuableDescendantBase::operator=(v);
                hash = v.hash;
                optimized = v.optimized;
            }
            return *this;
        }
        
        ValuableDescendantContract& operator=(ValuableDescendantContract&& v) noexcept {
            if (this != &v) {
                ValuableDescendantBase::operator=(std::move(v));
                hash = std::exchange(v.hash, 0);
                optimized = std::exchange(v.optimized, false);
            }
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
