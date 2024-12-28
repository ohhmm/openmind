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
        
        virtual Valuable* Clone() const override = 0;
        virtual Valuable* Move() override = 0;
        
        virtual bool IsComesBefore(const Valuable& v) const override;
        virtual universal_lambda_t CompileIntoLambda(variables_for_lambda_t) const override;
        
        virtual Valuable& operator^=(const Valuable& v) override;
        virtual Valuable& operator+=(const Valuable& v) override;
        virtual Valuable& operator*=(const Valuable& v) override;
        virtual Valuable& operator/=(const Valuable& v) override;
        virtual Valuable& operator%=(const Valuable& v) override;
        virtual Valuable operator-() const override;
        virtual void Values(const std::function<bool(const Valuable&)>& fun) const override;
        virtual Valuable Univariate() const override;
        virtual Valuable& sq() override;
        virtual Valuable Sqrt() const override;
    };

    template<typename ChildT>
    class ValuableDescendantContract : public ValuableDescendantBase
    {
    protected:
        using self = ValuableDescendantContract<ChildT>;
        friend ChildT;
        friend Valuable;

        class DepSz {
        public:
            DepSz(self* ths) {
                ths->setAllocSize(sizeof(ChildT));
#ifndef NOOMDEBUG
                assert(DefaultAllocSize >= sizeof(ChildT) && "Increase DefaultAllocSize");
#endif
            }
        };
        DepSz depSz = this;

#ifndef BOOST_TEST_MODULE
    protected:
#endif

        [[nodiscard]]
        auto CPtr() const noexcept {
            return reinterpret_cast<const ChildT*>(this);
        }

        [[nodiscard]]
        auto Ptr() noexcept {
            return reinterpret_cast<ChildT*>(this);
        }

        [[nodiscard]]
        auto& Ref() noexcept { return *Ptr(); }

        [[nodiscard]]
        size_t getTypeSize() const override { return sizeof(ChildT); }

        void New(void* addr, Valuable&& v) override {
            new(addr) ChildT(std::move(v.template as<ChildT>()));
        }

    public:
        ValuableDescendantContract() : ValuableDescendantBase(ValuableDescendantMarker()) {}
        
        ValuableDescendantContract(ValuableDescendantContract&& c)
            : ValuableDescendantBase(std::move(c), ValuableDescendantMarker()) {}
            
        ValuableDescendantContract(const ValuableDescendantContract& c)
            : ValuableDescendantBase(c, ValuableDescendantMarker()) {}
            
        ValuableDescendantContract& operator=(const ValuableDescendantContract& v) {
            if (this != &v) {
                this->~ValuableDescendantContract();
                new (this) ValuableDescendantContract(v);
            }
            return *this;
        }
        
        ValuableDescendantContract& operator=(ValuableDescendantContract&& v) {
            if (this != &v) {
                this->~ValuableDescendantContract();
                new (this) ValuableDescendantContract(std::move(v));
            }
            return *this;
        }

        Valuable* Clone() const override
        {
            return new ChildT(*CPtr());
        }

        Valuable* Move() override
        {
            return static_cast<Valuable*>(new ChildT(std::move(*Ptr())));
        }

        bool is(const std::type_index& ti) const override {
            return ti == this->Type();
        }

        std::type_index Type() const override {
            return typeid(ChildT);
        }

        Valuable::encapsulated_instance SharedFromThis() override {
            auto ptr = std::make_shared<ChildT>(std::move(this->template as<ChildT>()));
            return std::static_pointer_cast<Valuable>(ptr);
        }

        ChildT& Reset() {
            OptimizeOff off;
            this->Become(ChildT());
            return this->Ref();
        }

        Valuable& operator^=(const Valuable& v) override {
            return ValuableDescendantBase::operator^=(v);
        }
        
        Valuable& operator+=(const Valuable &v) override {
            return ValuableDescendantBase::operator+=(v);
        }
        
        Valuable& operator*=(const Valuable &v) override {
            return ValuableDescendantBase::operator*=(v);
        }
        
        Valuable& operator/=(const Valuable &v) override {
            return ValuableDescendantBase::operator/=(v);
        }
        
        Valuable& operator%=(const Valuable &v) override {
            return ValuableDescendantBase::operator%=(v);
        }

        Valuable operator -() const override {
            return ValuableDescendantBase::operator-();
        }

        constexpr Valuable& operator --() override { return operator+=(constants::minus_1); }
        constexpr Valuable& operator ++() override { return operator+=(constants::one); }

        Valuable& sq() override {
            return ValuableDescendantBase::sq();
        }

        void optimize() override { }
        //void expand() override { }
        Valuable Sqrt() const override {
            return ValuableDescendantBase::Sqrt();
        }

        universal_lambda_t CompileIntoLambda(variables_for_lambda_t vars) const override {
            return ValuableDescendantBase::CompileIntoLambda(vars);
        }

        bool IsComesBefore(const Valuable& v) const override {
            return ValuableDescendantBase::IsComesBefore(v);
        }

        void Values(const std::function<bool(const Valuable&)> &fun) const override {
            ValuableDescendantBase::Values(fun);
        }

        Valuable Univariate() const override {
            return ValuableDescendantBase::Univariate();
        }
    };
}
}
