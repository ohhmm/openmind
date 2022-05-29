//
// Created by Sergej Krivonos on 5/26/18.
//

#include "i.h"

using namespace omnn::math;

constinit std::string_view MinusOneSq::SerializationName = "i";

std::pair<bool, Valuable> MinusOneSq::IsMultiplicationSimplifiable(const Valuable &v) const {
    std::pair<bool, Valuable> is;
    is.first = v.Is_i();
    if (is.first)
        is.second = -1;
    return is;
}

bool MinusOneSq::MultiplyIfSimplifiable(const Valuable& v)
{
    auto multiply = v.Is_i();
    if (multiply)
        Become(-1_v);
    else{
        auto is = v.IsMultiplicationSimplifiable(*this);
        multiply = is.first;
        if(multiply)
            Become(std::move(is.second));
    }
    return multiply;
}

Valuable MinusOneSq::Sq() const {
    return Integer(-1);
}

Valuable& MinusOneSq::sq() {
    return Become(Sq());
}
