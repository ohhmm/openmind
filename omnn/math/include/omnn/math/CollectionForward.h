#pragma once
#include <cstddef>

namespace omnn {
namespace math {

// Forward declarations
struct SumOrderComparator;
template<typename T, typename Comparator = SumOrderComparator, size_t N = 16>
class OptimizedCollection;
class Valuable;

}} // namespace omnn::math
