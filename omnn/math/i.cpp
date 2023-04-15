//
// Created by Sergej Krivonos on 5/26/18.
//

#include "i.h"

using namespace omnn::math;

constinit std::string_view MinusOneSq::SerializationName = "i";

std::pair<bool, Valuable> MinusOneSq::IsSummationSimplifiable(const Valuable& v) const {
    std::pair<bool, Valuable> is = {v.Is_i(), {}}; 
	if (is.first) {
        is.second = v * 2;
    } else if (v.IsInt() || v.IsConstant()) {
    } else {
        is = v.IsSummationSimplifiable(*this);
    }
    return is;
}

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

Valuable MinusOneSq::Sign() const {
	// TODO: "prove" that the i is not the value that should be returned here... 
	return constants::one; // in dinstinction between sign as dirrection and dimmension towards the dirrection, decided that the complex plane is a dimmension while +1 is its dirrection towards "increasing" raltively to zero
}

Valuable& MinusOneSq::sq() {
    return Become(Sq());
}
