#pragma once

// Ensure macros are undefined first to avoid redefinition issues
#undef MSVC_CONSTEXPR
#undef NO_APPLE_CONSTEXPR
#undef NO_CLANG_CONSTEXPR

// Define platform-specific constexpr handling
#if defined(_MSC_VER)
    #define MSVC_CONSTEXPR inline
#else
    #define MSVC_CONSTEXPR constexpr
#endif

#if defined(__APPLE__)
    #define NO_APPLE_CONSTEXPR inline
#else
    #define NO_APPLE_CONSTEXPR constexpr
#endif

#if defined(__clang__)
    #define NO_CLANG_CONSTEXPR inline
#else
    #define NO_CLANG_CONSTEXPR constexpr
#endif
