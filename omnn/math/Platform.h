#pragma once

// Ensure macros are undefined first to avoid redefinition issues
#undef MSVC_CONSTEXPR
#undef NO_APPLE_CONSTEXPR
#undef NO_CLANG_CONSTEXPR

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
#ifdef __cplusplus
    #ifndef MSVC_CONSTEXPR
        #define MSVC_CONSTEXPR constexpr
    #endif
    #ifndef NO_APPLE_CONSTEXPR
        #define NO_APPLE_CONSTEXPR constexpr
    #endif
    #ifndef NO_CLANG_CONSTEXPR
        #define NO_CLANG_CONSTEXPR constexpr
    #endif
#endif
