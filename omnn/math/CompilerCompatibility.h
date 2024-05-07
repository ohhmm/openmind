#pragma once

// Define MSVC_CONSTEXPR as constexpr for non-MSVC compilers
#ifndef MSVC_CONSTEXPR
#define MSVC_CONSTEXPR constexpr
#endif

// Define NO_APPLE_CONSTEXPR and NO_CLANG_CONSTEXPR as empty for non-Apple Clang compilers
#ifndef NO_APPLE_CONSTEXPR
#define NO_APPLE_CONSTEXPR
#endif

#ifndef NO_CLANG_CONSTEXPR
#define NO_CLANG_CONSTEXPR
#endif

// Define APPLE_CONSTEXPR only when compiling on Apple platforms
#ifdef __APPLE__
#define APPLE_CONSTEXPR constexpr
#else
#define APPLE_CONSTEXPR
#endif
