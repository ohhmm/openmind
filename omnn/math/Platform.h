#pragma once

// Ensure macros are undefined first to avoid redefinition issues
#undef MSVC_CONSTEXPR
#undef NO_APPLE_CONSTEXPR
#undef NO_CLANG_CONSTEXPR

// Define platform-specific function attributes
#if defined(_MSC_VER)
    #define MSVC_CONSTEXPR __forceinline
    #define NO_APPLE_CONSTEXPR constexpr
    #define NO_CLANG_CONSTEXPR constexpr
#elif defined(__APPLE__)
    #define MSVC_CONSTEXPR constexpr
    #define NO_APPLE_CONSTEXPR __attribute__((always_inline))
    #define NO_CLANG_CONSTEXPR constexpr
#elif defined(__clang__)
    #define MSVC_CONSTEXPR constexpr
    #define NO_APPLE_CONSTEXPR constexpr
    #define NO_CLANG_CONSTEXPR __attribute__((always_inline))
#else
    #define MSVC_CONSTEXPR constexpr
    #define NO_APPLE_CONSTEXPR constexpr
    #define NO_CLANG_CONSTEXPR constexpr
#endif
