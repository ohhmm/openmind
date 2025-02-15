#pragma once

// Ensure macros are undefined first to avoid redefinition issues
#undef MSVC_CONSTEXPR
#undef NO_APPLE_CONSTEXPR
#undef NO_CLANG_CONSTEXPR

// Define platform-specific function attributes
#if defined(_MSC_VER)
    #define MSVC_CONSTEXPR
    #define NO_APPLE_CONSTEXPR constexpr
    #define NO_CLANG_CONSTEXPR constexpr
#elif defined(__APPLE__)
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

// Define empty macros for non-platform cases
#ifndef MSVC_CONSTEXPR
    #define MSVC_CONSTEXPR
#endif
#ifndef NO_APPLE_CONSTEXPR
    #define NO_APPLE_CONSTEXPR
#endif
#ifndef NO_CLANG_CONSTEXPR
    #define NO_CLANG_CONSTEXPR
#endif
