//
// Created by Сергей Кривонос on 25.09.17.
//

#pragma once
#include <algorithm>
#include <iterator>

// Include complete definitions first
#include "CollectionForward.h"
#include "OptimizedCollection.h"
#include "ValuableCollectionDescendantContract.h"
#include "Integer.h"
#include "Fraction.h"
#include "Variable.h"

namespace omnn {
namespace math {

    struct ProductOrderComparator {
        bool operator()(const Valuable& lhs, const Valuable& rhs) const {
            return lhs.Hash() < rhs.Hash();
        }
    };

    using product_cont = OptimizedCollection<Valuable, ProductOrderComparator>;

    class Product
        : public ValuableCollectionDescendantContract<Product, product_cont>
    {
    protected:
        using base = ValuableCollectionDescendantContract<Product, product_cont>;
        friend class Variable;

        product_cont members;
        max_exp_t vaExpsSum = 0;
        vars_cont_t vars;

        cont& GetCont() override { return members; }
        const cont& GetConstCont() const override { return members; }

        void AddToVars(const Variable& item, const Valuable& exponentiation);
        void AddToVarsIfVaOrVaExp(const Valuable::vars_cont_t&);
        void AddToVarsIfVaOrVaExp(const Valuable& item);

    public:
        using base::Add;

        Product() : members() { this->Add(constants::one); }
        Product(Product&&) = default;
        Product(const Product&) = default;
        Product& operator=(Product&&) = default;
        Product& operator=(const Product&) = default;

        Product(const std::initializer_list<Valuable>& list) : members() {
            for(const auto& v : list) { this->Add(v); }
        }

        explicit Product(const vars_cont_t& v) : vars(v), members() {
            this->Add(constants::one);
        }

        void Delete(iterator& it) override {
            if (it != members.end()) {
                this->hash ^= it->Hash();
                members.erase(it);
                this->is_optimized = false;
            }
        }

        iterator Had(iterator it) override;
        static bool VarSurdFactor(const Valuable&);

        const vars_cont_t& getCommonVars() const override { return vars; }
        vars_cont_t getCommonVars(const vars_cont_t& with) const;

        Valuable getCommVal(const Product& with) const;
        Valuable InCommonWith(const Valuable& v) const override;
        max_exp_t findMaxVaExp();
        bool IsComesBefore(const Valuable& v) const override;
        Valuable calcFreeMember() const override;

        Valuable& operator +=(const Valuable& v) override;
        std::pair<bool,Valuable> IsSummationSimplifiable(const Valuable& v) const override;
        std::pair<bool, Valuable> IsMultiplicationSimplifiable(const Valuable& v) const override;
        Valuable& operator*=(const Valuable& v) override;
        Valuable& operator /=(const Valuable& v) override;
        Valuable& operator %=(const Valuable& v) override;
        Valuable& operator ^=(const Valuable& v) override;
        bool operator ==(const Product& v) const;
        bool operator ==(const Valuable& v) const override;
        bool operator<(const Product& v) const;
        bool operator<(const Valuable& v) const override;
        Valuable Sign() const override;
        Valuable ToBool() const override;
        explicit operator double() const override;
        Valuable& d(const Variable& x) override;
        void optimize() override;
        Valuable Sqrt() const override;
        Valuable& sq() override;
        Valuable abs() const override;
        Valuable& reciprocal() override;

        bool IsProduct() const override { return true; }
        bool IsZero() const override;
        std::pair<Valuable, Valuable> SplitSimplePart() const;
        std::pair<Valuable, Valuable> split_simple_part();

        Valuable operator()(const Variable& va) const override;
        Valuable operator()(const Variable&, const Valuable& augmentation) const override;
        void solve(const Variable& va, solutions_t& solutions) const override;
        Valuable::solutions_t Distinct() const override;

        std::ostream& code(std::ostream& out) const override;

        static constexpr auto GetBinaryOperationLambdaTemplate() {
            return [](const auto& _1st, const auto& _2nd) { return _1st * _2nd; };
        }

        vars_cont_t GetVaExps() const override;

    protected:
        std::ostream& print(std::ostream& out) const override;
    };

}}
