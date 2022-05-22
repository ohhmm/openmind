#include "PrincipalSurd.h"
#include "Product.h"

using namespace omnn::math;


PrincipalSurd::PrincipalSurd(const Valuable& radicand, const Valuable& index)
    : base(radicand, index)
{

}


std::ostream& PrincipalSurd::print_sign(std::ostream &out) const
{
    out << 'r';
    return out;
}

std::ostream& PrincipalSurd::print(std::ostream& out) const {
    if (_2 == 2)
        out << "sqrt(" << _1 << ')';
    else
        out << '(' << _2 << 'r' << _1 << ')';
    return out;
}

std::ostream& PrincipalSurd::code(std::ostream& out) const {
    if(_2 == 2)
        out << "sqrt(" << _1 << ')';
    else
        out << "pow(" << _1 << ", 1/(" << _2 << "))";
    return out;
}

std::pair<bool, Valuable> PrincipalSurd::IsMultiplicationSimplifiable(const Valuable& v) const {
    std::pair<bool, Valuable> is;
	is.first = v.IsRadical();
    if (is.first) {
        auto& surd = v.as<PrincipalSurd>();
        is.first = _1 == surd._1;
        if (is.first) {
            if (_2 == surd._2 && _2 == 2) {
                is.second = _1;
}
        }
    }
    return is;
}

void PrincipalSurd::optimize() {
    if (_1.IsInt() && _1.ca() < 0) {
        Become(Product{constants::i, {PrincipalSurd{-_1, _2}}});
        return;
    }
}

