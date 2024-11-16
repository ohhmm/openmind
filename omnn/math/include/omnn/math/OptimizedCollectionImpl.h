#pragma once
#include <set>
#include <algorithm>
#include <iterator>
#include <memory>
#include "SmallVectorOptimization.h"

namespace omnn {
namespace math {

// Forward declarations
template<typename T, size_t N = 8> class OptimizedCollection;
class Valuable;

namespace detail {
    // Implementation details that don't require complete OptimizedCollection type
    template<typename T>
    struct CollectionTraits {
        using value_type = T;
        using reference = value_type&;
        using const_reference = const value_type&;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
    };
}

}} // namespace omnn::math
