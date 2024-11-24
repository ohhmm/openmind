#pragma once

// Constexpr support macros for different compilers
#ifdef __APPLE__
    #define NO_APPLE_CONSTEXPR
#else
    #define NO_APPLE_CONSTEXPR constexpr
#endif

#ifdef _MSC_VER
    #define MSVC_CONSTEXPR
    #define NO_MSVC_CONSTEXPR
#else
    #define MSVC_CONSTEXPR constexpr
    #define NO_MSVC_CONSTEXPR constexpr
#endif

#ifdef __clang__
    #define NO_CLANG_CONSTEXPR
#else
    #define NO_CLANG_CONSTEXPR constexpr
#endif

// Common constexpr macro for all platforms
#if defined(__APPLE__) || defined(__clang__) || defined(_MSC_VER)
    #define PLATFORM_CONSTEXPR
#else
    #define PLATFORM_CONSTEXPR constexpr
#endif

// MSVC-specific macro for move constructors
#ifdef _MSC_VER
    #define MSVC_MOVE_CONSTEXPR
    #define IMPLEMENT __debugbreak()
#else
    #define MSVC_MOVE_CONSTEXPR constexpr
    #define IMPLEMENT throw std::runtime_error("Not implemented")
#endif
