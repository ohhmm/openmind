#pragma once

// Define platform-specific function attributes
#if defined(_MSC_VER)
    #define MSVC_CONSTEXPR constexpr
    #define NO_APPLE_CONSTEXPR constexpr
    #define NO_CLANG_CONSTEXPR constexpr
#elif defined(__APPLE__)
    #define MSVC_CONSTEXPR constexpr
    #define NO_APPLE_CONSTEXPR constexpr
    #define NO_CLANG_CONSTEXPR constexpr
#elif defined(__clang__)
    #define MSVC_CONSTEXPR constexpr
    #define NO_APPLE_CONSTEXPR constexpr
    #define NO_CLANG_CONSTEXPR constexpr
#else
    #define MSVC_CONSTEXPR constexpr
    #define NO_APPLE_CONSTEXPR constexpr
    #define NO_CLANG_CONSTEXPR constexpr
#endif

// Define platform-specific overrides
#if defined(_MSC_VER) && !defined(__clang__)
    #undef MSVC_CONSTEXPR
    #define MSVC_CONSTEXPR
#endif

#if defined(__APPLE__) && !defined(__clang__)
    #undef NO_APPLE_CONSTEXPR
    #define NO_APPLE_CONSTEXPR
#endif

#if defined(__clang__)
    #undef NO_CLANG_CONSTEXPR
    #define NO_CLANG_CONSTEXPR
#endif
