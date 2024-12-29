#pragma once

namespace omnn::math {

class Valuable;

struct SumOrderComparator {
    using TypeOrderComparator = bool(*)(const Valuable&, const Valuable&);
    static const TypeOrderComparator toc;  // type order comparator
    bool operator()(const Valuable&, const Valuable&) const;
};

} // namespace omnn::math
