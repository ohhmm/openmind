#pragma once
#include <cstddef>

namespace omnn {
namespace math {

// Forward declare Valuable first
class Valuable;

// Complete comparator definitions with forward-declared Valuable
struct SumOrderComparator {
    bool operator()(const Valuable& lhs, const Valuable& rhs) const;
};

struct ProductOrderComparator {
    bool operator()(const Valuable& lhs, const Valuable& rhs) const;
};

// Forward declarations with complete template parameters
template<typename T, typename Comparator = SumOrderComparator, size_t N = 16>
class OptimizedCollection;

}} // namespace omnn::math
