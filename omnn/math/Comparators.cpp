#include "omnn/math/CollectionForward.h"
#include "omnn/math/Valuable.h"

namespace omnn {
namespace math {

bool SumOrderComparator::operator()(const Valuable& lhs, const Valuable& rhs) const {
    return lhs.Hash() < rhs.Hash();
}

bool ProductOrderComparator::operator()(const Valuable& lhs, const Valuable& rhs) const {
    return lhs.Hash() < rhs.Hash();
}

}} // namespace omnn::math
