#pragma once
#include "Valuable.h"

namespace omnn {
namespace math {

struct SumOrderComparator {
    bool operator()(const Valuable& lhs, const Valuable& rhs) const {
        return lhs.Hash() < rhs.Hash();
    }
};

struct ProductOrderComparator {
    bool operator()(const Valuable& lhs, const Valuable& rhs) const {
        return lhs.Hash() < rhs.Hash();
    }
};

}} // namespace omnn::math
