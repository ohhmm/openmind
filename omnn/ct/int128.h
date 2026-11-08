#pragma once

#if defined(__SIZEOF_INT128__) && __SIZEOF_INT128__ == 16
#define OMNN_NATIVE_INT128 1
#else
#define OMNN_NATIVE_INT128 0
#endif

#if OMNN_NATIVE_INT128
namespace omnn {
using int128_t = __int128;
using uint128_t = unsigned __int128;
}

#else
#include <boost/multiprecision/cpp_int.hpp>
namespace omnn {
using int128_t = boost::multiprecision::cpp_int;
using uint128_t = boost::multiprecision::cpp_int;
}
#endif
