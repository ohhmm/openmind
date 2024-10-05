//
// Created by Sergej Krivonos on 5/26/18.
//

#include "Euler.h"

using namespace omnn::math;

constinit std::string_view Euler::SerializationName = "e";

using namespace constant;
//    static const Euler e;


Valuable Euler::Sq() const {
    auto exponentiation = ptrs::make_shared<Exponentiation>(*this, 2);
    return Valuable(exponentiation);
}

Valuable& Euler::sq() { return Become(Sq()); }

std::pair<bool, Valuable> Euler::IsSummationSimplifiable(const Valuable& v) const {
	return {};
}

std::pair<bool, Valuable> Euler::IsMultiplicationSimplifiable(const Valuable& v) const {
    return {};
}

bool Euler::MultiplyIfSimplifiable(const Valuable& v) {
	return {};
}

