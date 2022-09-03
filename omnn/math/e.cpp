//
// Created by Sergej Krivonos on 5/26/18.
//

#include "e.h"

using namespace omnn::math;

constinit std::string_view Euler::SerializationName = "e";

using namespace constant;
//    static const Euler e;

namespace {
auto eSq = Exponentiation{Euler(), 2};
}

Valuable Euler::Sq() const { return eSq; }

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

