#ifndef CUSTOM_ALLOCATOR_H
#define CUSTOM_ALLOCATOR_H

#include <memory>
#include <type_traits>
#include <cstddef>
#include <limits>
#include <new>
#include <boost/version.hpp>
#include <vector>

template <typename T>
class custom_allocator {
public:
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using value_type = T;
    using iterator = T*;
    using const_iterator = const T*;

    template <class U>
    struct rebind {
        typedef custom_allocator<U> other;
    };

    custom_allocator() noexcept {}

    custom_allocator(size_type) noexcept : custom_allocator() {}

    template <class U>
    custom_allocator(const custom_allocator<U>&) noexcept {}

    pointer address(reference x) const noexcept {
        return std::addressof(x);
    }

    const_pointer address(const_reference x) const noexcept {
        return std::addressof(x);
    }

    pointer allocate(size_type n, const void* hint = 0) {
        if (n > this->max_size())
            throw std::bad_alloc();
        return static_cast<pointer>(::operator new(n * sizeof(T), std::nothrow));
    }

    void deallocate(pointer p, size_type n) noexcept {
        ::operator delete(p);
    }

    size_type max_size() const noexcept {
        return std::numeric_limits<size_type>::max() / sizeof(T);
    }

    template <class U, class... Args>
    void construct(U* p, Args&&... args) {
        new(static_cast<void*>(p)) U(std::forward<Args>(args)...);
    }

    template <class U>
    void destroy(U* p) {
        p->~U();
    }
};

// Wrapper class for unbounded_array
template <typename T, typename Allocator = std::allocator<T>>
class unbounded_array_wrapper {
public:
    using size_type = typename Allocator::size_type;
    using difference_type = typename Allocator::difference_type;
    using pointer = typename Allocator::pointer;
    using const_pointer = typename Allocator::const_pointer;
    using reference = T&;
    using const_reference = const T&;
    using value_type = T;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using allocator_type = Allocator;

private:
    std::vector<T, Allocator> data_;

public:
    explicit unbounded_array_wrapper(size_type size, const T& value = T(), const Allocator& alloc = Allocator())
        : data_(size, value, alloc) {}

    unbounded_array_wrapper(const unbounded_array_wrapper& other)
        : data_(other.data_) {}

    unbounded_array_wrapper& operator=(const unbounded_array_wrapper& other) {
        data_ = other.data_;
        return *this;
    }

    iterator begin() noexcept {
        return &data_[0];
    }

    const_iterator begin() const noexcept {
        return &data_[0];
    }

    iterator end() noexcept {
        return &data_[0] + data_.size();
    }

    const_iterator end() const noexcept {
        return &data_[0] + data_.size();
    }

    size_type size() const noexcept {
        return data_.size();
    }

    reference operator[](size_type i) {
        return data_[i];
    }

    const_reference operator[](size_type i) const {
        return data_[i];
    }

    void resize(size_type new_size, const T& value = T()) {
        data_.resize(new_size, value);
    }

    void clear() noexcept {
        data_.clear();
    }
};

#endif // CUSTOM_ALLOCATOR_H
