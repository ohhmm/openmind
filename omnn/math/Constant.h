//
// Created by Сергей Кривонос on 01.09.17.
//
#pragma once
#include "Exponentiation.h"
#include "Product.h"

namespace omnn{
namespace math {

    template <class Chld>
    class Constant
        : public ValuableDescendantContract<Chld>
    {
        using base = ValuableDescendantContract<Chld>;

    protected:
        bool IsSubObject(const Valuable& o) const override {
            return this == &o.get();
        }

    public:
        using base::base;

        bool IsConstant() const override { return true; }

        const Variable* FindVa() const override {
            return {};
        }
        
        bool HasVa(const Variable& va) const override {
            return {};
        }
        
        void CollectVa(std::set<Variable>& s) const override
        { }
        void CollectVaNames(Valuable::va_names_t& s) const override
        { }
        
        void Eval(const Variable& va, const Valuable& v) override
        { }
        
        Valuable& operator *=(const Valuable& v) override {
            if(v.IsProduct())
                return this->Become(v**this);
            else
                return this->Become(Product{*this,v});
        }

        std::pair<bool, Valuable> IsMultiplicationSimplifiable(const Valuable& v) const override {
            std::pair<bool, Valuable> is;
            is.first = v.IsConstant();
            if (is.first) {
                is.first = this->OfSameType(v);
                if (is.first) {
                    is.second = this->Sq();
                }
            } else if (!v.IsVa()) {
                is = v.IsMultiplicationSimplifiable(*this);
            }
            return is;
        }
        
        bool operator==(const Valuable& v) const override {
            return this->OfSameType(v);
        }
        
        const Valuable::vars_cont_t& getCommonVars() const override {
            return Valuable::emptyCommonVars();
        }
        
        a_int Complexity() const override { return 1; }

        typename base::solutions_t Distinct() const override { return { *this }; }
        Valuable Univariate() const override { return *this; }
    };
}}
