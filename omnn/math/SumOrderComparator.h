#pragma once
#include <typeindex>

namespace omnn::math {

class Valuable;

struct SumOrderComparator {
    bool operator()(const Valuable&, const Valuable&) const;
};

} // namespace omnn::math
