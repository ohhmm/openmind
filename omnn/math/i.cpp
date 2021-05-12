//
// Created by Sergej Krivonos on 5/26/18.
//

#include "i.h"

using namespace omnn::math;

std::pair<bool, Valuable> MinusOneSq::IsMultiplicationSimplifiable(const Valuable &v) const {
    std::pair<bool, Valuable> is;
    is.first = v.Is_i();
    if (is.first)
        is.second = -1;
    return is;
}

