#ifndef CUSTOM_ALLOCATOR_H
#define CUSTOM_ALLOCATOR_H

#include <memory>
#include <type_traits>
#include <cstddef>

template <typename T>
class custom_allocator : public std::allocator<T> {
public:
    using size_type = size_t;
    using pointer = T*;
    using const_pointer = const T*;
    using value_type = T;

    // Add iterator type definitions for compatibility with Boost Numeric Ublass
    using iterator = T*;
    using const_iterator = const T*;

    template <class U>
    struct rebind {
        typedef custom_allocator<U> other;
    };

    custom_allocator() = default;

    // Constructor that accepts a size_type argument
    custom_allocator(size_type) : std::allocator<T>() {}

    template <class U>
    custom_allocator(const custom_allocator<U>&) {}

    pointer allocate(size_type n) {
        return std::allocator<T>::allocate(n);
    }

    void deallocate(pointer p, size_type n) {
        std::allocator<T>::deallocate(p, n);
    }

    // Removed construct and destroy methods as they are not required by the Boost Numeric Ublass library

    // Dummy resize method to satisfy the Boost Numeric Ublass library's requirements
    void resize(size_type) noexcept {
        // This method is not required by the standard allocator interface.
        // It is provided as a dummy implementation to satisfy the requirements of the Boost Numeric Ublass library.
        // The actual resizing should be handled by the container using this allocator.
    }

    size_type size() const noexcept {
        // This is not a standard allocator function and is only here to satisfy the Boost Numeric Ublass library's requirements.
        // The actual size management should be handled by the container using this allocator.
        // Returning a dummy value as the size is not tracked by the allocator.
        return 0;
    }

    T& operator[](size_type n) {
        // This is not a standard allocator function and is only here to satisfy the Boost Numeric Ublass library's requirements.
        // The actual element access should be handled by the container using this allocator.
        // Returning a reference to a dummy static variable as this operator should not be used directly.
        static T dummy;
        return dummy;
    }

    const T& operator[](size_type n) const {
        // This is not a standard allocator function and is only here to satisfy the Boost Numeric Ublass library's requirements.
        // The actual element access should be handled by the container using this allocator.
        // Returning a reference to a dummy static variable as this operator should not be used directly.
        static T dummy;
        return dummy;
    }

    // Add swap method for compatibility with Boost Numeric Ublass
    void swap(custom_allocator& other) noexcept {
        std::swap(static_cast<std::allocator<T>&>(*this), static_cast<std::allocator<T>&>(other));
    }

    // Add begin method for compatibility with Boost Numeric Ublass
    iterator begin() noexcept {
        // Since the allocator does not actually hold the data, this method should not be used.
        // It is only provided for compatibility purposes and will return a null iterator.
        return nullptr;
    }
};

// Specialization for std::size_t
template <>
class custom_allocator<std::size_t> : public std::allocator<std::size_t> {
public:
    using size_type = std::size_t;
    using pointer = std::size_t*;
    using const_pointer = const std::size_t*;
    using value_type = std::size_t;

    // Add iterator type definitions for compatibility with Boost Numeric Ublass
    using iterator = std::size_t*;
    using const_iterator = const std::size_t*;

    template <class U>
    struct rebind {
        typedef custom_allocator<U> other;
    };

    custom_allocator() = default;

    // Constructor that accepts a size_type argument
    custom_allocator(size_type) : std::allocator<std::size_t>() {}

    template <class U>
    custom_allocator(const custom_allocator<U>&) {}

    pointer allocate(size_type n) {
        return std::allocator<std::size_t>::allocate(n);
    }

    void deallocate(pointer p, size_type n) {
        std::allocator<std::size_t>::deallocate(p, n);
    }

    // The construct and destroy methods are not required by the C++17 standard
    // and are handled by std::allocator_traits. However, if the Boost Numeric Ublass
    // library requires these methods, they should be implemented correctly.
    // The following implementations are provided for compatibility with Boost Numeric Ublass.

    template <class U, class... Args>
    void construct(U* p, Args&&... args) {
        ::new((void *)p) U(std::forward<Args>(args)...);
    }

    template <class U>
    void destroy(U* p) {
        p->~U();
    }

    // Dummy resize method to satisfy the Boost Numeric Ublass library's requirements
    void resize(size_type) noexcept {
        // This method is not required by the standard allocator interface.
        // It is provided as a dummy implementation to satisfy the requirements of the Boost Numeric Ublass library.
        // The actual resizing should be handled by the container using this allocator.
    }

    size_type size() const noexcept {
        // This is not a standard allocator function and is only here to satisfy the Boost Numeric Ublass library's requirements.
        // The actual size management should be handled by the container using this allocator.
        // Returning a dummy value as the size is not tracked by the allocator.
        return 0;
    }

    std::size_t& operator[](size_type n) {
        // This is not a standard allocator function and is only here to satisfy the Boost Numeric Ublass library's requirements.
        // The actual element access should be handled by the container using this allocator.
        // Returning a reference to a dummy static variable as this operator should not be used directly.
        static std::size_t dummy;
        return dummy;
    }

    const std::size_t& operator[](size_type n) const {
        // This is not a standard allocator function and is only here to satisfy the Boost Numeric Ublass library's requirements.
        // The actual element access should be handled by the container using this allocator.
        // Returning a reference to a dummy static variable as this operator should not be used directly.
        static std::size_t dummy;
        return dummy;
    }

    // Add swap method for compatibility with Boost Numeric Ublass
    void swap(custom_allocator& other) noexcept {
        std::swap(static_cast<std::allocator<std::size_t>&>(*this), static_cast<std::allocator<std::size_t>&>(other));
    }

    // Add begin method for compatibility with Boost Numeric Ublass
    iterator begin() noexcept {
        // Since the allocator does not actually hold the data, this method should not be used.
        // It is only provided for compatibility purposes and will return a null iterator.
        return nullptr;
    }
};

#endif // CUSTOM_ALLOCATOR_H
