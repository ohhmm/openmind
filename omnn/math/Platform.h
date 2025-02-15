#pragma once

// Define platform-specific function attributes
#if defined(_MSC_VER)
    #define MSVC_CONSTEXPR
#else
    #define MSVC_CONSTEXPR constexpr
#endif

#if defined(__APPLE__)
    #define NO_APPLE_CONSTEXPR
#else
    #define NO_APPLE_CONSTEXPR constexpr
#endif

#if defined(__clang__)
    #define NO_CLANG_CONSTEXPR
#else
    #define NO_CLANG_CONSTEXPR constexpr
#endif

// Define platform-specific overrides for special cases
#if defined(_MSC_VER) && !defined(__clang__)
    #undef MSVC_CONSTEXPR
    #define MSVC_CONSTEXPR inline
#endif

#if defined(__APPLE__) && !defined(__clang__)
    #undef NO_APPLE_CONSTEXPR
    #define NO_APPLE_CONSTEXPR inline
#endif

#if defined(__clang__)
    #undef NO_CLANG_CONSTEXPR
    #define NO_CLANG_CONSTEXPR inline
#endif
