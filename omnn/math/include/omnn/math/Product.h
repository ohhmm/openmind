//
// Created by Сергей Кривонос on 25.09.17.
//

#pragma once
#include "omnn/math/ValuableCollectionDescendantContract.h"
#include "omnn/math/Variable.h"
#include "omnn/math/Valuable.h"
#include "omnn/math/Constant.h"
#include "omnn/math/OptimizedCollection.h"
#include <set>

namespace omnn {
namespace math {

class ProductOrderComparator {
    using cont_t = std::set<Valuable>;
    cont_t members;
public:
    using iterator = cont_t::iterator;
    using const_iterator = cont_t::const_iterator;
    using value_type = cont_t::value_type;
    using reference = cont_t::reference;
    using const_reference = cont_t::const_reference;
    using difference_type = cont_t::difference_type;
    using size_type = cont_t::size_type;

    size_t size() const { return members.size(); }
    bool empty() const { return members.empty(); }
    void clear() { members.clear(); }

    iterator begin() { return members.begin(); }
    iterator end() { return members.end(); }
    const_iterator begin() const { return members.begin(); }
    const_iterator end() const { return members.end(); }
    const_iterator cbegin() const { return members.cbegin(); }
    const_iterator cend() const { return members.cend(); }

    template<typename... Args>
    auto insert(Args&&... args) {
        return members.insert(std::forward<Args>(args)...);
    }

    template<typename... Args>
    auto erase(Args&&... args) {
        return members.erase(std::forward<Args>(args)...);
    }

    iterator find(const Valuable& value) {
        return members.find(value);
    }

    const_iterator find(const Valuable& value) const {
        return members.find(value);
    }
};

class Product : public ValuableCollectionDescendantContract<Product, ProductOrderComparator>
{
    using base = ValuableCollectionDescendantContract<Product, ProductOrderComparator>;
    using cont = ProductOrderComparator;
    vars_cont_t vars;
    mutable exp_t maxVaExp = 0;
    mutable bool is_optimized = false;

public:
    static auto GetBinaryOperationLambdaTemplate() {
        return [](const Valuable& a, const Valuable& b) -> Valuable {
            return a * b;
        };
    }

    Product() : base() {
        Add(constants::one, this->end());
    }

    Product(Product&&) = default;
    Product(const Product&) = default;
    Product& operator=(Product&&) = default;
    Product& operator=(const Product&) = default;

    Product(const std::initializer_list<Valuable>& list)
        : base() {
        for(const auto& v : list) {
            Add(v, this->end());
        }
    }

    explicit Product(const vars_cont_t& v)
        : base() {
        for (const auto& [var, val] : v) {
            vars.insert(std::make_pair(var, val));
        }
        Add(constants::one, this->end());
    }

    void Delete(iterator& it) override {
        auto& c = GetCont();
        if (it != c.end()) {
            Valuable::hash ^= (*it).Hash();
            auto findNewMaxVaExp = (*it).getMaxVaExp() == maxVaExp;
            auto next = std::next(it);
            c.erase(it);
            it = next;
            is_optimized = false;
            if (findNewMaxVaExp) {
                maxVaExp = findMaxVaExp();
            }
        }
    }

    cont& GetCont() override { return base::GetCont(); }
    const cont& GetConstCont() const override { return base::GetConstCont(); }

protected:
    iterator getit(typename cont::iterator it) {
        return it;
    }

    const_iterator getit(typename cont::const_iterator it) const {
        return it;
    }

    void AddToVarsIfVaOrVaExp(const Valuable& v) {
        if (v.IsVa() || v.IsVaExp()) {
            vars.insert(std::make_pair(v.get(), v));
            auto ve = v.getMaxVaExp();
            if (ve > maxVaExp) maxVaExp = ve;
        }
    }

    Valuable findMaxVaExp() const {
        Valuable max;
        const auto& c = GetConstCont();
        for (const auto& v : c) {
            auto ve = v.getMaxVaExp();
            if (ve > max) max = ve;
        }
        return max;
    }

public:
    using iterator = typename base::iterator;
    using const_iterator = typename base::const_iterator;

    using base::Add;  // Make base class Add visible

    const iterator Add(const Valuable& item, const iterator& hint) override {
        auto& c = GetCont();
        Valuable::hash ^= item.Hash();
        is_optimized = false;
        AddToVarsIfVaOrVaExp(item);
        return hint == this->end() ? getit(c.insert(item)) : getit(c.insert(hint, item));
    }

    const iterator Add(Valuable&& item, const iterator& hint) override {
        auto& c = GetCont();
        Valuable::hash ^= item.Hash();
        is_optimized = false;
        AddToVarsIfVaOrVaExp(item);
        return hint == this->end() ? getit(c.insert(std::move(item))) : getit(c.insert(hint, std::move(item)));
    }

    bool operator==(const Valuable& v) const override;
    bool operator<(const Valuable& v) const override;
    Valuable& operator+=(const Valuable& v) override;
    Valuable& operator*=(const Valuable& v) override;
    Valuable& operator/=(const Valuable& v) override;
    Valuable& operator%=(const Valuable& v) override;
    Valuable& operator^=(const Valuable& v) override;
    Valuable operator-() const override;
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
    Valuable operator()(const Variable& va) const override;
    Valuable operator()(const Variable&, const Valuable& augmentation) const override;
    void solve(const Variable& va, solutions_t& solutions) const override;
    solutions_t Distinct() const override;
    std::ostream& code(std::ostream& out) const override;
    std::ostream& print(std::ostream& out) const override;
};

}} // namespace omnn::math
