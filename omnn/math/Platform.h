#pragma once

// Define platform-specific function attributes
#if defined(_MSC_VER)
    #define MSVC_CONSTEXPR inline
    #define NO_APPLE_CONSTEXPR inline
    #define NO_CLANG_CONSTEXPR inline
#elif defined(__APPLE__)
    #define MSVC_CONSTEXPR inline
    #define NO_APPLE_CONSTEXPR inline
    #define NO_CLANG_CONSTEXPR inline
#elif defined(__clang__)
    #define MSVC_CONSTEXPR inline
    #define NO_APPLE_CONSTEXPR inline
    #define NO_CLANG_CONSTEXPR inline
#else
    #define MSVC_CONSTEXPR inline
    #define NO_APPLE_CONSTEXPR inline
    #define NO_CLANG_CONSTEXPR inline
#endif
