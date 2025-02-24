#pragma once

#ifdef _MSC_VER
#define MSVC_CONSTEXPR constexpr
#else
#define MSVC_CONSTEXPR
#endif

#ifdef __APPLE__
#define NO_APPLE_CONSTEXPR
#define APPLE_CONSTEXPR constexpr
#else
#define NO_APPLE_CONSTEXPR constexpr
#define APPLE_CONSTEXPR
#endif

#ifdef __clang__
#define NO_CLANG_CONSTEXPR
#else
#define NO_CLANG_CONSTEXPR constexpr
#endif
