#pragma once
#include <memory>
#include <type_traits>
#include <cstddef>
#include <limits>
#include <new>
#include <vector>

#include <boost/version.hpp>

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

    template <class U, class... Args>
    void construct(U* p, Args&&... args) {
        new(static_cast<void*>(p)) U(std::forward<Args>(args)...);
    }

    template <class U>
    void destroy(U* p) {
        p->~U();
    }

    // Additional methods for Boost compatibility
    bool operator==(const custom_allocator&) const noexcept { return true; }
    bool operator!=(const custom_allocator&) const noexcept { return false; }

    // Propagate on container copy assignment
    using propagate_on_container_copy_assignment = std::false_type;
    // Propagate on container move assignment
    using propagate_on_container_move_assignment = std::false_type;
    // Propagate on container swap
    using propagate_on_container_swap = std::false_type;

    // Is always equal
    using is_always_equal = std::true_type;

    // Select on container copy construction
    static custom_allocator select_on_container_copy_construction(const custom_allocator&) { return custom_allocator(); }

    // Allocator traits
    template <typename U>
    struct rebind {
        using other = custom_allocator<U>;
    };

    // Required methods for Boost compatibility
    T* data() { return storage.data(); }
    const T* data() const { return storage.data(); }
    size_type size() const noexcept { return storage.size(); }

    // Implement operator[] for compatibility with Boost's containers
    T& operator[](size_type i) {
        return storage[i];
    }
    const T& operator[](size_type i) const {
        return storage[i];
    }

    // Implement resize method for Boost compatibility
    void resize(size_type n) {
        storage.resize(n);
    }

    // Implement swap method for Boost compatibility
    void swap(custom_allocator& other) noexcept {
        storage.swap(other.storage);
    }

private:
    std::vector<T> storage;
};

// Wrapper class for unbounded_array
template <typename T, typename Allocator = custom_allocator<T>>
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
