#pragma once

// Define platform-specific function attributes
#if defined(_MSC_VER)
    #define MSVC_CONSTEXPR
    #define NO_APPLE_CONSTEXPR
    #define NO_CLANG_CONSTEXPR
#elif defined(__APPLE__)
    #define MSVC_CONSTEXPR
    #define NO_APPLE_CONSTEXPR
    #define NO_CLANG_CONSTEXPR
#elif defined(__clang__)
    #define MSVC_CONSTEXPR
    #define NO_APPLE_CONSTEXPR
    #define NO_CLANG_CONSTEXPR
#else
    #define MSVC_CONSTEXPR
    #define NO_APPLE_CONSTEXPR
    #define NO_CLANG_CONSTEXPR
#endif

// Define constexpr for non-platform cases
#ifndef MSVC_CONSTEXPR
    #define MSVC_CONSTEXPR constexpr
#endif
#ifndef NO_APPLE_CONSTEXPR
    #define NO_APPLE_CONSTEXPR constexpr
#endif
#ifndef NO_CLANG_CONSTEXPR
    #define NO_CLANG_CONSTEXPR constexpr
#endif
