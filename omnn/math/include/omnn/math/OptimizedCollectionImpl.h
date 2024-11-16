#pragma once
#include <set>
#include <algorithm>
#include <iterator>
#include <memory>
#include "OptimizedCollection.h"

namespace omnn {
namespace math {

namespace detail {
    // Implementation details for OptimizedCollection
    template<typename T>
    struct CollectionTraits {
        using value_type = T;
        using reference = value_type&;
        using const_reference = const value_type&;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using iterator = typename OptimizedCollection<T>::iterator;
        using const_iterator = typename OptimizedCollection<T>::const_iterator;
    };
}

}} // namespace omnn::math
