#pragma once
#include <array>
#include <memory>
#include <thread>
#include <set>

namespace omnn {
namespace math {

// Forward declare for friend declaration
template<typename T, size_t N = 8> class OptimizedCollection;

// Small vector optimization to reduce heap allocations for small collections
template<typename T, size_t N = 16>
class SmallVector {
    friend class OptimizedCollection<T, N>;
    alignas(T) std::array<std::byte, sizeof(T) * N> buffer;
    T* data_ = reinterpret_cast<T*>(buffer.data());
    size_t size_ = 0;
    size_t capacity_ = N;

public:
    using value_type = T;
    using iterator = T*;
    using const_iterator = const T*;

    SmallVector() = default;
    ~SmallVector() {
        clear();
    }

    // Move constructor
    SmallVector(SmallVector&& other) noexcept
        : size_(other.size_)
        , capacity_(other.capacity_) {
        if (other.size_ <= N) {
            for (size_t i = 0; i < size_; ++i) {
                new (data_ + i) T(std::move(other.data_[i]));
                other.data_[i].~T();
            }
        }
        other.size_ = 0;
    }

    // Move assignment
    SmallVector& operator=(SmallVector&& other) noexcept {
        if (this != &other) {
            clear();
            size_ = other.size_;
            capacity_ = other.capacity_;
            if (other.size_ <= N) {
                for (size_t i = 0; i < size_; ++i) {
                    new (data_ + i) T(std::move(other.data_[i]));
                    other.data_[i].~T();
                }
            }
            other.size_ = 0;
        }
        return *this;
    }

    bool empty() const { return size_ == 0; }
    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }

    iterator begin() { return data_; }
    iterator end() { return data_ + size_; }
    const_iterator begin() const { return data_; }
    const_iterator end() const { return data_ + size_; }
    const_iterator cbegin() const { return data_; }
    const_iterator cend() const { return data_ + size_; }

    void push_back(const T& value) {
        if (size_ < N) {
            new (data_ + size_) T(value);
            ++size_;
        } else {
            throw std::bad_alloc();
        }
    }

    void push_back(T&& value) {
        if (size_ < N) {
            new (data_ + size_) T(std::move(value));
            ++size_;
        } else {
            throw std::bad_alloc();
        }
    }

    template<typename... Args>
    T& emplace_back(Args&&... args) {
        if (size_ < N) {
            T* p = new (data_ + size_) T(std::forward<Args>(args)...);
            ++size_;
            return *p;
        }
        throw std::bad_alloc();
    }

    void pop_back() {
        if (size_ > 0) {
            --size_;
            data_[size_].~T();
        }
    }

    T& operator[](size_t index) { return data_[index]; }
    const T& operator[](size_t index) const { return data_[index]; }

    void clear() {
        for (size_t i = 0; i < size_; ++i) {
            data_[i].~T();
        }
        size_ = 0;
    }

    // Extract functionality
    T extract(iterator pos) {
        T value = std::move(*pos);
        std::move(pos + 1, end(), pos);
        --size_;
        return value;
    }

    // Erase functionality
    iterator erase(iterator pos) {
        std::move(pos + 1, end(), pos);
        --size_;
        return pos;
    }

    // Prevent copying
    SmallVector(const SmallVector&) = delete;
    SmallVector& operator=(const SmallVector&) = delete;
};

// Helper function to determine optimal async threshold based on hardware
inline size_t get_async_threshold() {
    static const size_t threshold = std::max(100u, std::thread::hardware_concurrency() * 25);
    return threshold;
}

} // namespace math
} // namespace omnn
