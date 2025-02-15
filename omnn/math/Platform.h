#pragma once

// Define platform-specific function attributes
#if defined(_MSC_VER)
    #define MSVC_CONSTEXPR
    #define NO_APPLE_CONSTEXPR constexpr
    #define NO_CLANG_CONSTEXPR constexpr
#elif defined(__APPLE__) && !defined(__clang__)
    #define MSVC_CONSTEXPR constexpr
    #define NO_APPLE_CONSTEXPR
    #define NO_CLANG_CONSTEXPR constexpr
#elif defined(__clang__)
    #define MSVC_CONSTEXPR constexpr
    #define NO_APPLE_CONSTEXPR constexpr
    #define NO_CLANG_CONSTEXPR
#else
    #define MSVC_CONSTEXPR constexpr
    #define NO_APPLE_CONSTEXPR constexpr
    #define NO_CLANG_CONSTEXPR constexpr
#endif

// Define function specifiers for each platform
#ifdef _MSC_VER
    #define PLATFORM_INLINE __forceinline
#else
    #define PLATFORM_INLINE inline
#endif

// Ensure macros are defined for all cases
#ifndef MSVC_CONSTEXPR
    #define MSVC_CONSTEXPR constexpr
#endif

#ifndef NO_APPLE_CONSTEXPR
    #define NO_APPLE_CONSTEXPR constexpr
#endif

#ifndef NO_CLANG_CONSTEXPR
    #define NO_CLANG_CONSTEXPR constexpr
#endif
