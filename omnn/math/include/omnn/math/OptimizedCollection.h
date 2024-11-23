#pragma once
#include <set>
#include <algorithm>
#include <iterator>
#include <memory>
#include "SmallVectorOptimization.h"
#include "Valuable.h"
#include "Comparators.h"

namespace omnn {
namespace math {

template<
    typename T,
    typename Comparator = SumOrderComparator,
    size_t N = 16
>
class OptimizedCollection {
public:
    using container_type = std::set<T, Comparator>;
    using value_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using iterator = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;

private:
    container_type large_collection;
    SmallVector<T, N> small_collection;
    bool using_small = true;

    void switch_to_large() {
        if (using_small) {
            large_collection.insert(small_collection.begin(), small_collection.end());
            small_collection.clear();
            using_small = false;
        }
    }

public:
    OptimizedCollection() = default;

    iterator begin() noexcept {
        return using_small ? small_collection.begin() : large_collection.begin();
    }

    iterator end() noexcept {
        return using_small ? small_collection.end() : large_collection.end();
    }

    const_iterator begin() const noexcept {
        return using_small ? small_collection.begin() : large_collection.begin();
    }

    const_iterator end() const noexcept {
        return using_small ? small_collection.end() : large_collection.end();
    }

    const_iterator cbegin() const noexcept {
        return begin();
    }

    const_iterator cend() const noexcept {
        return end();
    }

    size_type size() const noexcept {
        return using_small ? small_collection.size() : large_collection.size();
    }

    bool empty() const noexcept {
        return using_small ? small_collection.empty() : large_collection.empty();
    }

    void clear() noexcept {
        if (using_small) {
            small_collection.clear();
        } else {
            large_collection.clear();
            using_small = true;
        }
    }

    std::pair<iterator, bool> emplace(T&& value) {
        if (using_small) {
            if (small_collection.size() < N) {
                auto it = std::lower_bound(small_collection.begin(), small_collection.end(), value, Comparator());
                if (it != small_collection.end() && !Comparator()(value, *it)) {
                    return {it, false};
                }
                small_collection.push_back(std::move(value));
                return {--small_collection.end(), true};
            }
            switch_to_large();
        }
        return large_collection.emplace(std::move(value));
    }

    std::pair<iterator, bool> insert(const T& value) {
        if (using_small) {
            if (small_collection.size() < N) {
                auto it = std::lower_bound(small_collection.begin(), small_collection.end(), value, Comparator());
                if (it != small_collection.end() && !Comparator()(value, *it)) {
                    return {it, false};
                }
                small_collection.push_back(value);
                return {--small_collection.end(), true};
            }
            switch_to_large();
        }
        return large_collection.insert(value);
    }

    iterator insert(const_iterator hint, const T& value) {
        if (using_small) {
            if (small_collection.size() < N) {
                small_collection.push_back(value);
                return --small_collection.end();
            }
            switch_to_large();
        }
        return large_collection.insert(hint, value);
    }

    iterator erase(const_iterator pos) {
        if (using_small) {
            return small_collection.erase(pos);
        }
        return large_collection.erase(pos);
    }
};

}} // namespace omnn::math
