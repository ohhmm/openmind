#pragma once

#include <boost/mpl/accumulate.hpp>
#include <boost/mpl/arithmetic.hpp>
#include <boost/mpl/math/rational_c.hpp>

namespace boost{
namespace mpl{
namespace math{
    template <class Numerator, class Denominator>
    struct exp{
        using arg = rational_c<long_long_type, Numerator, Denominator>;

        using res = accumulate<
    };
}}}

double exponent(double x){
    double res = 1+x, prev, powx = x;
    double denominator = 1;
    double i = 1;
    do{
        prev = res;
        ++i;
        powx *= x;
        denominator *= i;
        res += powx / denominator;
    } while (res != prev);
    return res;
}
