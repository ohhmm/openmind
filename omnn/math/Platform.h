#pragma once

// Define platform-specific function attributes
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
