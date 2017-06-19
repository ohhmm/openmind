#pragma once

#include <boost/mpl/accumulate.hpp>
#include <boost/mpl/arithmetic.hpp>
#include <boost/mpl/math/rational_c.hpp>
#include <boost/ratio/mpl/abs.hpp>

namespace boost{
namespace mpl{
namespace math{
template <class X>
struct simple_sigmoid{
    using one = ratio<1,1>;
    using type = divides<X, minus<one, abs<X>>>;
};
}}}