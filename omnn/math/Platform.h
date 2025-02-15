#pragma once

// Ensure macros are undefined first to avoid redefinition issues
#undef MSVC_CONSTEXPR
#undef NO_APPLE_CONSTEXPR
#undef NO_CLANG_CONSTEXPR

#ifdef _MSC_VER
#define MSVC_CONSTEXPR
#else
#define MSVC_CONSTEXPR constexpr
#endif

#ifdef __APPLE__
#define NO_APPLE_CONSTEXPR
#else
#define NO_APPLE_CONSTEXPR constexpr
#endif

#ifdef __clang__
#define NO_CLANG_CONSTEXPR
#else
#define NO_CLANG_CONSTEXPR constexpr
#endif
