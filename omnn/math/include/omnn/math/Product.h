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
#include <map>  // Include map for vars_cont_t

namespace omnn {
namespace math {

class ProductOrderComparator {
private:
    using container_type = std::set<Valuable, std::less<Valuable>>;
    container_type members;

public:
    // Standard container type definitions
    using iterator = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;
    using value_type = typename container_type::value_type;
    using reference = typename container_type::reference;
    using const_reference = typename container_type::const_reference;
    using difference_type = typename container_type::difference_type;
    using size_type = typename container_type::size_type;

    // Default constructors
    ProductOrderComparator() = default;
    ProductOrderComparator(const ProductOrderComparator&) = default;
    ProductOrderComparator(ProductOrderComparator&&) = default;
    ProductOrderComparator& operator=(const ProductOrderComparator&) = default;
    ProductOrderComparator& operator=(ProductOrderComparator&&) = default;

    // Container interface methods
    size_type size() const noexcept { return members.size(); }
    bool empty() const noexcept { return members.empty(); }
    void clear() noexcept { members.clear(); }

    // Iterator methods
    iterator begin() noexcept { return members.begin(); }
    const_iterator begin() const noexcept { return members.begin(); }
    const_iterator cbegin() const noexcept { return members.cbegin(); }
    iterator end() noexcept { return members.end(); }
    const_iterator end() const noexcept { return members.end(); }
    const_iterator cend() const noexcept { return members.cend(); }

    // Modifiers
    std::pair<iterator, bool> insert(const value_type& value) {
        return members.insert(value);
    }

    iterator insert(const_iterator hint, const value_type& value) {
        return members.insert(hint, value);
    }

    template<typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        return members.emplace(std::forward<Args>(args)...);
    }

    iterator erase(const_iterator pos) {
        return members.erase(pos);
    }

    size_type erase(const value_type& value) {
        return members.erase(value);
    }

    iterator find(const value_type& value) {
        return members.find(value);
    }

    const_iterator find(const value_type& value) const {
        return members.find(value);
    }

    // Custom comparison operator for ordering
    bool operator()(const Valuable& l, const Valuable& r) const {
        return l.Hash() < r.Hash();
    }
};

class Product : public ValuableCollectionDescendantContract<Product, ProductOrderComparator>
{
    using base = ValuableCollectionDescendantContract<Product, ProductOrderComparator>;
    using cont = ProductOrderComparator;
    using vars_cont_t = std::map<Variable, Valuable>;  // Explicitly define vars_cont_t
    vars_cont_t vars;
    mutable Valuable maxVaExp;  // Changed from exp_t to Valuable
    mutable bool is_optimized = false;

protected:
    cont collection;

    // Ensure proper container access
    using collection_type = cont;
    collection_type& GetCollection() { return collection; }
    const collection_type& GetCollection() const { return collection; }

    // Base class interface implementation
    cont& GetCont() override { return GetCollection(); }
    const cont& GetConstCont() const override { return GetCollection(); }

    // Binary operation template for Product
    auto GetBinaryOperationLambdaTemplate() const {
        return [](const Valuable& a, const Valuable& b) -> Valuable {
            return a * b;
        };
    }

    void AddToVarsIfVaOrVaExp(const Valuable& v) {
        if (v.IsVa() || v.IsVaExp()) {
            auto var = v.get();
            if (var.IsUnivariable()) {
                vars[var] = v;
                auto ve = v.getMaxVaExp();
                if (ve > maxVaExp) maxVaExp = ve;
            }
        }
    }

    Valuable findMaxVaExp() const {
        Valuable max;
        const auto& c = GetCollection();
        for (const auto& item : c) {
            auto ve = item.getMaxVaExp();
            if (ve > max) max = ve;
        }
        return max;
    }

public:
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
        auto& c = GetCollection();
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

    // Implement container interface methods by forwarding to collection
    using iterator = typename cont::iterator;
    using const_iterator = typename cont::const_iterator;
    using value_type = typename cont::value_type;
    using reference = typename cont::reference;
    using const_reference = typename cont::const_reference;
    using difference_type = typename cont::difference_type;
    using size_type = typename cont::size_type;

    size_type size() const noexcept { return GetCollection().size(); }
    bool empty() const noexcept { return GetCollection().empty(); }
    iterator begin() noexcept { return GetCollection().begin(); }
    const_iterator begin() const noexcept { return GetCollection().begin(); }
    iterator end() noexcept { return GetCollection().end(); }
    const_iterator end() const noexcept { return GetCollection().end(); }

    const iterator Add(const Valuable& item, const iterator& hint) override {
        auto& c = GetCollection();
        Valuable::hash ^= item.Hash();
        is_optimized = false;
        AddToVarsIfVaOrVaExp(item);
        return hint == this->end() ? c.insert(item).first : c.insert(hint, item);
    }

    const iterator Add(Valuable&& item, const iterator& hint) override {
        auto& c = GetCollection();
        Valuable::hash ^= item.Hash();
        is_optimized = false;
        AddToVarsIfVaOrVaExp(item);
        return hint == this->end() ? c.insert(std::move(item)).first : c.insert(hint, std::move(item));
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
