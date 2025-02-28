//
// Created by Devin AI on February 28, 2025.
//

#pragma once

// Platform-specific macro definitions for constexpr usage
#if defined(_MSC_VER) || defined(MSVC)
    // Microsoft Visual C++ compiler
    #define MSVC_CONSTEXPR constexpr
    #define NO_MSVC_CONSTEXPR /* empty */
    #define APPLE_CONSTEXPR /* empty */
    #define NO_APPLE_CONSTEXPR constexpr
    #define NO_CLANG_CONSTEXPR constexpr
#else
    #define MSVC_CONSTEXPR /* empty */
    #define NO_MSVC_CONSTEXPR constexpr
    #if defined(__APPLE__)
        // Apple platform (macOS, iOS, etc.)
        #define APPLE_CONSTEXPR constexpr
        #define NO_APPLE_CONSTEXPR /* empty */
        #define NO_CLANG_CONSTEXPR /* empty */
    #else
        // Other platforms (Linux, etc.)
        #define APPLE_CONSTEXPR /* empty */
        #define NO_APPLE_CONSTEXPR constexpr
        #if defined(__clang__)
            #define NO_CLANG_CONSTEXPR /* empty */
        #else
            #define NO_CLANG_CONSTEXPR constexpr
        #endif
    #endif
#endif
