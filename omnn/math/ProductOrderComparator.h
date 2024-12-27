#pragma once


namespace omnn::math {

class Valuable;

struct ProductOrderComparator {
    using index_t = int;
    static index_t Order(const Valuable&);
    bool operator()(const Valuable&, const Valuable&) const;
};

} // namespace omnn::math
