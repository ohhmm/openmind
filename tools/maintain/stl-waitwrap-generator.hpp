#pragma once
#include <coroutine>
#if defined(__cpp_lib_generator) && __has_include(<generator>)
#include <generator>
#elif __has_include(<experimental/generator>)
#include <experimental/generator>
using namespace std::experimental;
#else

#include <exception>
#include <utility>

namespace std {
template<typename T>
struct generator
{
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    struct promise_type // required
    {
        T value_;
        std::exception_ptr exception_;

        generator get_return_object()
        {
            return generator(handle_type::from_promise(*this));
        }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() { exception_ = std::current_exception(); }

        template<std::convertible_to<T> From> // C++20 concept
        std::suspend_always yield_value(From&& from)
        {
            value_ = std::forward<From>(from); // caching the result in promise
            return {};
        }
        void return_void() {}
    };

    handle_type h_;

    generator(handle_type h) : h_(h) {}
    ~generator() { if (h_) h_.destroy(); }

    generator(const generator&) = delete;            // non-copyable
    generator& operator=(const generator&) = delete; // non-copyable
    generator(generator&& other) noexcept : h_(other.h_) { other.h_ = nullptr; }
    generator& operator=(generator&& other) noexcept {
        if (this != &other) {
            if (h_) h_.destroy();
            h_ = other.h_;
            other.h_ = nullptr;
        }
        return *this;
    }

    struct iterator
    {
        handle_type h_;
        bool done_;

        iterator(handle_type h, bool done) : h_(h), done_(done) {}

        iterator& operator++()
        {
            h_(); // Resume the coroutine
            done_ = h_.done();
            if (h_.promise().exception_)
                std::rethrow_exception(h_.promise().exception_);
            return *this;
        }

        const T& operator*() const { return h_.promise().value_; }
        const T* operator->() const { return &h_.promise().value_; }

        bool operator==(const iterator& other) const { return done_ == other.done_; }
        bool operator!=(const iterator& other) const { return !(*this == other); }
    };

    iterator begin()
    {
        if (h_) {
            h_(); // Start the coroutine
            if (h_.promise().exception_)
                std::rethrow_exception(h_.promise().exception_);
        }
        return iterator(h_, h_.done());
    }

    iterator end() { return iterator(h_, true); }
};

} // namespace std
#endif