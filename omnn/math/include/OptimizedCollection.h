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

public:
    class iterator {
        friend class OptimizedCollection;
        OptimizedCollection* collection;
        typename small_container::iterator small_it;
        typename large_container::iterator large_it;
        bool is_small;

        iterator(OptimizedCollection* c, bool small,
                typename small_container::iterator sit,
                typename large_container::iterator lit)
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

    using const_iterator = const iterator;

    iterator begin() {
        return iterator(this, using_small,
                       using_small ? small_members.begin() : small_container::iterator{},
                       using_small ? large_container::iterator{} : large_members.begin());
    }

    iterator end() {
        return iterator(this, using_small,
                       using_small ? small_members.end() : small_container::iterator{},
                       using_small ? large_container::iterator{} : large_members.end());
    }

    const_iterator begin() const {
        return const_iterator(const_cast<OptimizedCollection*>(this), using_small,
                            using_small ? small_members.begin() : small_container::iterator{},
                            using_small ? large_container::iterator{} : large_members.begin());
    }

    const_iterator end() const {
        return const_iterator(const_cast<OptimizedCollection*>(this), using_small,
                            using_small ? small_members.end() : small_container::iterator{},
                            using_small ? large_container::iterator{} : large_members.end());
    }

    std::pair<iterator, bool> insert(const T& value) {
        if (using_small) {
            if (small_members.size() < SmallSize) {
                small_members.push_back(value);
                return {iterator(this, true, --small_members.end(), large_container::iterator{}), true};
            }
            // Switch to large collection
            using_small = false;
            large_members.insert(small_members.begin(), small_members.end());
            small_members.clear();
        }
        auto [it, inserted] = large_members.insert(value);
        return {iterator(this, false, small_container::iterator{}, it), inserted};
    }

    std::pair<iterator, bool> insert(T&& value) {
        if (using_small) {
            if (small_members.size() < SmallSize) {
                small_members.push_back(std::move(value));
                return {iterator(this, true, --small_members.end(), large_container::iterator{}), true};
            }
            using_small = false;
            large_members.insert(
                std::make_move_iterator(small_members.begin()),
                std::make_move_iterator(small_members.end()));
            small_members.clear();
        }
        auto [it, inserted] = large_members.insert(std::move(value));
        return {iterator(this, false, small_container::iterator{}, it), inserted};
    }

    size_t size() const { return using_small ? small_members.size() : large_members.size(); }
    bool empty() const { return using_small ? small_members.empty() : large_members.empty(); }

    T extract(iterator pos) {
        if (using_small) {
            return small_members.extract(pos.small_it);
        }
        auto node = large_members.extract(pos.large_it);
        return std::move(node.value());
    }

    iterator erase(iterator pos) {
        if (using_small) {
            auto next_it = small_members.erase(pos.small_it);
            return iterator(this, true, next_it, large_container::iterator{});
        }
        auto next_it = large_members.erase(pos.large_it);
        return iterator(this, false, small_container::iterator{}, next_it);
    }
};

}} // namespace omnn::math
