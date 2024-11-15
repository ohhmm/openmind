#pragma once
#include "SmallVectorOptimization.h"
#include <set>

namespace omnn {
namespace math {

// Forward declare SumOrderComparator if not already declared
struct SumOrderComparator;

template<typename T>
class OptimizedCollection {
    static constexpr size_t SmallSize = 16;
    using small_container = SmallVector<T, SmallSize>;
    using large_container = std::set<T, SumOrderComparator>;

    small_container small_members;
    large_container large_members;
    bool using_small = true;

    // Helper methods for default iterators
    static typename small_container::iterator default_small_iterator() {
        static small_container dummy;
        return dummy.begin();
    }

    static typename large_container::const_iterator default_large_iterator() {  // Changed return type
        static large_container dummy;
        return dummy.cbegin();  // Use cbegin() for const_iterator
    }

    static typename small_container::const_iterator default_small_const_iterator() {
        static const small_container dummy;
        return dummy.begin();
    }

    static typename large_container::const_iterator default_large_const_iterator() {
        static const large_container dummy;
        return dummy.cbegin();  // Use cbegin() for consistency
    }

public:
    class iterator;
    class const_iterator;

    class iterator {
        friend class OptimizedCollection;
        friend class const_iterator;

        OptimizedCollection* collection;
        typename small_container::iterator small_it;
        typename large_container::const_iterator large_it;  // Changed to const_iterator
        bool is_small;

        iterator(OptimizedCollection* c, bool small,
                typename small_container::iterator sit,
                typename large_container::const_iterator lit)  // Changed to const_iterator
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

        reference operator*() { return is_small ? *small_it : const_cast<reference>(*large_it); }
        pointer operator->() { return is_small ? &*small_it : const_cast<pointer>(&*large_it); }

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
        typename small_container::const_iterator small_it;
        typename large_container::const_iterator large_it;
        bool is_small;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        const_iterator(const OptimizedCollection* c, bool small,
                      typename small_container::const_iterator sit,
                      typename large_container::const_iterator lit)
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
            return iterator(this, true, small_members.begin(), default_large_iterator());
        }
        return iterator(this, false, default_small_iterator(), large_members.cbegin());
    }

    iterator end() {
        if (using_small) {
            return iterator(this, true, small_members.end(), default_large_iterator());
        }
        return iterator(this, false, default_small_iterator(), large_members.cend());
    }

    const_iterator begin() const {
        if (using_small) {
            return const_iterator(this, true, small_members.begin(), default_large_const_iterator());
        }
        return const_iterator(this, false, default_small_const_iterator(), large_members.cbegin());
    }

    const_iterator end() const {
        if (using_small) {
            return const_iterator(this, true, small_members.end(), default_large_const_iterator());
        }
        return const_iterator(this, false, default_small_const_iterator(), large_members.cend());
    }

    std::pair<iterator, bool> insert(const T& value) {
        if (using_small) {
            auto result = small_members.insert(value);
            if (small_members.size() > SmallSize) {
                switchToLarge();
            }
            return {iterator(this, true, result.first, default_large_iterator()), result.second};
        }
        auto result = large_members.insert(value);
        return {iterator(this, false, default_small_iterator(), result.first), result.second};
    }

    std::pair<iterator, bool> insert(T&& value) {
        if (using_small) {
            if (small_members.size() < SmallSize) {
                small_members.push_back(std::move(value));
                return {iterator(this, true, --small_members.end(), default_large_iterator()), true};
            }
            using_small = false;
            large_members.insert(
                std::make_move_iterator(small_members.begin()),
                std::make_move_iterator(small_members.end()));
            small_members.clear();
        }
        auto [it, inserted] = large_members.insert(std::move(value));
        return {iterator(this, false, default_small_iterator(), it), inserted};
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
            return iterator(this, true, next_it, default_large_iterator());
        }
        auto next_it = large_members.erase(pos.large_it);
        return iterator(this, false, default_small_iterator(), next_it);
    }

private:
    void switchToLarge() {
        using_small = false;
        large_members.insert(small_members.begin(), small_members.end());
        small_members.clear();
    }
};

}} // namespace omnn::math
