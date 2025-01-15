#pragma once
#include <memory>
#include <type_traits>
#include <cstddef>
#include <limits>
#include <new>
#include <vector>

#include <boost/version.hpp>

namespace omnn::rt {

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

    custom_allocator() noexcept {}

    custom_allocator(size_type) noexcept : custom_allocator() {}

    custom_allocator(size_type, const value_type&) noexcept : custom_allocator() {}

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

    // Type definitions for Boost.uBLAS compatibility
    using self_type = custom_allocator<T>;
    using const_reference_type = const_reference;
    using pointer_type = pointer;
    using const_pointer_type = const_pointer;

    // Rebind allocator to different type (required by both std:: and boost::)
    template <typename U>
    struct rebind {
        using other = custom_allocator<U>;
    };

    // Construction/destruction methods with C++23 compatibility
    template <typename U>
    constexpr void construct(U* p) noexcept(std::is_nothrow_default_constructible_v<U>) {
        ::new(static_cast<void*>(p)) U();
    }

    template <typename U>
    constexpr void construct(U* p, const U& value) noexcept(std::is_nothrow_copy_constructible_v<U>) {
        ::new(static_cast<void*>(p)) U(value);
    }

    template <typename U, typename... Args>
    constexpr void construct(U* p, Args&&... args)
        noexcept(std::is_nothrow_constructible_v<U, Args...>) {
        ::new(static_cast<void*>(p)) U(std::forward<Args>(args)...);
    }

    template <typename U>
    constexpr void destroy(U* p) noexcept {
        if (p != nullptr) {
            p->~U();
        }
    }

    // C++23 allocator requirements
    using is_always_equal = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_copy_assignment = std::false_type;
    using propagate_on_container_swap = std::false_type;

    // C++23 comparison operators
    [[nodiscard]] constexpr bool operator==(const custom_allocator&) const noexcept { return true; }
    [[nodiscard]] constexpr bool operator!=(const custom_allocator&) const noexcept { return false; }

    // Assignment operator
    template<class U>
    custom_allocator& operator=(const custom_allocator<U>&) noexcept { return *this; }

    // Container behavior
    static custom_allocator select_on_container_copy_construction(const custom_allocator&) noexcept {
        return custom_allocator();
    }

    // Memory management
    pointer data() noexcept { return nullptr; }
    const_pointer data() const noexcept { return nullptr; }
    size_type size() const noexcept { return 0; }

    // Container operations
    void initialize(size_type size) {
        if (size > 0) {
            pointer p = allocate(size);
            for (size_type i = 0; i < size; ++i) {
                construct(p + i, value_type());
            }
        }
    }

    void resize(size_type new_size) {
        if (new_size > 0) {
            pointer p = allocate(new_size);
            for (size_type i = 0; i < new_size; ++i) {
                construct(p + i, value_type());
            }
        }
    }

    void swap(custom_allocator& other) noexcept {
        std::swap(*this, other);
    }
};

// Wrapper class for unbounded_array
template <typename T, typename Allocator = omnn::rt::custom_allocator<T>>
class unbounded_array_wrapper {
public:
    using size_type = typename Allocator::size_type;
    using difference_type = typename Allocator::difference_type;
    using pointer = typename Allocator::pointer;
    using const_pointer = typename Allocator::const_pointer;
    using reference = T&;
    using const_reference = const T&;
    using value_type = T;
    using iterator = typename std::vector<T, Allocator>::iterator;
    using const_iterator = typename std::vector<T, Allocator>::const_iterator;
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
        return data_.begin();
    }

    const_iterator begin() const noexcept {
        return data_.cbegin();
    }

    iterator end() noexcept {
        return data_.end();
    }

    const_iterator end() const noexcept {
        return data_.cend();
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

} // namespace omnn::rt
