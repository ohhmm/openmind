//
// Created by Sergej Krivonos on 5/26/18.
//

#include "i.h"
#include "PrincipalSurd.h"
#include "Sum.h"


using namespace omnn::math;

constinit std::string_view MinusOneSurd::SerializationName = "i";

Valuable& MinusOneSurd::operator^=(const Valuable& v) {
    if (v == constants::minus_1) {
        Become(Product{*this, v});
    } else {
        Become(Exponentiation{*this, v});
    }
    return *this;
}

std::pair<bool, Valuable> MinusOneSurd::IsSummationSimplifiable(const Valuable& v) const {
    std::pair<bool, Valuable> is = {v.Is_i(), {}}; 
	if (is.first) {
        is.second = v * 2;
    } else if (v.IsInt() || v.IsConstant()) {
    } else {
        is = v.IsSummationSimplifiable(*this);
    }
    return is;
}

std::pair<bool, Valuable> MinusOneSurd::IsMultiplicationSimplifiable(const Valuable &v) const {
    std::pair<bool, Valuable> is;
    is.first = v.Is_i();
    if (is.first)
        is.second = -1;
    return is;
}

bool MinusOneSurd::MultiplyIfSimplifiable(const Valuable& v)
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

Valuable MinusOneSurd::Sq() const {
    return constants::minus_1;
}

namespace {
const Valuable sqrt_i = 
    // Fraction{Sum{1, MinusOneSurd()}, PrincipalSurd(2)};
    PrincipalSurd(-1,4);
}
Valuable MinusOneSurd::Sqrt() const { return sqrt_i; }

Valuable MinusOneSurd::Sign() const {
	// TODO: "prove" that the i is not the value that should be returned here... 
	return constants::one; // in distinction between sign as direction and dimension towards the direction, decided that the complex plane is a dimension while +1 is its direction towards "increasing" relatively to zero
}

Valuable& MinusOneSurd::sq() {
    return Become(Sq());
}

Valuable MinusOneSurd::abs() const {
    return constants::one;
}
