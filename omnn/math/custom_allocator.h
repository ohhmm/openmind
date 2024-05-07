#ifndef CUSTOM_ALLOCATOR_H
#define CUSTOM_ALLOCATOR_H

#include <memory>
#include <type_traits>
#include <cstddef>
#include <limits>
#include <new>

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
        ::new((void*)p) U(std::forward<Args>(args)...);
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
        std::swap(*this, other);
    }

    // Add begin method for compatibility with Boost Numeric Ublass
    pointer begin() noexcept {
        // Since the allocator does not actually hold the data, this method should not be used.
        // It is only provided for compatibility purposes and will return a null iterator.
        return nullptr;
    }
};

// Specialization for std::size_t
template <>
class custom_allocator<std::size_t> {
public:
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = std::size_t*;
    using const_pointer = const std::size_t*;
    using reference = std::size_t&;
    using const_reference = const std::size_t&;
    using value_type = std::size_t;
    using iterator = std::size_t*;
    using const_iterator = const std::size_t*;

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
        return static_cast<pointer>(::operator new(n * sizeof(std::size_t), std::nothrow));
    }

    void deallocate(pointer p, size_type n) noexcept {
        ::operator delete(p);
    }

    size_type max_size() const noexcept {
        return std::numeric_limits<size_type>::max() / sizeof(std::size_t);
    }

    template <class U, class... Args>
    void construct(U* p, Args&&... args) {
        ::new((void*)p) U(std::forward<Args>(args)...);
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
        std::swap(*this, other);
    }

    // Add begin method for compatibility with Boost Numeric Ublass
    pointer begin() noexcept {
        // Since the allocator does not actually hold the data, this method should not be used.
        // It is only provided for compatibility purposes and will return a null iterator.
        return nullptr;
    }

    using void_pointer = void*;
    using const_void_pointer = const void*;
};

namespace std {
    template <typename T>
    struct allocator_traits<custom_allocator<T>> : std::allocator_traits<std::allocator<T>> {
        using allocator_type = custom_allocator<T>;
        using value_type = T;
        using pointer = typename allocator_type::pointer;
        using const_pointer = typename allocator_type::const_pointer;
        using void_pointer = typename std::allocator<T>::void_pointer;
        using const_void_pointer = typename std::allocator<T>::const_void_pointer;
        using difference_type = typename allocator_type::difference_type;
        using size_type = typename allocator_type::size_type;
        using propagate_on_container_move_assignment = std::true_type;
        using is_always_equal = std::is_empty<allocator_type>;

        template <class U, class... Args>
        static void construct(allocator_type& a, U* p, Args&&... args) {
            a.construct(p, std::forward<Args>(args)...);
        }

        template <class U>
        static void destroy(allocator_type& a, U* p) {
            a.destroy(p);
        }
    };

    template <>
    struct allocator_traits<custom_allocator<std::size_t>> : std::allocator_traits<std::allocator<std::size_t>> {
        using allocator_type = custom_allocator<std::size_t>;
        using value_type = std::size_t;
        using pointer = typename allocator_type::pointer;
        using const_pointer = typename allocator_type::const_pointer;
        using void_pointer = void*;
        using const_void_pointer = const void*;
        using difference_type = typename allocator_type::difference_type;
        using size_type = typename allocator_type::size_type;
        using propagate_on_container_move_assignment = std::true_type;
        using is_always_equal = std::is_empty<allocator_type>;

        template <class U, class... Args>
        static void construct(allocator_type& a, U* p, Args&&... args) {
            a.construct(p, std::forward<Args>(args)...);
        }

        template <class U>
        static void destroy(allocator_type& a, U* p) {
            a.destroy(p);
        }
    };
}

#endif // CUSTOM_ALLOCATOR_H
