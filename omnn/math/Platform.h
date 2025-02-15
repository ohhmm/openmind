#pragma once

// Define platform-specific function attributes with defaults
#if defined(_MSC_VER)
    #define MSVC_CONSTEXPR constexpr
    #define NO_APPLE_CONSTEXPR constexpr
    #define NO_CLANG_CONSTEXPR constexpr
#elif defined(__APPLE__) && !defined(__clang__)
    #define MSVC_CONSTEXPR constexpr
    #define NO_APPLE_CONSTEXPR inline
    #define NO_CLANG_CONSTEXPR constexpr
#elif defined(__clang__)
    #define MSVC_CONSTEXPR constexpr
    #define NO_APPLE_CONSTEXPR constexpr
    #define NO_CLANG_CONSTEXPR inline
#else
    #define MSVC_CONSTEXPR constexpr
    #define NO_APPLE_CONSTEXPR constexpr
    #define NO_CLANG_CONSTEXPR constexpr
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
