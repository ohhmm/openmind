#pragma once
#include <set>
#include "SmallVectorOptimization.h"
#include "Valuable.h"

namespace omnn {
namespace math {

struct SumOrderComparator {
    bool operator()(const Valuable& lhs, const Valuable& rhs) const {
        return lhs.Hash() < rhs.Hash();
    }
};

template<typename T, size_t N = 16>
class OptimizedCollection {
public:
    using value_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using small_container_type = SmallVector<value_type, N>;
    using large_container_type = std::set<value_type, SumOrderComparator>;

    class iterator;
    class const_iterator;

    class iterator {
        friend class OptimizedCollection;
        friend class const_iterator;

        OptimizedCollection* collection;
        typename small_container_type::iterator small_it;
        typename large_container_type::iterator large_it;
        bool is_small;

        iterator(OptimizedCollection* c, bool small,
                typename small_container_type::iterator sit,
                typename large_container_type::iterator lit)
            : collection(c)
            , small_it(sit)
            , large_it(lit)
            , is_small(small) {}

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        reference operator*() { return is_small ? *small_it : *large_it; }
        pointer operator->() { return is_small ? &*small_it : &*large_it; }

        iterator& operator++() {
            if (is_small) ++small_it;
            else ++large_it;
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++*this;
            return tmp;
        }

        iterator& operator--() {
            if (is_small) --small_it;
            else --large_it;
            return *this;
        }

        iterator operator--(int) {
            iterator tmp = *this;
            --*this;
            return tmp;
        }

        bool operator==(const iterator& other) const {
            return collection == other.collection &&
                   is_small == other.is_small &&
                   (is_small ? small_it == other.small_it : large_it == other.large_it);
        }

        bool operator!=(const iterator& other) const { return !(*this == other); }
    };

    class const_iterator {
        friend class OptimizedCollection;

        const OptimizedCollection* collection;
        typename small_container_type::const_iterator small_it;
        typename large_container_type::const_iterator large_it;
        bool is_small;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        const_iterator(const OptimizedCollection* c, bool small,
                      typename small_container_type::const_iterator sit,
                      typename large_container_type::const_iterator lit)
            : collection(c)
            , small_it(sit)
            , large_it(lit)
            , is_small(small) {}

        const_iterator(const iterator& it)
            : collection(it.collection)
            , small_it(it.small_it)
            , large_it(it.large_it)
            , is_small(it.is_small) {}

        reference operator*() const { return is_small ? *small_it : *large_it; }
        pointer operator->() const { return is_small ? &*small_it : &*large_it; }

        const_iterator& operator++() {
            if (is_small) ++small_it;
            else ++large_it;
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ++*this;
            return tmp;
        }

        const_iterator& operator--() {
            if (is_small) --small_it;
            else --large_it;
            return *this;
        }

        const_iterator operator--(int) {
            const_iterator tmp = *this;
            --*this;
            return tmp;
        }

        bool operator==(const const_iterator& other) const {
            return collection == other.collection &&
                   is_small == other.is_small &&
                   (is_small ? small_it == other.small_it : large_it == other.large_it);
        }

        bool operator!=(const const_iterator& other) const { return !(*this == other); }
    };

    iterator begin() {
        if (using_small) {
            return iterator(this, true, small_members.begin(), large_members.end());
        }
        return iterator(this, false, small_members.end(), large_members.begin());
    }

    iterator end() {
        if (using_small) {
            return iterator(this, true, small_members.end(), large_members.end());
        }
        return iterator(this, false, small_members.end(), large_members.end());
    }

    const_iterator begin() const {
        if (using_small) {
            return const_iterator(this, true, small_members.begin(), large_members.end());
        }
        return const_iterator(this, false, small_members.end(), large_members.begin());
    }

    const_iterator end() const {
        if (using_small) {
            return const_iterator(this, true, small_members.end(), large_members.end());
        }
        return const_iterator(this, false, small_members.end(), large_members.begin());
    }

    std::pair<iterator, bool> insert(const T& value) {
        if (using_small) {
            if (small_members.size() < N) {
                small_members.push_back(value);
                return {iterator(this, true, --small_members.end(), large_members.end()), true};
            }
            switchToLarge();
        }
        auto [it, inserted] = large_members.insert(value);
        return {iterator(this, false, small_members.end(), it), inserted};
    }

    std::pair<iterator, bool> insert(T&& value) {
        if (using_small) {
            if (small_members.size() < N) {
                small_members.push_back(std::move(value));
                return {iterator(this, true, --small_members.end(), large_members.end()), true};
            }
            switchToLarge();
        }
        auto [it, inserted] = large_members.insert(std::move(value));
        return {iterator(this, false, small_members.end(), it), inserted};
    }

    size_t size() const { return using_small ? small_members.size() : large_members.size(); }
    bool empty() const { return using_small ? small_members.empty() : large_members.empty(); }

    T extract(iterator pos) {
        if (using_small) {
            T value = std::move(*pos.small_it);
            erase(pos);
            return value;
        }
        auto node = large_members.extract(pos.large_it);
        return std::move(node.value());
    }

    iterator erase(iterator pos) {
        if (using_small) {
            auto next_it = small_members.erase(pos.small_it);
            return iterator(this, true, next_it, large_members.end());
        }
        auto next_it = large_members.erase(pos.large_it);
        return iterator(this, false, small_members.end(), next_it);
    }

private:
    void switchToLarge() {
        using_small = false;
        for (auto& item : small_members) {
            large_members.insert(std::move(item));
        }
        small_members.clear();
    }

    small_container_type small_members;
    large_container_type large_members;
    bool using_small = true;
};

}} // namespace omnn::math
